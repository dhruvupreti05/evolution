# Simulating Evolution with Neuroevolution, Natural Selection, and Emergent Survival Behavior

## Current Research and Progress

The goal of this project is to build an artificial ecosystem where simple agents begin with limited survival behavior and gradually become better adapted to their environment through evolution. The simulation starts with a grid-based world containing humans, predators, water, floods, droughts, and eventually food, prey, reproduction, inherited traits, and learned behavior. The long-term idea is not just to make a game-like simulation, but to create a controlled environment where evolutionary pressure can be studied directly.

At the current stage, the project is focused on building the world itself. The simulation already has the basic structure of a visual environment: a grid, generated terrain features such as lakes, agents moving through the world, predators, environmental events such as flooding and drought, and a statistics panel for tracking population-level behavior. This matters because the learning algorithm will only be meaningful if the world produces real pressure. If water is always available, food is always nearby, predators are easy to avoid, and reproduction is automatic, then evolution has nothing interesting to optimize. The environment has to be harsh enough that different strategies succeed or fail.

The project is being built around the idea that intelligence should emerge from survival requirements rather than being hand-coded. Instead of directly programming a human agent to “know” how to survive, the agent should have a small decision-making system. That system receives inputs from the environment, produces actions, and is evaluated based on whether the agent survives, finds resources, avoids danger, and reproduces. Over many generations, better-performing decision systems should become more common.

This places the project in the same broad research area as artificial life, evolutionary computation, reinforcement learning, neuroevolution, and agent-based modeling. Artificial life research studies how lifelike behavior can emerge from simple rules. Evolutionary computation studies how populations of candidate solutions can be improved through selection, mutation, and inheritance. Reinforcement learning studies how agents can learn through reward and punishment. Neuroevolution combines these ideas by using evolutionary algorithms to optimize neural networks.

The most relevant algorithm for the project is **NEAT**, which stands for **NeuroEvolution of Augmenting Topologies**. NEAT was introduced by Kenneth O. Stanley and Risto Miikkulainen in 2002 in the paper *Evolving Neural Networks Through Augmenting Topologies*. The central idea is that evolution should not only tune the weights of a neural network, but also evolve the structure of the network itself. This is important for the project because the best survival strategy may not require a large fixed neural network at the beginning. A simple creature should start with a simple brain. Over time, if more complexity is useful, evolution can add hidden nodes, add new connections, and create more complex behavior.

Recent research has also shown that evolutionary approaches remain relevant even in the age of deep learning. Deep learning usually depends on gradient descent and backpropagation, which work extremely well when there is a differentiable loss function and large amounts of training data. But survival in an artificial ecosystem is not naturally a supervised learning problem. There is no dataset of correct actions for every situation. Instead, the agent has to act, live with the consequences, and be judged by its long-term performance. This is exactly the kind of situation where neuroevolution is useful.

The project is therefore not trying to copy biological evolution perfectly. It is trying to build a simplified experimental system where some of the central forces of evolution can be made visible: selection, mutation, inherited variation, survival pressure, reproduction, resource competition, environmental change, and eventually gene flow and genetic drift. The simulation becomes a small artificial world where we can ask questions like:

- Do agents learn to move toward water before they die of thirst?
- Do predators create enough pressure for avoidance behavior to evolve?
- Does a changing environment favor general strategies over narrow strategies?
- Does adding memory improve survival?
- Does speciation protect new strategies before they become competitive?
- Does population diversity help the ecosystem recover after disasters?
- Can simple rules produce complex population-level behavior?

That is the main purpose of the project: to connect the theory of evolution and neuroevolution to a working simulation where the results can be seen, measured, and improved over time.

## Theory Background: Evolution, Neural Networks, NEAT, and Deep Learning

### Evolution as an Optimization Process

Biological evolution is not an intentional search process, but it can be modeled computationally as one. A population contains variation. Some individuals survive and reproduce more successfully than others. Traits are inherited, but inheritance is not perfect because mutation introduces new variation. Over many generations, traits that improve reproductive success become more common.

For this project, the key evolutionary mechanisms are:

1. **Natural selection**: agents with better survival behavior are more likely to reproduce.
2. **Mutation**: offspring are not exact copies of their parents; their brains or traits can randomly change.
3. **Genetic drift**: traits can become more or less common by chance, especially in small populations.
4. **Gene flow**: traits can move between groups when populations mix or reproduce across subgroups.
5. **Recombination/crossover**: offspring can inherit information from more than one parent.

