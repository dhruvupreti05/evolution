#include "human.h"
#include "core/debuglog.h"
#include "core/gridutils.h"
#include "core/config.h"
#include "environment/lake.h"
#include "entities/crop.h"
#include "entities/predator.h"
#include "brain/manual-brain.h"
#include "brain/random-brain.h"
#include "entities/action.h"
#include "entities/entityoccupancy.h"

#include <cstdlib>
#include <set>
#include <utility>
#include <cmath>
#include <algorithm>

std::vector<Human> Human::humans;
int Human::nextId = 0;
int Human::inspectedHumanId = -1;

/*
    Checks whether an action is aimed at a specific human's current tile.
    Used when confirming that two humans chose to mate with each other.
*/
static bool targetsEntity(const Action& action, const Human& human)
{
    return action.targetX == human.getX() && action.targetY == human.getY();
}

/*
    Creates a human with randomized starting stats, a random gender, and a unique ID.
*/
Human::Human(int gridX, int gridY) : Entity(gridX, gridY, Config::HUMAN_START_HEALTH + (rand() % (2 * Config::HUMAN_START_HEALTH_BUFFER + 1) - Config::HUMAN_START_HEALTH_BUFFER), Config::HUMAN_START_THIRST + (rand() % (2 * Config::HUMAN_START_THIRST_BUFFER + 1) - Config::HUMAN_START_THIRST_BUFFER), Config::HUMAN_START_HUNGER + (rand() % (2 * Config::HUMAN_START_HUNGER_BUFFER + 1) - Config::HUMAN_START_HUNGER_BUFFER)), Food(Config::TICKS_PER_MEAL_HUMAN, Config::HUNGER_PER_TICK_MEAL_HUMAN), age(0), gender((rand() % 2 == 0) ? Gender::Male : Gender::Female), id(nextId++)
{
    setRandomBrain();
}

/*
    Spawns the starting humans on non-water tiles.
*/
void Human::init(GameWorld& world)
{
    for (int i = 0; i < Config::NUM_PLAYERS; ++i)
    {
        int gridX;
        int gridY;

        do
        {
            gridX = rand() % world.getGridWidth();
            gridY = rand() % world.getGridHeight();
        }
        while (world.getTile(gridX, gridY) == TileType::Water || Human::isHumanAt(gridX, gridY));

        humans.emplace_back(gridX, gridY);
    }
}

/*
    Updates one human.
    Dead humans only keep their body timer running.
*/
void Human::update(GameWorld& world)
{
    if (dead)
    {
        if (deadBodyTicksRemaining > 0)
        {
            deadBodyTicksRemaining--;
        }

        return;
    }

    Entity::update(world);
}

/*
    Returns whether this dead human still has an edible body.
*/
bool Human::hasBody() const
{
    return isEdible();
}

/*
    Returns whether this human's body can currently be eaten.
*/
bool Human::isBodyEdible() const
{
    return isEdible();
}

/*
    Body claiming is currently disabled, so bodies are never marked as claimed.
*/
bool Human::isBodyClaimedThisTick() const
{
    return false;
}

/*
    Placeholder for body-claiming logic.
*/
void Human::claimBodyForEating()
{
}

/*
    Placeholder for resetting body-claiming logic each tick.
*/
void Human::resetBodyEatingClaims()
{
}

/*
    Removes one eating tick from this human's dead body.
*/
void Human::eatBodyOneTick()
{
    eatOneTick();
}

/*
    Updates hunger, thirst, age, and the health effects caused by them.
    Hunger and thirst no longer kill directly; they only push health up or down.
*/
void Human::decayStats()
{
    thirst -= Config::HUMAN_THIRST_DECAY;
    hunger -= Config::HUMAN_HUNGER_DECAY;
    age += Config::HUMAN_AGE_INCREASE;

    thirst = std::max(0, thirst);
    hunger = std::max(0, hunger);

    updateHealthFromNeedsAndAge();
}

/*
    Applies the combined health effect of hunger, thirst, and age.
    Integer divisors keep the curve smooth enough without adding fractional state.
*/
void Human::updateHealthFromNeedsAndAge()
{
    if (Config::HUMAN_HEALTH_UPDATE_INTERVAL <= 0)
    {
        return;
    }

    if (age % Config::HUMAN_HEALTH_UPDATE_INTERVAL != 0)
    {
        return;
    }

    int hungerDamage = std::max(0, Config::HUMAN_SAFE_HUNGER - hunger) / Config::HUMAN_HUNGER_DAMAGE_DIVISOR;
    int thirstDamage = std::max(0, Config::HUMAN_SAFE_THIRST - thirst) / Config::HUMAN_THIRST_DAMAGE_DIVISOR;
    int ageDamage = (age + Config::HUMAN_AGE_DAMAGE_DIVISOR - 1) / Config::HUMAN_AGE_DAMAGE_DIVISOR;

    int healing = 0;

    if (hunger >= Config::HUMAN_HEALING_HUNGER && thirst >= Config::HUMAN_HEALING_THIRST)
    {
        healing = Config::HUMAN_HEALTH_RECOVERY;
    }

    health += healing - hungerDamage - thirstDamage - ageDamage;
    health = std::max(0, std::min(Config::HUMAN_MAX_HEALTH, health));
}

