# Evolution Simulation

The purpose of this project is to simulate a population of creatures evolving in an environment with limited resources, predators, disease, weather, and other survival pressures.

The long-term goal is for creatures to learn how to survive, reproduce, and eventually develop more complex behaviors through evolution. These behaviors may include finding food and water, avoiding predators, hiding in forests or water, caring for young, using resources, and modifying the environment around them.

# Project Direction

This project will be developed in phases. The goal is to start with a simple survival environment and slowly add more complexity over time. This should make it easier for the creatures to learn basic behaviors first before being placed into a much harder world.

## Phase 1: Plain NEAT

Start with a basic NEAT system where agents evolve neural networks through survival, reproduction, and mutation.

At this stage, the world should stay simple. It should include food, water, movement, hunger, thirst, health, and death. The goal is not complex intelligence yet. The goal is to see whether agents can learn basic survival behaviors, such as moving toward food, drinking water, and staying alive.

## Phase 2: NEAT with Better Fitness Shaping

Once basic NEAT works, improve the fitness function so agents are rewarded for useful behaviors, not just staying alive.

For example, agents can be rewarded for eating, drinking, reproducing, avoiding predators, sleeping safely, and keeping children alive. This should guide evolution toward more meaningful survival strategies instead of boring behaviors like standing still or camping near water forever.

## Phase 3: NEAT with Novelty Search

Novelty search rewards agents for discovering new behaviors, not only for having high fitness.

This is useful because complex behaviors often require exploration before they become useful. For example, picking up food, hiding in forests, entering water, or placing objects might not immediately improve survival, but these actions could become important later. Novelty search helps preserve unusual behaviors instead of removing them too early.

## Phase 4: MAP-Elites for Different Survival Strategies

MAP-Elites is useful when there is not one single best way to survive.

Instead of keeping only the strongest agents, it preserves many different types of successful agents. Some agents may become aggressive fighters, some may hide in forests, some may stay near water, some may reproduce quickly, and others may become better at carrying resources.

This is helpful for creating an ecosystem with multiple survival strategies instead of having one dominant behavior take over the entire population.

## Phase 5: POET-Style Curriculum

In this phase, the environment becomes harder over time along with the agents.

Instead of placing agents into the full complex world immediately, the world slowly adds new challenges, such as predators, night cycles, weather, disease, inventory, farming, and resource placement. This gives evolution stepping stones. Agents first learn simple survival, then gradually adapt to more complicated environments where long-term planning becomes useful.

# To Do

## To Add

### Death and Bodies

* When a person dies, their body should remain on the map.
* Dead bodies should be displayed as grey blocks.
* Bodies may later contribute to disease if many are close together.

### Food and Water

* Add a way for people to eat food when they are within a one-block radius.
* Add a way for people to drink water when they are within a one-block radius.
* Add food sources around the map.
* Add forests, possibly with red dots representing apples or other food.

### Fighting

* Add a way for people to fight when they are within a one-block radius of another person or predator.

### Inventory

* Add a way for people to pick up items and store them in an inventory.
* Each person should have an inventory limit of 5 blocks/items.
* Inventory should be visible in the player inspector view.
* Eventually, people may be able to pick up water and place it somewhere else to generate their own food.
* People may also be able to pick up food and feed it to their young.

### Day and Night Cycle

* Add a day and night cycle.
* Change the ground color depending on the time of day.
* Add tiredness as a factor that affects health.
* Tiredness should decrease when a person is stationary, which will act as sleeping.

### Weather

* Add weather events.
* Rain should make lakes grow larger and may destroy some plants.
* Very hot weather should make lakes shrink and may cause plants to die or disappear.

### Disease

* Add disease zones when many dead bodies are near each other.
* Disease should affect people within a certain radius of clustered bodies.

### Predators

* Add land predators and water predators.
* Predators should chase and attack people.
* Predators should die after 500 ticks without eating someone.
* Some predators should have a sleep setting so they only come out at night.

### Neural Network Inputs

* Add a way for each person to sense statistics about nearby people, resources, predators, and terrain.
* These nearby statistics should be used as inputs to the neural network.

### Hiding and Terrain Strategy

* Allow people to hide in forests to avoid being seen by predators.
* Allow people to hide in water to avoid land predators.
* People should not be able to stay in water for too long, because water predators may attack them.

# Developer

This project is created by Dhruv Upreti.