NEAT mainly implements selection, mutation, and crossover. Speciation in NEAT also preserves diversity, which indirectly helps with drift-like behavior and prevents the population from collapsing too quickly into one strategy. Gene flow is not automatically modeled unless the project explicitly creates partially separated populations or islands that occasionally exchange individuals.

In the simulation, a “genome” can encode either physical traits, neural network structure, neural network weights, or all of these. The “phenotype” is the actual agent produced from the genome: its visible behavior, body traits, speed, perception, and decision-making network. Fitness is the measure used to decide which agents performed well enough to reproduce.

The hard part is choosing a fitness function that does not cheat. If the fitness function directly rewards “move toward water,” then the agent is not really discovering survival behavior; it is being told what to do. A better fitness function rewards outcomes rather than instructions. For example, the simulation can reward living longer, staying hydrated, eating enough food, escaping predators, reproducing, and helping offspring survive. Then the agent has to discover the behavior that produces those outcomes.

### Neural Networks as Agent Brains

A neural network is a function that maps inputs to outputs. In this project, the inputs are what the agent senses about the world, and the outputs are the actions the agent chooses.

A simple agent brain might receive inputs such as:

- distance to nearest water
- direction to nearest water
- distance to nearest food
- direction to nearest food
- distance to nearest predator
- direction to nearest predator
- current hunger
- current thirst
- current energy
- current age
- whether the current tile is dangerous
- whether a flood or drought is active
- nearby population density

The outputs might represent actions such as:

- move up
- move down
- move left
- move right
- stay still
- flee
- seek food
- seek water
- reproduce

At the beginning, the neural networks should probably be small. A first version might have only input nodes connected directly to output nodes. This is useful because the simulation should not begin with a complicated brain before the environment has proven that complexity is needed. If a direct input-output network is enough for basic survival, that should be discovered. If more complex behavior requires hidden neurons, memory, recurrence, or deeper networks, those can be added later.

This is one reason NEAT is a good fit. NEAT starts with simple networks and adds complexity only when mutation and selection make that complexity useful.

### What Deep Learning Means Here

Deep learning usually refers to neural networks with multiple layers. A deep neural network can learn complex representations by transforming the input through several hidden layers before producing an output. In image recognition, language modeling, and many supervised learning tasks, deep learning is extremely powerful because there is a clear loss function and a large dataset.

This project is different. There is no fixed training set. The agent is not given examples like:

```text
state: predator nearby, water far away, hunger low
correct action: move away from predator
```

Instead, the agent has to act in the environment, and the quality of its behavior is measured indirectly. This makes the project closer to reinforcement learning and neuroevolution than standard supervised deep learning.

Deep learning can still become useful later in the project. For example, if agents eventually receive visual input from the grid instead of hand-designed sensory values, then a deeper network could process local spatial patterns. If agents need memory, recurrent neural networks or attention-based models could be tested. If the environment becomes large and complex, a deeper architecture might learn internal features such as “safe region,” “danger corridor,” “high-resource zone,” or “migration route.”

However, the project should not jump to deep learning too early. A large deep network can make the simulation harder to debug. If behavior improves, it may be unclear whether the improvement came from the learning algorithm, the fitness function, the environment design, or accidental shortcuts. A better path is to begin with a small interpretable neural system and then scale up only when the simple version has reached its limits.

### Neuroevolution

Neuroevolution means evolving neural networks using evolutionary algorithms. Instead of using backpropagation to calculate gradients, neuroevolution treats each neural network as an individual in a population. Each individual is tested in the environment. The better individuals are selected to create offspring. The offspring inherit network properties from their parents, with random mutations.

The basic loop is:

1. Create an initial population of neural networks.
2. Place each network inside an agent or group of agents.
3. Run the simulation and measure performance.
4. Assign a fitness score.
5. Select the best networks.
6. Produce offspring through mutation and crossover.
7. Replace weak networks with new ones.
8. Repeat for many generations.

The benefit is that the system does not need labeled examples or differentiable rewards. The network only needs a fitness score. This is especially useful for survival simulations because survival behavior is long-term and indirect. An agent may make many small decisions before the final result becomes clear.