/*
    Marks the human as dead only when health reaches zero.
    Hunger and thirst can cause health loss, but they are not death conditions anymore.
*/
void Human::checkDeath()
{
    if (dead)
    {
        return;
    }

    if (health <= 0)
    {
        dead = true;
        deadBodyTicksRemaining = Config::TICKS_PER_DEAD_BODY;
        mealTicksRemaining = Config::TICKS_PER_MEAL_HUMAN;
    }
}

/*
    Saves one movement attempt into the circular movement history.
*/
void Human::recordMoveAttempt(MoveAttempt attempt)
{
    previousMoves[moveMemoryIndex] = attempt;

    moveMemoryIndex = (moveMemoryIndex + 1) % MOVE_MEMORY_SIZE;

    if (moveMemoryCount < MOVE_MEMORY_SIZE)
    {
        moveMemoryCount++;
    }
}

/*
    Returns the most recent move attempt.
*/
MoveAttempt Human::getPreviousMove() const
{
    int previousIndex = moveMemoryIndex - 1;

    if (previousIndex < 0)
    {
        previousIndex = MOVE_MEMORY_SIZE - 1;
    }

    return previousMoves[previousIndex];
}

/*
    Picks a random direction, including staying still.
*/
Direction Human::getRandomDirection() const
{
    int choice = rand() % 5;

    switch (choice)
    {
        case 0:
            return Direction::Stay;

        case 1:
            return Direction::Up;

        case 2:
            return Direction::Down;

        case 3:
            return Direction::Left;

        case 4:
            return Direction::Right;

        default:
            return Direction::Stay;
    }
}

/*
    Converts a direction into x/y movement.
*/
void Human::directionToDelta(Direction direction, int& dx, int& dy) const
{
    dx = 0;
    dy = 0;

    switch (direction)
    {
        case Direction::Up:
            dy = -1;
            break;

        case Direction::Down:
            dy = 1;
            break;

        case Direction::Left:
            dx = -1;
            break;

        case Direction::Right:
            dx = 1;
            break;

        case Direction::Stay:
        default:
            break;
    }
}

/*
    Updates the direction the human is facing.
    Staying still does not change orientation.
*/
void Human::updateOrientation(Direction direction)
{
    switch (direction)
    {
        case Direction::Up:
            orientation = Orientation::North;
            break;

        case Direction::Down:
            orientation = Orientation::South;
            break;

        case Direction::Left:
            orientation = Orientation::West;
            break;

        case Direction::Right:
            orientation = Orientation::East;
            break;

        case Direction::Stay:
        default:
            break;
    }
}

/*
    Tries to move the human one tile.
    Failed moves are still recorded so movement history stays accurate.
*/
void Human::move(Direction direction, GameWorld& world)
{
    if (dead)
    {
        return;
    }

    if (direction != Direction::Stay)
    {
        updateOrientation(direction);
    }

    int dx;
    int dy;

    directionToDelta(direction, dx, dy);

    int newX = x + dx;
    int newY = y + dy;

    MoveAttempt attempt;
    attempt.direction = direction;
    attempt.tile = TileType::Empty;
    attempt.succeeded = false;

    if (!world.isInsideGrid(newX, newY))
    {
        recordMoveAttempt(attempt);
        return;
    }

    if (direction != Direction::Stay && (Human::isBlockingEntityAt(newX, newY) || Predator::isPredatorAt(newX, newY)))
    {
        recordMoveAttempt(attempt);
        return;
    }

    TileType tile = world.getTile(newX, newY);
    attempt.tile = tile;

    if (direction == Direction::Stay)
    {
        attempt.succeeded = true;
        recordMoveAttempt(attempt);
        return;
    }

    if (tile == TileType::Water)
    {
        bool shouldMoveThroughWater = false;

        if (moveMemoryCount > 0)
        {
            MoveAttempt previousMove = getPreviousMove();

            // A human only enters water after trying the same water move twice in a row.
            shouldMoveThroughWater = previousMove.direction == direction && previousMove.tile == TileType::Water && previousMove.succeeded == false;
        }

        if (shouldMoveThroughWater)
        {
            int oldX = x;
            int oldY = y;

            x = newX;
            y = newY;

            EntityOccupancy::updateHumanPosition(*this, oldX, oldY);

            attempt.succeeded = true;
        }

        recordMoveAttempt(attempt);
        return;
    }

    int oldX = x;
    int oldY = y;

    x = newX;
    y = newY;

    EntityOccupancy::updateHumanPosition(*this, oldX, oldY);

    attempt.succeeded = true;
    recordMoveAttempt(attempt);
}

