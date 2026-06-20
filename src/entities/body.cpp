#include "entities/body.h"

#include "core/config.h"
#include "core/gridutils.h"
#include "environment/forest.h"

#include <algorithm>
#include <cmath>

std::vector<Body> Body::bodies;

/*
    Creates a body with one shared body lifetime and the same eating settings bodies used before.
*/
Body::Body(int gridX, int gridY, BodySource source, TileType tileUnderBody) : Food(Config::TICKS_PER_MEAL_HUMAN, Config::HUNGER_PER_TICK_MEAL_HUMAN), x(gridX), y(gridY), source(source), tileUnderBody(tileUnderBody), ticksRemaining(Config::TICKS_PER_DEAD_BODY)
{
}

/*
    Adds a body on top of the current tile.
    The visible world tile becomes Body, while the original tile is stored underneath.
*/
void Body::addBodyAt(GameWorld& world, int x, int y, BodySource source)
{
    if (!world.isInsideGrid(x, y))
    {
        return;
    }

    if (isBodyAt(x, y))
    {
        return;
    }

    TileType tileUnderBody = world.getTile(x, y);

    bodies.emplace_back(x, y, source, tileUnderBody);

    world.setTile(x, y, TileType::Body);
}

/*
    Updates decay timers and removes bodies that expired.
    The body is erased before restoring the tile so GameWorld::setTile does not treat the write as an underneath-body update.
*/
void Body::update(GameWorld& world)
{
    for (int i = static_cast<int>(bodies.size()) - 1; i >= 0; --i)
    {
        bodies[i].ticksRemaining--;

        if (bodies[i].isExpired())
        {
            int restoreX = bodies[i].x;
            int restoreY = bodies[i].y;
            TileType restoreTile = bodies[i].tileUnderBody;

            bodies.erase(bodies.begin() + i);

            world.setTile(restoreX, restoreY, restoreTile);
        }
    }
}

/*
    Draws all visible bodies.
*/
void Body::drawBodies(GameWorld& world)
{
    for (const Body& body : bodies)
    {
        world.drawTile(body.x, body.y, TileType::Body);
    }
}

/*
    Returns whether a body currently covers this tile.
*/
bool Body::isBodyAt(int x, int y)
{
    for (const Body& body : bodies)
    {
        if (body.x == x && body.y == y)
        {
            return true;
        }
    }

    return false;
}

/*
    Returns whether this body can still be eaten.
*/
bool Body::isEdibleBodyAt(int x, int y)
{
    for (const Body& body : bodies)
    {
        if (body.x == x && body.y == y && body.isEdible() && !body.claimedThisTick)
        {
            return true;
        }
    }

    return false;
}

/*
    Eats one tick from a body.
    The body is erased before restoring the tile so the visible Body tile is actually removed.
*/
bool Body::eatBodyAt(GameWorld& world, int x, int y, int& hungerGain)
{
    hungerGain = 0;

    for (int i = 0; i < static_cast<int>(bodies.size()); ++i)
    {
        Body& body = bodies[i];

        if (body.x != x || body.y != y)
        {
            continue;
        }

        if (!body.isEdible() || body.claimedThisTick)
        {
            return false;
        }

        if (!body.eatOneTick())
        {
            return false;
        }

        body.claimedThisTick = true;
        hungerGain = body.getHungerPerTick();

        if (body.isFullyEaten())
        {
            int restoreX = body.x;
            int restoreY = body.y;
            TileType restoreTile = body.tileUnderBody;

            bodies.erase(bodies.begin() + i);

            world.setTile(restoreX, restoreY, restoreTile);
        }

        return true;
    }

    return false;
}

/*
    Finds an edible body directly next to a position.
*/
Body* Body::getAdjacentEdibleBody(int x, int y)
{
    for (Body& body : bodies)
    {
        if (!body.isEdible() || body.claimedThisTick)
        {
            continue;
        }

        if (GridUtils::isFourNeighborDistance(x, y, body.x, body.y))
        {
            return &body;
        }
    }

    return nullptr;
}

/*
    Finds the nearest edible body within range.
    Bodies hidden in forests are ignored unless the predator is directly next to them.
*/
Body* Body::getNearestBodyWithinRange(const GameWorld& world, int x, int y, int range)
{
    Body* bestBody = nullptr;
    int bestDistance = range + 1;

    for (Body& body : bodies)
    {
        if (!body.isEdible())
        {
            continue;
        }

        int distance = GridUtils::manhattanDistance(x, y, body.x, body.y);

        if (distance <= 0 || distance > range || distance >= bestDistance)
        {
            continue;
        }

        if (Forest::isForestTile(world, body.x, body.y) && !GridUtils::isFourNeighborDistance(x, y, body.x, body.y))
        {
            continue;
        }

        bestBody = &body;
        bestDistance = distance;
    }

    return bestBody;
}

/*
    Allows bodies to be eaten again on the next simulation tick.
*/
void Body::resetEatingClaims()
{
    for (Body& body : bodies)
    {
        body.claimedThisTick = false;
    }
}

/*
    Updates the hidden tile underneath a body.
    This is how weather, crop, water, and terrain changes still affect body-covered cells.
*/
void Body::setTileUnderBodyAt(int x, int y, TileType tile)
{
    for (Body& body : bodies)
    {
        if (body.x == x && body.y == y)
        {
            body.tileUnderBody = tile;
            return;
        }
    }
}

/*
    Returns the body's x position.
*/
int Body::getX() const
{
    return x;
}

/*
    Returns the body's y position.
*/
int Body::getY() const
{
    return y;
}

/*
    Returns which kind of entity created this body.
*/
BodySource Body::getSource() const
{
    return source;
}

/*
    Returns whether this body should disappear.
*/
bool Body::isExpired() const
{
    return ticksRemaining <= 0 || isFullyEaten();
}

/*
    Restores the latest tile that should exist underneath the body.
*/
void Body::restoreTile(GameWorld& world) const
{
    world.setTile(x, y, tileUnderBody);
}