#pragma once

#include <array>
#include <bitset>
#include "random_generator.hpp"

#include <godot_cpp/variant/vector2i.hpp>

using std::bitset;
using std::array;
using namespace godot;

class Tile {
    private:
        // Vector2i used for coords on map
        Vector2i coords;

        // represents the possible types, with each bit mapped to one type
        // 1 represents possible, 0 represents impossible
        bitset<32> possibleTiles;
    
    public:

        Tile(const Vector2i & coords, int typeCount);

        int getPriority() const;

        void removeTypesNotIn(const bitset<32> & types);

        Vector2i getCoords() const;

        // picks a random tileType among its options in given direction
        // then returns the index of the bit
        int collapseTile();
};