/*
    Moves the human in a random direction.
*/
void Human::moveRandomly(GameWorld& world)
{
    Direction direction = getRandomDirection();

    move(direction, world);
}

/*
    Converts the human's orientation into forward and right direction vectors.
    Vision uses these to turn relative coordinates into world coordinates.
*/
void Human::orientationToBasis(int& forwardDx, int& forwardDy, int& rightDx, int& rightDy) const
{
    forwardDx = 0;
    forwardDy = 0;
    rightDx = 0;
    rightDy = 0;

    switch (orientation)
    {
        case Orientation::North:
            forwardDx = 0;
            forwardDy = -1;
            rightDx = 1;
            rightDy = 0;
            break;

        case Orientation::South:
            forwardDx = 0;
            forwardDy = 1;
            rightDx = -1;
            rightDy = 0;
            break;

        case Orientation::East:
            forwardDx = 1;
            forwardDy = 0;
            rightDx = 0;
            rightDy = 1;
            break;

        case Orientation::West:
            forwardDx = -1;
            forwardDy = 0;
            rightDx = 0;
            rightDy = -1;
            break;
    }
}

/*
    Checks whether a world-relative offset is inside the current human vision shape.
    The forward triangle is rotated by orientation; centered shapes ignore orientation.
*/
bool Human::isRelativeVisionCell(int dx, int dy) const
{
    if (dx == 0 && dy == 0)
    {
        return false;
    }

    int range = Config::HUMAN_VISION_RANGE;

    switch (Config::HUMAN_VISION_SHAPE)
    {
        case VisionShape::Circle:
        {
            int scaledDx = dx * 2;
            int scaledDy = dy * 2;
            int scaledRadius = range * 2 + 1;

            return scaledDx * scaledDx + scaledDy * scaledDy <= scaledRadius * scaledRadius;
        }

        case VisionShape::Square:
            return std::abs(dx) <= range && std::abs(dy) <= range;

        case VisionShape::Diamond:
            return std::abs(dx) + std::abs(dy) <= range;

        case VisionShape::ForwardTriangle:
        default:
        {
            int forwardDx;
            int forwardDy;
            int rightDx;
            int rightDy;

            orientationToBasis(forwardDx, forwardDy, rightDx, rightDy);

            int forward = dx * forwardDx + dy * forwardDy;
            int side = dx * rightDx + dy * rightDy;

            if (forward < 1 || forward > range)
            {
                return false;
            }

            return std::abs(side) <= forward - 1;
        }
    }
}

/*
    Adds a terrain tile to the visible list when the offset is visible and inside the world.
    Empty and sand tiles are skipped so the vision list stays focused on useful information.
*/
void Human::addVisibleTileIfSeen(const GameWorld& world, std::vector<VisibleTile>& visibleTiles, int dx, int dy) const
{
    if (!isRelativeVisionCell(dx, dy))
    {
        return;
    }

    int worldX = x + dx;
    int worldY = y + dy;

    if (!world.isInsideGrid(worldX, worldY))
    {
        return;
    }

    TileType tile = world.getTile(worldX, worldY);

    if (!shouldLogVisibleTile(tile))
    {
        return;
    }

    visibleTiles.push_back({tile, dx, dy, worldX, worldY});
}

/*
    Filters out boring visible tiles from the inspector list.
*/
bool Human::shouldLogVisibleTile(TileType tile) const
{
    return tile != TileType::Empty && tile != TileType::Sand;
}

