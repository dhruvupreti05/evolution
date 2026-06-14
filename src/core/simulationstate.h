#pragma once

#include <vector>

#include "entities/human.h"
#include "entities/predator.h"
#include "entities/crop.h"

/*
    Small access point for the current simulation data.
    This wraps the static entity lists so UI code can read counts and collections from one place.
*/
class SimulationState
{
public:
    std::vector<Human>& humans()
    {
        return Human::getHumans();
    }

    const std::vector<Predator>& predators() const
    {
        return Predator::getPredators();
    }

    const std::vector<Crop>& crops() const
    {
        return Crop::getCrops();
    }

    int humanCountAlive() const
    {
        return Human::countAlive();
    }

    int predatorCountAlive() const
    {
        return Predator::countAlive();
    }
};