#pragma once

#include <set>
#include <unordered_map>
#include "bucket.h"
#include "vector2i_hash.hpp"

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>

#include <godot_cpp/classes/tile_map_layer.hpp>

using namespace godot;

using std::unordered_map;

class TileMapGenerator : public TileMapLayer {
    GDCLASS(TileMapGenerator, TileMapLayer)

    protected:
        static void _bind_methods();

    private:
        struct BQNode {
            Bucket bucket;
            BQNode * next;
            BQNode * prev;
        };

        BQNode * head;
        BQNode * tail;

        // maps a tile type to a list of valid neighbours within radius
        unordered_map<Vector2i, std::set<Vector2i>> typeRules;

        std::set<Vector2i> types;

        int mapDimensions;
        int radius;

        void createBuckets(int length);

        // picks a tile among those with the highest priority > 1
        // picks a random tileType among its possible types
        // adds string to corresponding location on map
        Vector2i collapseTile();

        // returns true if there are no more tiles in a node after collapsed
        bool hasTilesToCollapse();

        // updates neighbouring tiles within radius of center based on tile rules
        void propogate(Vector2i center, Vector2i type);

        // finds all types that don't work with given type
        // removes those types from given tile
        // moves the tile to the right bucket based on new possibilities
        void updateTile(Vector2i tileCoords, Vector2i tileType);
    
    public:
        TileMapGenerator(int mapDimensions, int radius = 1);

        // takes a godot dictionary (from the engine) sets typeRules to match it in c++
        // additionally runs the generator itself
        void generate(Dictionary tileDict);

        ~TileMapGenerator();
};