/*
    Builds the list of visible terrain, humans, and predators for the configured human vision shape.
    Only the forward triangle uses orientation; circle, square, and diamond are centered on the human.
*/
std::vector<VisibleTile> Human::getVisibleTiles(const GameWorld& world) const
{
    std::vector<VisibleTile> visibleTiles;

    if (dead)
    {
        return visibleTiles;
    }

    int range = Config::HUMAN_VISION_RANGE;

    for (int dy = -range; dy <= range; ++dy)
    {
        for (int dx = -range; dx <= range; ++dx)
        {
            addVisibleTileIfSeen(world, visibleTiles, dx, dy);
        }
    }

    for (const auto& otherHuman : humans)
    {
        if (otherHuman.isDead())
        {
            continue;
        }

        if (otherHuman.getId() == id)
        {
            continue;
        }

        int dx = otherHuman.getX() - x;
        int dy = otherHuman.getY() - y;

        if (isRelativeVisionCell(dx, dy))
        {
            visibleTiles.push_back({TileType::Human, dx, dy, otherHuman.getX(), otherHuman.getY()});
        }
    }

    for (const auto& predator : Predator::getPredators())
    {
        if (predator.isDead())
        {
            continue;
        }

        int dx = predator.getX() - x;
        int dy = predator.getY() - y;

        if (isRelativeVisionCell(dx, dy))
        {
            visibleTiles.push_back({TileType::Predator, dx, dy, predator.getX(), predator.getY()});
        }
    }

    return visibleTiles;
}

/*
    Draws one living human.
    The inspected human is drawn with its vision outline and highlight color.
*/
void Human::draw(GameWorld& world) const
{
    if (dead)
    {
        return;
    }

    if (id == inspectedHumanId)
    {
        drawVisionOutline(world);
        world.drawTile(x, y, Config::COLOR_SELECTED_HUMAN);
    }
    else
    {
        world.drawTile(x, y, TileType::Human);
    }
}

/*
    Draws all edible dead bodies.
*/
void Human::drawBodies(GameWorld& world)
{
    for (const auto& human : humans)
    {
        if (human.hasBody())
        {
            world.drawTile(human.getX(), human.getY(), Config::COLOR_DEAD_BODY);
        }
    }
}

/*
    Updates every human in phases.
    Mating is resolved between choosing actions and executing normal actions.
*/
void Human::updateHumans(GameWorld& world)
{
    for (auto& human : humans)
    {
        if (human.dead)
        {
            if (human.deadBodyTicksRemaining > 0)
            {
                human.deadBodyTicksRemaining--;
            }

            human.clearPreparedAction();
            continue;
        }

        human.tickMatingCooldown();

        human.decayStats();
        human.checkDeath();

        if (human.dead)
        {
            human.clearPreparedAction();
            continue;
        }

        human.prepareAction(world);
    }

    resolveMatingActions(world);

    for (auto& human : humans)
    {
        if (human.dead)
        {
            continue;
        }

        if (human.hasPreparedAction() && human.getPreparedAction().type != ActionType::Mate)
        {
            human.executePreparedAction(world);
        }
        else
        {
            human.clearPreparedAction();
        }
    }
}

/*
    Moves every human randomly.
    Mainly useful for simple testing behavior.
*/
void Human::moveHumans(GameWorld& world)
{
    for (auto& human : humans)
    {
        human.moveRandomly(world);
    }
}

/*
    Draws all humans, with the inspected human drawn last so it appears on top.
*/
void Human::drawHumans(GameWorld& world)
{
    int selectedId = Human::getInspectedHumanId();

    for (const auto& human : humans)
    {
        if (human.getId() != selectedId)
        {
            human.draw(world);
        }
    }

    for (const auto& human : humans)
    {
        if (human.getId() == selectedId)
        {
            human.draw(world);
        }
    }
}

/*
    Returns the shared human list.
*/
std::vector<Human>& Human::getHumans()
{
    return humans;
}

/*
    Sets which human the inspector is currently viewing.
*/
void Human::setInspectedHumanId(int humanId)
{
    inspectedHumanId = humanId;
}

/*
    Clears the inspected human selection.
*/
void Human::clearInspectedHuman()
{
    inspectedHumanId = -1;
}

/*
    Returns the human's grid x position.
*/
int Human::getX() const
{
    return x;
}

/*
    Returns the human's grid y position.
*/
int Human::getY() const
{
    return y;
}

/*
    Returns the human's current health.
*/
int Human::getHealth() const
{
    return health;
}

/*
    Returns the human's current thirst.
*/
int Human::getThirst() const
{
    return thirst;
}

/*
    Returns the human's current hunger.
*/
int Human::getHunger() const
{
    return hunger;
}

/*
    Returns the human's age counter.
*/
int Human::getAge() const
{
    return age;
}

/*
    Returns the human's unique ID.
*/
int Human::getId() const
{
    return id;
}

/*
    Returns the human's gender.
*/
Gender Human::getGender() const
{
    return gender;
}

/*
    Returns the direction this human is facing.
*/
Orientation Human::getOrientation() const
{
    return orientation;
}

/*
    Returns gender as text for the UI.
*/
std::string Human::getGenderString() const
{
    switch (gender)
    {
        case Gender::Male:
            return "Male";

        case Gender::Female:
            return "Female";

        default:
            return "Unknown";
    }
}

