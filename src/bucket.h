#pragma once

#include <vector>
#include <map>
#include <tile.h>

#include <cstdlib>
#include <ctime> 
#include <stdexcept>

using std::vector;
using std::map;

using namespace godot;

class Bucket {
    private: 
        // Stores the tiles
        vector<Tile> tiles;
        
        // Maps a tile to its index
        map<Vector2i, int> tileToIndex;

    public:
        void insert(Tile tile);

        void updateTile(Vector2i tileCoords, Vector2i opToRemove);

        Tile removeRandom();

        Tile removeTile(Vector2i coordsToRemove);

        bool isEmpty();
};