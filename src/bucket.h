#pragma once

#include <vector>
#include <map>
#include <string>
#include <tile.h>

#include <cstdlib>
#include <ctime> 
#include <stdexcept>

using std::vector;
using std::map;
using std::string;

class Bucket {
    private: 
        // Stores the tiles
        vector<Tile> tiles;
        
        // Maps a tile to its index
        map<pair<int,int>, int> tileToIndex;

    public:
        void insert(Tile tile);

        void updateTile(pair<int, int> tileCoords, string opToRemove);

        Tile removeRandom();

        Tile removeTile(pair<int, int> coordsToRemove);

        bool isEmpty();
};