The weakness is that evolution can be computationally expensive. Many individuals must be evaluated, and each evaluation may require running a simulation for a long time. The project will need careful performance design, especially if the number of agents, predators, resources, and generations grows.

### The NEAT Algorithm

NEAT is a specific neuroevolution algorithm designed to evolve both neural network weights and neural network topology. A topology is the structure of the network: which nodes exist and which connections exist between them.

Before NEAT, many neuroevolution methods used a fixed network structure. The algorithm would evolve only the weights. That approach can work, but it forces the programmer to choose the network size ahead of time. If the network is too small, it may not be capable of solving the task. If it is too large, evolution has to search through too many unnecessary parameters.

NEAT avoids this by starting simple and complexifying over time.

The three core ideas of NEAT are:

1. **Historical markings for genes**
2. **Speciation to protect innovation**
3. **Incremental complexification from simple networks**

#### Genomes and Networks

In NEAT, each neural network is encoded as a genome. The genome usually contains two main kinds of genes:

- **Node genes**: input nodes, output nodes, and hidden nodes.
- **Connection genes**: directed links between nodes, each with a weight, enabled/disabled status, and innovation number.

A connection gene might store:

```text
input node: 3
output node: 7
weight: -0.42
enabled: true
innovation number: 15
```

The innovation number is one of NEAT’s most important ideas. It records when a structural mutation first appeared historically. If two different genomes contain genes with the same innovation number, NEAT knows those genes came from the same original mutation.

This solves a major crossover problem. If two networks have different structures, it is difficult to align their genes. Innovation numbers make alignment possible.

#### Mutation

NEAT uses several kinds of mutation.

A **weight mutation** changes the strength of an existing connection. This is similar to normal neural network optimization, except it is random rather than gradient-based.

An **add-connection mutation** creates a new connection between two previously unconnected nodes.

An **add-node mutation** splits an existing connection. The old connection is disabled, a new hidden node is inserted, and two new connections are created. This allows the network to become deeper and more complex over time.

For example, suppose there is a connection:

```text
A -> B
```

An add-node mutation might turn it into:

```text
A -> C -> B
```

where `C` is a new hidden node.

This is important because complexity is introduced gradually. The system does not need to guess the final brain architecture before evolution begins.

#### Crossover

Crossover combines genes from two parent genomes. When both parents have matching genes, the child may inherit from either parent. When one parent has extra genes, those genes may be inherited depending on which parent has better fitness.

Because NEAT uses innovation numbers, crossover can align genes even when networks have different topologies. This is much cleaner than trying to line up two neural networks based only on node order.

#### Speciation

A major problem in evolution is that new innovations are often bad at first. A new hidden node or connection might not immediately improve performance because the rest of the network has not adapted to use it yet. If the population only keeps the highest-performing individuals at every moment, new structures may be eliminated before they have time to become useful.

NEAT solves this with speciation. Networks are grouped into species based on genetic similarity. Individuals mostly compete against other members of their own species, not against the entire population. This protects new structural innovations long enough for them to improve.

In the project, speciation is especially useful because survival strategies may differ. One group of agents might evolve to stay near water. Another might evolve to move constantly. Another might evolve to avoid predators first and seek resources second. If only one strategy is immediately best, all other strategies might disappear too early. Speciation helps preserve multiple approaches.

#### Fitness Sharing

Within a species, fitness is adjusted so that large species do not automatically dominate the whole population. This encourages diversity. If many individuals are nearly identical, they share the reward. If a smaller species has a promising innovation, it has a chance to survive.

For the project, this is important because diversity is not just an algorithmic detail. It is part of the biological story. A population with diverse strategies may be more resilient when the environment changes.

#### Why NEAT Is a Good Match for This Project

NEAT fits this project because:

- The agents need to learn from long-term survival outcomes.
- There is no labeled dataset of correct actions.
- The best network structure is not known in advance.
- The world can become more complex over time.
- Different survival strategies should be allowed to coexist.
- Evolutionary diversity is part of the project’s purpose.

NEAT also matches the educational purpose of the project. It makes evolution visible. You can inspect genomes, track species, visualize network growth, and compare survival behavior across generations.

### NEAT Compared with Standard Deep Learning

Standard deep learning usually starts with a fixed architecture. The training process changes the weights using gradient descent. This works well when a loss function can be calculated directly.

