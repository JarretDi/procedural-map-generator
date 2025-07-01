#pragma once

#include <set>
#include <string>
#include "bucket.h"

using std::set;
using std::string;

class BucketQueue {
    private:
        struct BQNode {
            Bucket bucket;
            BQNode * next;
            BQNode * prev;
        };

        BQNode * head;
        BQNode * tail;

        vector<vector<string>> map;

        // maps a tile type to a list of valid neighbours within radius
        unordered_map<string, set<string>> typeRules;

        int radius;

        void createBuckets(int length);

        // picks a tile among those with the highest priority > 1
        // picks a random tileType among its possible types
        // adds string it to corresponding location on map
        string collapseTile();

        // returns true if there are no more tiles in a node after collapsed
        bool hasTilesToCollapse();

        // updates neighbouring tiles within radius of center based on tile rules
        void propogate(pair<int, int> center, string type);

        // finds all types that don't work with given type
        // removes those types from given tile
        // moves the tile to the right bucket based on new possibilities
        void updateTile(pair<int, int> tileCoords, string tileType);
    
    public:
        BucketQueue(unordered_map<string, set<string>> & typesRules, int mapDimensions, int radius = 1);

        ~BucketQueue();
};