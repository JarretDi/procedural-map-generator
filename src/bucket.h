#pragma once

#include <vector>
#include <unordered_map>
#include <tile.h>

using std::vector;
using std::unordered_map;

class Bucket {
    private: 
        // Stores the tiles
        vector<Tile> tiles;
        
        // Maps a tile to its index
        unordered_map<Tile, int> tileToIndex;
};