NEAT starts with a population of networks. It changes both weights and structure using mutation, crossover, and selection. It does not require gradients.

A simple comparison:

| Feature | Standard Deep Learning | NEAT / Neuroevolution |
|---|---|---|
| Training signal | Loss function | Fitness score |
| Learning method | Backpropagation | Selection, mutation, crossover |
| Network structure | Usually fixed | Evolves over time |
| Data requirement | Often needs many examples | Needs environment evaluations |
| Good for | Classification, prediction, differentiable tasks | Control, survival, sparse rewards, artificial life |
| Main weakness | Needs differentiable objective and data | Can be computationally expensive |

For this project, NEAT should probably be used first. Deep learning can be introduced later as the agent brain becomes more complex.

## Project Description and Eventual Goals

### Core Purpose

The core purpose of the project is to simulate evolution through artificial agents that must survive in a changing environment. The agents should not be manually programmed with all the correct behaviors. Instead, they should inherit and mutate decision-making systems, and the environment should determine which behaviors survive.

The project is about making evolution measurable and visible. The simulation should show not only individual movement, but also population-level trends: how many humans are alive, how many predators are alive, how many agents die from thirst, how many die from predators, how often reproduction happens, which species dominate, and how behavior changes over time.

A finished version of the project should be able to produce results like:

- average lifespan increases across generations
- agents become better at finding water
- agents learn to avoid predators
- agents adapt differently under flood and drought conditions
- population crashes occur when environmental pressure is too high
- different species or lineages develop different strategies
- more complex neural networks emerge when the environment requires them

The simulation does not need to perfectly reproduce biology. Instead, it should be a simplified model where evolutionary ideas can be tested.

### Current World Model

The world is a grid. Each cell can represent terrain, water, resource blocks, agents, predators, or event effects. Lakes are generated as clumps of tiles. This is a good design choice because many natural structures appear as connected regions rather than isolated single cells. The same clump-generation logic can later be reused for forests, food patches, mountains, obstacles, shelters, or disease zones.

The current world already includes the idea of environmental events:

- **Floods**: water expands or affects more cells, changing movement and survival conditions.
- **Droughts**: water becomes scarce or less useful, increasing pressure on agents.

These events are important because evolution becomes more interesting when the environment changes. A strategy that works in a stable world may fail in a changing world. If water is always in the same place and predators behave the same way forever, agents can evolve narrow habits. But if droughts, floods, and resource shifts occur, agents need more flexible behavior.

### Agent Design

The human agents should eventually have both physical state and decision-making state.

Physical state can include:

- position
- health
- hunger
- thirst
- energy
- age
- reproductive readiness
- speed
- sensory radius

Decision-making state can include:

- neural network genome
- current neural network phenotype
- memory values, if recurrence is added
- species ID
- generation number
- parent lineage
- mutation history

The neural network should receive sensory inputs and output actions. At first, inputs can be simple engineered values. Later, the system can move toward richer perception, such as giving the agent a small local view of the grid.

### Predator Design

Predators are important because they create active pressure. Without predators, survival may reduce to finding water and food. With predators, agents must balance competing needs. Moving toward water may also move toward danger. Staying still may save energy but increase risk. Running away may avoid death but cause thirst or hunger later.

Predators can begin with simple hand-coded behavior. For example, they can move toward nearby humans or wander randomly when no humans are nearby. Later, predators can also evolve. This would create coevolution: humans evolve to avoid predators, and predators evolve to catch humans.

Coevolution is especially interesting because it prevents the problem from becoming static. If only humans evolve and predators stay simple, humans may eventually find one trick that solves the environment. If predators also evolve, the task keeps changing.

### Food, Water, and Resource Pressure

Food and water should not be treated only as decorations. They should create real tradeoffs.

Water can control thirst. Food can control hunger and energy. Reproduction can require extra energy. Movement can cost energy. Predators can create danger around resource-rich areas. Droughts can reduce water availability. Floods can make movement harder or change where water is safe.

This creates an actual survival problem. An agent must decide what to prioritize:

- drink now or run from predator
- eat now or search for mate
- stay in a safe area or travel to a resource-rich area
- reproduce early or wait until stronger
- follow other agents or explore alone

These tradeoffs are what make the project more than a movement simulation.

