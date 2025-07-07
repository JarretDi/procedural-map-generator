#pragma once

#include <vector>
#include <unordered_map>
#include <tile.h>

#include <cstdlib>
#include <ctime> 
#include <stdexcept>

#include "vector2i_hash.hpp"
#include "random_generator.hpp"

using std::vector;
using std::unordered_map;

using namespace godot;

class Bucket {
    private: 
        // Stores the tiles
        vector<Tile> tiles;
        
        // Maps a tile to its index
        unordered_map<Vector2i, int> tileToIndex;

    public:
        void insert(const Tile & tile);

        Tile removeRandom();

        Tile removeTile(const Vector2i & coordsToRemove);

        bool containsTile(const Vector2i & tileCoords) const;

        bool isEmpty() const;

        Tile & operator[](const Vector2i & tileCoords);
};