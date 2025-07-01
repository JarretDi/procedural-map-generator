#pragma once

#include "bucket.h"

class BucketQueue {
    private:
        class BQNode {
            Bucket bucket;
            Bucket * next;
            Bucket * prev;
        };

        vector<vector<string>> map;

        BQNode * head;
        BQNode * tail;

        void createBuckets(int length);

        // picks a tile among those with the highest priority > 1
        // picks a random tileType among its possible types
        // adds string it to corresponding location on map
        string collapseTile();

        // returns true if there are no more tiles in a node after collapsed
        bool hasTilesToCollapse();

        // removes tileType from the given tiles
        void updateTile(pair<int, int> tileCoords, string tileType);
    
    public:
        BucketQueue(vector<string> types, int mapDimensions);

        ~BucketQueue();
};