### Reproduction and Inheritance

Reproduction should be the bridge between individual survival and population evolution. An agent that survives but never reproduces should not dominate future generations. Fitness should be connected to reproductive success.

There are several possible reproduction models.

The simplest model is **asexual reproduction**. One parent creates one offspring with a mutated copy of its genome. This is easier to implement and debug.

A more advanced model is **sexual reproduction**. Two parents combine genomes through crossover. This fits NEAT well because NEAT already includes crossover logic.

Another option is **generation-based reproduction**. The simulation runs for a fixed time, then the best genomes are selected to create the next generation. This is easier for experiments because every generation has a clear beginning and end.

A more realistic option is **continuous reproduction**. Agents reproduce during the simulation whenever they meet conditions such as age, energy, health, and proximity to a mate. This creates a more natural ecosystem but is harder to analyze.

For the project, a good path is:

1. Start with generation-based reproduction.
2. Add asexual mutation.
3. Add NEAT crossover.
4. Add species tracking.
5. Later move toward continuous reproduction if desired.

### Fitness Design

Fitness is one of the most important parts of the project. A bad fitness function can ruin the simulation by rewarding behavior that looks good numerically but is not actually intelligent.

Possible fitness components:

- lifespan
- amount of food consumed
- amount of water consumed
- number of predator encounters survived
- distance maintained from predators
- successful reproduction
- offspring survival
- energy efficiency
- exploration
- survival during environmental events

But the fitness function should not become too complicated too early. If every behavior is directly rewarded, the agents may optimize the scoring formula instead of survival. The best early fitness function might be simple:

```text
fitness = lifespan + reproduction_bonus + offspring_survival_bonus
```

Then more details can be added only when needed.

A useful principle is that fitness should reward outcomes, not instructions. Instead of rewarding “moving toward water,” reward staying alive when thirst exists. Instead of rewarding “running from predators,” reward surviving predator pressure.

### Data and Statistics

The statistics window is not just a visual feature. It is part of the research side of the project. The simulation should collect data so that progress can be measured.

Useful statistics include:

- humans alive
- humans dead
- predators alive
- predators dead
- births per generation
- deaths by cause
- average lifespan
- average hunger at death
- average thirst at death
- average fitness
- best fitness
- number of species
- average neural network size
- average number of hidden nodes
- average number of connections
- mutation rates
- population diversity
- survival rate during floods
- survival rate during droughts

Later, this data can be exported to CSV files for plotting. That would make the project much stronger because it would allow the simulation to produce actual experimental results rather than only visual observations.

## Development Phases

### Phase 1: Build the Environment

The first phase is to create a stable world. This includes the grid, terrain, clumps, lakes, visual rendering, and basic agents.

Goals:

- generate lakes as connected clumps
- render the grid clearly
- spawn humans and predators
- support movement
- support basic collision rules
- add flood and drought events
- display simulation statistics
- keep the code organized into separate systems

This phase is mostly about infrastructure. It should not focus too much on machine learning yet. The world needs to be reliable before evolution can be meaningful.

### Phase 2: Add Survival Mechanics

The second phase is to make the world matter to the agents.

Goals:

- add hunger
- add thirst
- add energy cost for movement
- add death causes
- add food sources
- make water necessary
- make floods and droughts affect survival
- add predator attacks
- track why agents die

At the end of this phase, random agents should usually die. That is good. If random agents survive easily, then there is not enough pressure for evolution.

### Phase 3: Add Simple Brains

The third phase is to give humans a decision-making system.

Goals:

- define sensory inputs
- define action outputs
- create a simple feedforward neural network
- connect network outputs to movement/actions
- allow random initial weights
- evaluate survival performance

At first, the brain can have a fixed topology. This makes debugging easier. Once the input/output format is stable, NEAT can be added.

### Phase 4: Add Genetic Algorithms

The fourth phase is to evolve the neural network weights.

Goals:

- create a genome representation
- evaluate each genome in the simulation
- assign fitness
- select parents
- mutate weights
- create offspring
- run multiple generations
- save the best genome
- plot fitness over time

This phase proves that evolution can improve behavior even before topology evolution is added.

### Phase 5: Add NEAT

The fifth phase is to implement or integrate NEAT.

Goals:

