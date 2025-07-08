#pragma once

#include <set>
#include <unordered_map>

#include "bucket.h"
#include "vector2i_hash.hpp"
#include "random_generator.hpp"

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/os.hpp>
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

        int tileAtlas;
        int mapDimensions;
        int radius;

        void createBuckets(int length);

        // updates neighbouring tiles within radius of center based on tile rules
        void propagate(const Vector2i & center, const Vector2i & type);

        // finds all types that don't work with given type
        // removes those types from given tile
        // moves the tile to the right bucket based on new possibilities
        void updateTile(const Vector2i & tileCoords, const Vector2i & tileType);

        BQNode * findTile(Vector2i tileCoords);
    
    public:
        TileMapGenerator();

        ~TileMapGenerator();

        // takes a godot dictionary (from the engine) sets typeRules to match it in c++
        void build(Dictionary tileDict, int tileAtlas, int mapDimensions, int radius = 1);

        // since the generation moves out from one area, generating a tile within a chunk helps create variety
        // generates one tile for each chunk (so there will be chunks^2 tiles placed)
        // takes in an optional array as to what should be seeded,
        // inOrder is whether it should be chosen randomly from types, or in the order given (wraps)
        void seed(int chunks, Array types = Array(), bool inOrder = false);

        // returns true if there are no more tiles in a node after collapsed
        bool hasTilesToCollapse();

        // picks a tile among those with the highest priority > 1
        // picks a random tileType among its possible types
        // adds string to corresponding location on map
        // if given a coordinate, collapses that one instead of doing it randomly
        void collapseTile(const Vector2i & givenCoords = Vector2i(-1, -1), const Vector2i & givenType = Vector2i(-1, -1));

        void collapseRandomTile();
};