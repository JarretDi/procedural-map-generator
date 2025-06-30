#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <tile.h>

#include <cstdlib>
#include <ctime> 

using std::vector;
using std::unordered_map;
using std::string;

class Bucket {
    private: 
        // Stores the tiles
        vector<Tile> tiles;
        
        // Maps a tile to its index
        unordered_map<pair<int,int>, int> tileToIndex;

    public:
        void insert(Tile tile);

        void updateTile(pair<int, int> tileCoords, string opToRemove);

        Tile removeRandom(Tile tile);
};