- encode node genes
- encode connection genes
- track innovation numbers
- implement weight mutation
- implement add-connection mutation
- implement add-node mutation
- implement crossover
- implement compatibility distance
- implement species
- implement fitness sharing
- preserve best genomes
- visualize evolved networks

This is the main algorithmic phase of the project. By the end of it, agents should be able to evolve increasingly complex neural networks.

### Phase 6: Add Reproduction Inside the World

The sixth phase is to make reproduction part of the simulation rather than only an external generation step.

Goals:

- add age and maturity
- require energy for reproduction
- spawn offspring near parents
- pass genomes to offspring
- mutate offspring genomes
- track lineages
- decide whether reproduction is asexual or sexual
- allow population size to grow or collapse naturally

This phase moves the project closer to artificial life.

### Phase 7: Add Coevolution

The seventh phase is to allow predators to evolve too.

Goals:

- give predators neural networks
- create predator fitness functions
- evolve predator hunting behavior
- track predator-prey dynamics
- prevent either side from becoming too dominant
- study arms-race behavior

Coevolution is harder than single-population evolution because the fitness landscape changes over time. A human strategy is good only relative to the current predator strategies, and a predator strategy is good only relative to the current human strategies.

### Phase 8: Add Diversity, Gene Flow, and Genetic Drift

The eighth phase is to make the population structure more biologically interesting.

Goals:

- divide the map into regions
- let groups evolve semi-independently
- allow occasional migration between regions
- track genetic distance between populations
- simulate bottlenecks after disasters
- study drift in small populations
- study gene flow when groups mix

This phase directly connects the project to population genetics.

### Phase 9: Add Richer Environments

The ninth phase is to make the world more complex.

Possible additions:

- forests
- obstacles
- shelters
- diseases
- seasons
- temperature
- resource regrowth
- resource depletion
- different terrain costs
- social behavior
- cooperation
- communication
- memory

The key is not to add everything at once. Each new environmental feature should create a specific kind of pressure and should be measurable.

### Phase 10: Run Experiments and Write Results

The final phase is to treat the simulation like an experiment.

Possible experiments:

- NEAT vs fixed-topology genetic algorithm
- stable environment vs changing environment
- predators vs no predators
- droughts vs no droughts
- speciation on vs speciation off
- simple sensors vs local grid vision
- asexual reproduction vs sexual reproduction
- small population vs large population
- isolated populations vs migration

For each experiment, the project should record data and produce plots.

Important plots:

- best fitness over generations
- average fitness over generations
- population size over time
- number of species over time
- average lifespan over generations
- average network complexity over generations
- deaths by cause
- survival rate during environmental events

This would make the project much more convincing because it would show actual evolutionary trends.

## Relevant Research Articles and Resources

The following papers and resources are especially relevant to the project.

### NEAT and Neuroevolution

1. **Kenneth O. Stanley and Risto Miikkulainen, “Evolving Neural Networks Through Augmenting Topologies” (2002)**  
   This is the original NEAT paper. It introduces evolving both neural network weights and topology, historical markings, speciation, and incremental complexification.  
   Link: https://direct.mit.edu/evco/article/10/2/99/1123/Evolving-Neural-Networks-through-Augmenting  
   PDF: https://nn.cs.utexas.edu/downloads/papers/stanley.ec02.pdf

2. **Kenneth O. Stanley, Jeff Clune, Joel Lehman, and Risto Miikkulainen, “Designing Neural Networks through Neuroevolution” (2019)**  
   This paper gives a modern overview of neuroevolution and explains why evolutionary approaches are still useful for designing and optimizing neural networks.  
   Link: https://www.nature.com/articles/s42256-018-0006-z

3. **Felipe Petroski Such et al., “Deep Neuroevolution: Genetic Algorithms Are a Competitive Alternative for Training Deep Neural Networks for Reinforcement Learning” (2017)**  
   This paper shows that even simple genetic algorithms can train large neural networks for difficult reinforcement learning tasks. It is useful because it connects evolutionary algorithms with modern deep learning.  
   Link: https://arxiv.org/abs/1712.06567  
   OpenReview PDF: https://openreview.net/pdf?id=HyGh4sR9YQ

4. **Edgar Galván and Peter Mooney, “Neuroevolution in Deep Neural Networks: Current Trends and Future Challenges” (2020)**  
   This is a survey of neuroevolution in deep neural networks. It is useful for understanding the broader field and the challenges of scaling neuroevolution.  
   Link: https://arxiv.org/abs/2006.05415

