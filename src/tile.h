#pragma once

#include <array>
#include <bitset>

#include "random_generator.hpp"
#include "config.hpp"

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
        bitset<TILE_TYPE_COUNT> possibleTiles;
    
    public:

        Tile(const Vector2i & coords, int typeCount);

        // returns true if something changed
        bool removeTypesNotIn(const bitset<TILE_TYPE_COUNT> & types);

        // picks a random tileType among its options in given direction
        // then returns the index of the bit
        int collapseTile();

        int getPriority() const;
        
        Vector2i getCoords() const;

        bitset<TILE_TYPE_COUNT> getPossibleTiles() const;
};