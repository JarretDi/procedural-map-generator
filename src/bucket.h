#pragma once

#include <vector>
#include <unordered_map>
#include <tile.h>

#include <cstdlib>
#include <ctime> 
#include <stdexcept>

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

        void updateTile(const Vector2i & tileCoords, const Vector2i & opToRemove);

        Tile removeRandom();

        Tile removeTile(const Vector2i & coordsToRemove);

        bool isEmpty() const;
};