5. **Yanan Liu et al., “A Survey on Evolutionary Neural Architecture Search” (2020)**  
   This paper reviews evolutionary approaches to neural architecture search. It is relevant because NEAT is also about evolving architecture, not just weights.  
   Link: https://arxiv.org/abs/2008.10937

6. **Lishuang Wang et al., “Tensorized NeuroEvolution of Augmenting Topologies for GPU Acceleration” (2024)**  
   This paper is useful for later stages of the project if performance becomes a bottleneck. It focuses on making NEAT faster through tensorization and parallel computation.  
   Link: https://arxiv.org/abs/2404.01817

### Artificial Life, Predator-Prey Systems, and Evolutionary Simulation

7. **Jongwook Park et al., “Co-Evolution of Predator-Prey Ecosystems by Reinforcement Learning Agents” (2021)**  
   This paper is relevant for the predator-prey part of the project. It studies predator-prey ecosystems using modern reinforcement learning.  
   Link: https://pmc.ncbi.nlm.nih.gov/articles/PMC8069842/

8. **J. Li et al., “Predator-prey survival pressure is sufficient to evolve swarming behaviors” (2023)**  
   This paper is useful because it connects survival pressure to emergent group behavior. It supports the idea that complex behavior can arise from simple predator-prey pressure.  
   Link: https://shiyuzhao.westlake.edu.cn/Li_2023_New_J._Phys._25_092001.pdf

9. **Neuroevolution Book / Educational Resource**  
   This is a useful general resource for understanding neuroevolution as a population-based method for training neural networks when correct training targets are not known.  
   Link: https://neuroevolutionbook.com/

### Why These Sources Matter for This Project

The NEAT paper gives the main algorithm. The neuroevolution surveys explain why this approach is still relevant. The deep neuroevolution paper shows that evolutionary algorithms can scale farther than people often assume. Predator-prey and artificial life papers give examples of how survival pressure can create emergent behavior. Together, these sources support the main direction of the project: use an evolving neural system inside a simulated ecosystem to study survival, adaptation, and complexity.

## Experimental Questions

The project should eventually be able to test specific questions rather than only showing interesting behavior.

### Question 1: Does NEAT improve survival over random behavior?

This is the first basic test. Random agents should be compared with evolved agents. If evolved agents do not survive longer, then either the fitness function, sensory inputs, action space, or environment design needs to be fixed.

### Question 2: Does topology evolution help more than weight-only evolution?

This tests whether NEAT is actually useful compared with a simpler genetic algorithm. One group of agents would evolve only weights with a fixed network. Another group would use NEAT to evolve both weights and structure. If NEAT performs better or produces more flexible behavior, that supports the choice of algorithm.

### Question 3: Does environmental change favor more general intelligence?

Agents can be trained in a stable environment and in an environment with floods, droughts, and shifting resources. The hypothesis is that changing environments should favor more general behavior, while stable environments may allow narrow strategies.

### Question 4: Does predator pressure produce better navigation and avoidance?

The simulation can compare worlds with no predators, simple predators, and evolving predators. If predator pressure matters, humans should evolve stronger avoidance behavior and possibly different resource strategies.

### Question 5: Does speciation preserve useful diversity?

NEAT’s speciation can be turned on and off. The project can measure whether speciation leads to better long-term fitness, more diverse strategies, or better recovery after environmental changes.

### Question 6: Does gene flow help isolated populations adapt?

The map can be divided into regions. Each region can evolve mostly independently. Occasionally, individuals can migrate. The project can measure whether migration improves adaptation or reduces diversity.

### Question 7: Can complex behavior emerge without directly rewarding it?

This is one of the most important questions. The project should avoid directly rewarding every desired behavior. Instead, it should reward survival and reproduction. If agents still develop behaviors such as avoiding predators, moving toward water, staying near safe zones, or timing reproduction, then the simulation is producing emergent behavior rather than scripted behavior.

## Design Principles

### Keep the First Version Simple

The project should not begin with every feature at once. A small working evolutionary loop is better than a large unfinished ecosystem. The first useful version only needs agents, resources, death, reproduction, mutation, and fitness tracking.