/*
    Returns orientation as text for the UI.
*/
std::string Human::getOrientationString() const
{
    switch (orientation)
    {
        case Orientation::North:
            return "North";

        case Orientation::South:
            return "South";

        case Orientation::East:
            return "East";

        case Orientation::West:
            return "West";

        default:
            return "Unknown";
    }
}

/*
    Returns whether this human is dead.
*/
bool Human::isDead() const
{
    return dead;
}

/*
    Returns the ID of the human selected in the inspector.
*/
int Human::getInspectedHumanId()
{
    return inspectedHumanId;
}

/*
    Draws an outline around the inspected human's visible area.
    Only outer edges are drawn so the selected vision shape looks like one connected region.
*/
void Human::drawVisionOutline(GameWorld& world) const
{
    if (dead)
    {
        return;
    }

    std::set<std::pair<int, int>> visionCells;

    int range = Config::HUMAN_VISION_RANGE;

    for (int dy = -range; dy <= range; ++dy)
    {
        for (int dx = -range; dx <= range; ++dx)
        {
            if (!isRelativeVisionCell(dx, dy))
            {
                continue;
            }

            int worldX = x + dx;
            int worldY = y + dy;

            if (!world.isInsideGrid(worldX, worldY))
            {
                continue;
            }

            visionCells.insert({worldX, worldY});
        }
    }

    float cellSize = static_cast<float>(world.getCellSize());

    for (const auto& cell : visionCells)
    {
        int gx = cell.first;
        int gy = cell.second;

        bool hasUp = visionCells.count({gx, gy - 1}) > 0;
        bool hasDown = visionCells.count({gx, gy + 1}) > 0;
        bool hasLeft = visionCells.count({gx - 1, gy}) > 0;
        bool hasRight = visionCells.count({gx + 1, gy}) > 0;

        float px = gx * cellSize;
        float py = gy * cellSize;

        if (!hasUp)
        {
            world.drawLine(px, py, px + cellSize, py, sf::Color::Black, 2.0f);
        }

        if (!hasDown)
        {
            world.drawLine(px, py + cellSize, px + cellSize, py + cellSize, sf::Color::Black, 2.0f);
        }

        if (!hasLeft)
        {
            world.drawLine(px, py, px, py + cellSize, sf::Color::Black, 2.0f);
        }

        if (!hasRight)
        {
            world.drawLine(px + cellSize, py, px + cellSize, py + cellSize, sf::Color::Black, 2.0f);
        }
    }
}

/*
    Moves the human directly from manual or inspector controls.
*/
void Human::controlledMove(Direction direction, GameWorld& world)
{
    move(direction, world);
}

/*
    Finds the tile directly in front of the human.
*/
void Human::getFacingCell(int& targetX, int& targetY) const
{
    int forwardDx;
    int forwardDy;
    int rightDx;
    int rightDy;

    orientationToBasis(forwardDx, forwardDy, rightDx, rightDy);

    targetX = x + forwardDx;
    targetY = y + forwardDy;
}

/*
    Returns whether a tile type can be stored in the human inventory.
*/
bool Human::canPickUp(TileType tile) const
{
    return tile == TileType::Water || tile == TileType::Crop;
}

/*
    Returns whether the human can drop an item onto a target tile.
*/
bool Human::canDropOn(TileType tile, int targetX, int targetY) const
{
    if (tile != TileType::Empty && tile != TileType::Sand)
    {
        return false;
    }

    if (Human::isHumanAt(targetX, targetY))
    {
        return false;
    }

    return true;
}

/*
    Returns whether this human can currently be eaten as a body.
*/
bool Human::isEdible() const
{
    return dead && deadBodyTicksRemaining > 0 && Food::isEdible();
}

/*
    Picks up the crop or water directly in front of the human.
*/
bool Human::tryPickUp(GameWorld& world)
{
    if (dead)
    {
        return false;
    }

    if (static_cast<int>(inventory.size()) >= Config::HUMAN_INVENTORY_SIZE)
    {
        return false;
    }

    int targetX;
    int targetY;
    getFacingCell(targetX, targetY);

    if (!world.isInsideGrid(targetX, targetY))
    {
        return false;
    }

    TileType tile = world.getTile(targetX, targetY);

    if (!canPickUp(tile))
    {
        return false;
    }

    if (tile == TileType::Crop)
    {
        Crop::removeCropAt(world, targetX, targetY);
        world.setTile(targetX, targetY, TileType::Empty);
        inventory.push_back(TileType::Crop);
        return true;
    }

    if (tile == TileType::Water)
    {
        bool removed = Lake::removeWaterAt(world, targetX, targetY);

        if (!removed)
        {
            return false;
        }

        inventory.push_back(TileType::Water);
        return true;
    }

    return false;
}

