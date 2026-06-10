#include <SFML/Graphics.hpp>
#include <set>

#include "block.h"


class Clump
{   
    public:
        Clump();
    private:
        std::set<Block> clump;       
};