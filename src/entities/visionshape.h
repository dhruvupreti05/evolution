#pragma once

/*
    Human vision shapes that can be swapped from Config.
    Only ForwardTriangle uses orientation; the centered shapes see equally around the human.
*/
enum class VisionShape
{
    ForwardTriangle,
    Circle,
    Square,
    Diamond
};