/*
    Drops the most recently picked-up item onto the tile in front of the human.
*/
bool Human::tryDrop(GameWorld& world)
{
    if (dead)
    {
        return false;
    }

    if (inventory.empty())
    {
        return false;
    }

    int targetX;
    int targetY;
    getFacingCell(targetX, targetY);

    if (!world.isInsideGrid(targetX, targetY))
    {
        return false;
    }

    TileType targetTile = world.getTile(targetX, targetY);

    if (!canDropOn(targetTile, targetX, targetY))
    {
        return false;
    }

    TileType item = inventory.back();

    if (item == TileType::Crop)
    {
        world.setTile(targetX, targetY, TileType::Crop);
        Crop::addCropAt(targetX, targetY);
        inventory.pop_back();
        return true;
    }

    if (item == TileType::Water)
    {
        bool placed = Lake::placeWaterAt(world, targetX, targetY);

        if (!placed)
        {
            return false;
        }

        inventory.pop_back();
        return true;
    }

    return false;
}

/*
    Returns the human's inventory.
*/
const std::vector<TileType>& Human::getInventory() const
{
    return inventory;
}

/*
    Checks whether a living human is standing on a tile.
    Uses occupancy lookup when available, otherwise falls back to scanning.
*/
bool Human::isHumanAt(int x, int y)
{
    if (EntityOccupancy::hasBeenBuilt())
    {
        return EntityOccupancy::hasHumanAt(x, y);
    }

    for (const Human& human : humans)
    {
        if (human.dead)
        {
            continue;
        }

        if (human.x == x && human.y == y)
        {
            return true;
        }
    }

    return false;
}

/*
    Checks whether a tile is blocked by a living human or an edible dead body.
*/
bool Human::isBlockingEntityAt(int x, int y)
{
    if (EntityOccupancy::hasBeenBuilt())
    {
        if (EntityOccupancy::hasHumanAt(x, y))
        {
            return true;
        }
    }
    else
    {
        for (const Human& human : humans)
        {
            if (!human.dead && human.x == x && human.y == y)
            {
                return true;
            }
        }
    }

    for (const Human& human : humans)
    {
        if (human.dead && human.hasBody() && human.x == x && human.y == y)
        {
            return true;
        }
    }

    return false;
}

/*
    Finds a living human directly next to a position.
*/
Human* Human::getAdjacentLivingHuman(int x, int y)
{
    for (const auto& direction : GridUtils::FOUR_DIRECTIONS)
    {
        int targetX = x + direction.dx;
        int targetY = y + direction.dy;

        Human* human = EntityOccupancy::getHumanAt(targetX, targetY);

        if (human != nullptr && !human->isDead())
        {
            return human;
        }
    }

    if (EntityOccupancy::hasBeenBuilt())
    {
        return nullptr;
    }

    for (Human& human : humans)
    {
        if (human.dead)
        {
            continue;
        }

        if (GridUtils::isFourNeighborDistance(x, y, human.x, human.y))
        {
            return &human;
        }
    }

    return nullptr;
}

/*
    Finds an edible dead body directly next to a position.
*/
Human* Human::getAdjacentEdibleBody(int x, int y)
{
    for (auto& human : humans)
    {
        if (!human.isBodyEdible())
        {
            continue;
        }

        if (GridUtils::isFourNeighborDistance(x, y, human.getX(), human.getY()))
        {
            return &human;
        }
    }

    return nullptr;
}

/*
    Finds the nearest living human or edible body with no range limit.
*/
Human* Human::getNearestLivingHumanOrBody(int x, int y)
{
    Human* nearest = nullptr;
    int bestDistance = 0;

    for (auto& human : humans)
    {
        bool validTarget = !human.isDead() || human.isBodyEdible();

        if (!validTarget)
        {
            continue;
        }

        int dx = std::abs(human.getX() - x);
        int dy = std::abs(human.getY() - y);
        int distance = dx + dy;

        if (nearest == nullptr || distance < bestDistance)
        {
            nearest = &human;
            bestDistance = distance;
        }
    }

    return nearest;
}

/*
    Counts living humans.
*/
int Human::countAlive()
{
    int count = 0;

    for (const auto& human : humans)
    {
        if (!human.isDead())
        {
            count++;
        }
    }

    return count;
}

/*
    Counts dead humans.
*/
int Human::countDead()
{
    int count = 0;

    for (const auto& human : humans)
    {
        if (human.isDead())
        {
            count++;
        }
    }

    return count;
}