### Make Every Feature Create Pressure

A feature should not be added only because it looks interesting. It should create a survival tradeoff. For example, water matters if thirst exists. Predators matter if they can kill agents. Floods matter if they change movement or survival. Food matters if hunger exists. Reproduction matters if it costs energy.

### Track Data from the Beginning

The project should collect statistics early. Without data, it will be hard to tell whether agents are actually improving. Visual behavior can be misleading. A population may look smarter while average fitness is not improving.

### Avoid Over-Rewarding Behavior

The fitness function should not become a list of commands. The goal is not to force the agent to follow human-designed behavior. The goal is to let useful behavior emerge from survival pressure.

### Make the Code Modular

The project should separate systems clearly:

- rendering
- world generation
- agent logic
- predator logic
- resources
- environmental events
- neural networks
- genomes
- evolution
- statistics
- configuration

This will make the project easier to expand.

### Save and Reload Experiments

Eventually, the simulation should save:

- best genome
- generation statistics
- config settings
- random seed
- population state
- species history

This makes experiments reproducible.

## Possible Project Structure

A clean structure might look like this:

```text
src/
  core/
    config.h
    grid.h
    simulation.h
  world/
    clump.h
    lake.h
    terrain.h
    resource.h
    environment_event.h
  agents/
    human.h
    predator.h
    organism.h
  evolution/
    genome.h
    gene.h
    species.h
    population.h
    neat.h
    fitness.h
  neural/
    neural_network.h
    activation.h
  ui/
    renderer.h
    statistics_panel.h
    person_viewer.h
  data/
    logger.h
    csv_writer.h
assets/
  icons/
  textures/
experiments/
  configs/
  results/
```

The exact structure can change, but the important idea is that the simulation should not put rendering, evolution, and agent behavior all in one file. NEAT will become complicated, so the code should be organized before that complexity is added.

## Expected Challenges

### Fitness Hacking

Agents may discover behavior that increases the fitness score without actually looking intelligent. For example, if fitness rewards movement distance, agents may spin around pointlessly. If fitness rewards staying near water, agents may never explore or reproduce.

The solution is to reward survival outcomes and test behavior visually.

### Computational Cost

Evolution requires many evaluations. If each genome needs a full simulation run, training can become slow. This can be improved by reducing early simulation complexity, running shorter evaluations, parallelizing later, or saving only necessary data.

### Balancing the Environment

If the environment is too easy, all agents survive and selection is weak. If it is too hard, all agents die and evolution has no signal. The environment needs to be tuned so that some variation in behavior matters.

### Noisy Fitness

The same genome may perform differently depending on spawn location, predator movement, or random events. This can make selection unstable. One solution is to evaluate each genome multiple times and average the fitness, but that increases computation.

### Premature Convergence

The population may become too similar too quickly. Speciation, mutation, novelty search, and environmental variation can help maintain diversity.

### Debugging Learned Behavior

When an evolved agent does something strange, it may be hard to know why. The project should include tools for inspecting an agent’s inputs, outputs, genome, and network structure.

## Long-Term Outlook

The long-term direction of the project is to turn the simulation into a small research platform for studying artificial evolution. The first goal is basic survival. The next goal is adaptation across generations. After that, the project can explore coevolution, environmental change, population structure, and emergent behavior.

A strong final version would have:

- a visual ecosystem
- humans with evolved neural networks
- predators with either programmed or evolved behavior
- food and water pressure
- reproduction and inheritance
- mutation and crossover
- NEAT-based topology evolution
- species tracking
- environmental events
- data export
- experiment configs
- plots showing evolutionary progress
- saved best genomes
- network visualizations

The project is valuable because it connects several ideas that are often learned separately: evolution, neural networks, reinforcement learning, ecology, and simulation. It also makes the abstract idea of natural selection visible. Instead of only saying that better-adapted agents survive, the simulation can show which agents survive, why they survive, how their brains change, and how the population responds when the environment changes.

The most important thing is to build the project in layers. A simple world with meaningful pressure is better than a complicated world where nothing can be measured. Once the basic evolutionary loop works, the simulation can grow naturally: more resources, more agents, better brains, harder environments, and more detailed experiments.

In the end, the project should answer a simple but deep question: how much intelligent-looking behavior can emerge when agents are only asked to survive, reproduce, and adapt?