/*
    Attempts to move and returns success.
    This currently returns true even if the move was blocked.
*/
bool Human::tryMove(Direction direction, GameWorld& world)
{
    move(direction, world);
    return true;
}

/*
    Tries to eat a crop from an adjacent tile.
*/
bool Human::tryEatAt(GameWorld& world, int targetX, int targetY)
{
    if (!GridUtils::isFourNeighborDistance(x, y, targetX, targetY))
    {
        return false;
    }

    if (world.getTile(targetX, targetY) != TileType::Crop)
    {
        return false;
    }

    int hungerGain = 0;

    if (!Crop::eatCropAt(world, targetX, targetY, hungerGain))
    {
        return false;
    }

    increaseHunger(hungerGain, Config::HUMAN_MAX_HUNGER);
    return true;
}

/*
    Tries to drink water from an adjacent tile.
*/
bool Human::tryDrinkAt(GameWorld& world, int targetX, int targetY)
{
    if (!GridUtils::isFourNeighborDistance(x, y, targetX, targetY))
    {
        return false;
    }

    if (!Lake::drinkWaterAt(world, targetX, targetY))
    {
        return false;
    }

    increaseThirst(Config::HUMAN_THIRST_PER_TICK_WATER, Config::HUMAN_MAX_THIRST);

    return true;
}

/*
    Humans currently do not attack.
*/
bool Human::tryAttackAt(GameWorld& world, int targetX, int targetY)
{
    return false;
}

/*
    Gives the human a random brain.
*/
void Human::setRandomBrain()
{
    setBrain(std::make_unique<RandomBrain>());
}

/*
    Gives the human a manual brain.
*/
void Human::setManualBrain()
{
    setBrain(std::make_unique<ManualBrain>());
}

/*
    Sends an action to the manual brain.
    If the human is not already manual-controlled, it switches first.
*/
void Human::giveManualAction(const Action& action)
{
    ManualBrain* manualBrain = dynamic_cast<ManualBrain*>(getBrain());

    if (manualBrain == nullptr)
    {
        setManualBrain();
        manualBrain = dynamic_cast<ManualBrain*>(getBrain());
    }

    if (manualBrain != nullptr)
    {
        manualBrain->setNextAction(action);
    }
}

/*
    Checks whether two humans are valid mating partners right now.
*/
bool Human::canMateWith(const Human& other) const
{
    if (dead || other.dead)
    {
        return false;
    }

    if (!canMateNow() || !other.canMateNow())
    {
        return false;
    }

    if (!GridUtils::isFourNeighborDistance(x, y, other.x, other.y))
    {
        return false;
    }

    return gender != other.gender;
}

/*
    Human mating is resolved as a pair action later, not immediately here.
*/
bool Human::tryMateAt(GameWorld& world, int targetX, int targetY)
{
    return false;
}

/*
    Finds an adjacent human this human can mate with.
*/
Human* Human::getAdjacentMateCandidate(const Human& human)
{
    for (auto& other : humans)
    {
        if (other.getId() == human.getId())
        {
            continue;
        }

        if (human.canMateWith(other))
        {
            return &other;
        }
    }

    return nullptr;
}

/*
    Finds an open neighboring cell around either parent for the child to spawn in.
*/
bool Human::findChildSpawnCell(GameWorld& world, const Human& parentA, const Human& parentB, int& childX, int& childY)
{
    const int candidates[8][2] = {{parentA.getX() + 1, parentA.getY()}, {parentA.getX() - 1, parentA.getY()}, {parentA.getX(), parentA.getY() + 1}, {parentA.getX(), parentA.getY() - 1}, {parentB.getX() + 1, parentB.getY()}, {parentB.getX() - 1, parentB.getY()}, {parentB.getX(), parentB.getY() + 1}, {parentB.getX(), parentB.getY() - 1}};

    for (const auto& candidate : candidates)
    {
        int x = candidate[0];
        int y = candidate[1];

        if (!world.isInsideGrid(x, y))
        {
            continue;
        }

        TileType tile = world.getTile(x, y);

        if (tile == TileType::Water || tile == TileType::Crop)
        {
            continue;
        }

        if (Human::isBlockingEntityAt(x, y))
        {
            continue;
        }

        childX = x;
        childY = y;
        return true;
    }

    return false;
}

/*
    Resolves mating after all humans have chosen their actions.
    A child is only created when both parents target each other on the same tick.
*/
void Human::resolveMatingActions(GameWorld& world)
{
    std::set<int> alreadyMatedIds;

    auto alreadyMated = [&](int id)
    {
        return alreadyMatedIds.count(id) > 0;
    };

    int originalCount = static_cast<int>(humans.size());

    for (int i = 0; i < originalCount; ++i)
    {
        Human& parentA = humans[i];

        if (parentA.dead || alreadyMated(parentA.getId()) || !parentA.hasPreparedAction() || parentA.getPreparedAction().type != ActionType::Mate)
        {
            continue;
        }

        for (int j = i + 1; j < originalCount; ++j)
        {
            Human& parentB = humans[j];

            if (parentB.dead || alreadyMated(parentB.getId()) || !parentB.hasPreparedAction() || parentB.getPreparedAction().type != ActionType::Mate)
            {
                continue;
            }

            if (!parentA.canMateWith(parentB))
            {
                continue;
            }

            if (!targetsEntity(parentA.getPreparedAction(), parentB) || !targetsEntity(parentB.getPreparedAction(), parentA))
            {
                continue;
            }

            int childX;
            int childY;

            if (!findChildSpawnCell(world, parentA, parentB, childX, childY))
            {
                continue;
            }

            int parentAId = parentA.getId();
            int parentBId = parentB.getId();

            humans.emplace_back(childX, childY);
            int childId = humans.back().getId();

            // emplace_back can move vector storage, so parent references are refreshed by ID after birth.
            EntityOccupancy::rebuild(world);

            DebugLog::birth("Human", parentAId, parentBId, childId);

            Human* parentAAfterBirth = Human::getById(parentAId);
            Human* parentBAfterBirth = Human::getById(parentBId);

            Human* mother = nullptr;

            if (parentAAfterBirth != nullptr && parentAAfterBirth->getGender() == Gender::Female)
            {
                mother = parentAAfterBirth;
            }
            else if (parentBAfterBirth != nullptr && parentBAfterBirth->getGender() == Gender::Female)
            {
                mother = parentBAfterBirth;
            }

            if (mother != nullptr)
            {
                mother->takeDamage(Config::HUMAN_FEMALE_MATING_HEALTH_COST);
                mother->checkDeath();
            }

            if (parentAAfterBirth != nullptr)
            {
                parentAAfterBirth->addChild(childId);
                parentAAfterBirth->startMatingCooldown(Config::HUMAN_MATING_COOLDOWN_TICKS);
                parentAAfterBirth->clearPreparedAction();
            }

            if (parentBAfterBirth != nullptr)
            {
                parentBAfterBirth->addChild(childId);
                parentBAfterBirth->startMatingCooldown(Config::HUMAN_MATING_COOLDOWN_TICKS);
                parentBAfterBirth->clearPreparedAction();
            }

            alreadyMatedIds.insert(parentAId);
            alreadyMatedIds.insert(parentBId);

            break;
        }
    }
}

/*
    Finds a human by unique ID.
*/
Human* Human::getById(int id)
{
    for (Human& human : humans)
    {
        if (human.getId() == id)
        {
            return &human;
        }
    }

    return nullptr;
}

/*
    Finds the nearest living human or edible body within a limited range.
*/
Human* Human::getNearestLivingHumanOrBodyWithinRange(int x, int y, int range)
{
    if (range <= 0)
    {
        return nullptr;
    }

    Human* bestHuman = nullptr;
    int bestDistance = range + 1;

    if (EntityOccupancy::hasBeenBuilt())
    {
        for (int dy = -range; dy <= range; ++dy)
        {
            for (int dx = -range; dx <= range; ++dx)
            {
                int distance = std::abs(dx) + std::abs(dy);

                if (distance == 0 || distance > range)
                {
                    continue;
                }

                if (distance >= bestDistance)
                {
                    continue;
                }

                Human* human = EntityOccupancy::getHumanAt(x + dx, y + dy);

                if (human == nullptr)
                {
                    continue;
                }

                if (human->isDead())
                {
                    continue;
                }

                bestHuman = human;
                bestDistance = distance;
            }
        }
    }
    else
    {
        for (Human& human : humans)
        {
            if (human.dead)
            {
                continue;
            }

            int distance = GridUtils::manhattanDistance(x, y, human.x, human.y);

            if (distance <= range && distance < bestDistance)
            {
                bestHuman = &human;
                bestDistance = distance;
            }
        }
    }

    for (Human& human : humans)
    {
        if (!human.hasBody())
        {
            continue;
        }

        if (!human.isBodyEdible())
        {
            continue;
        }

        int distance = GridUtils::manhattanDistance(x, y, human.x, human.y);

        if (distance <= range && distance < bestDistance)
        {
            bestHuman = &human;
            bestDistance = distance;
        }
    }

    return bestHuman;
}
