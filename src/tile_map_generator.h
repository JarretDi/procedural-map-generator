#pragma once

#include <array>
#include <bitset>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

#include "bucket.h"
#include "vector2i_hash.hpp"
#include "random_generator.hpp"
#include "config.hpp"

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp>

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/classes/tile_map_layer.hpp>

using std::array;
using std::bitset;
using std::queue;
using std::unordered_map;
using std::vector;

using namespace godot;

class TileMapGenerator : public TileMapLayer {
    GDCLASS(TileMapGenerator, TileMapLayer)

    protected:
        static void _bind_methods();

    private:
        vector<Bucket> buckets;

        // maps a tile type to a list of valid neighbours within radius
        // accounts for direction
        vector<array<bitset<TILE_TYPE_COUNT>, 4>> typeRules;

        enum Direction {
            UP = 0,
            RIGHT = 1,
            DOWN = 2,
            LEFT = 3
        };

        // maps a bitset index to a Vector2i (tileset atlas coord)
        vector<Vector2i> idxToType;

        int tileAtlas;
        int mapDimensions;

        /*
        Given Dictionary should look something like (ABC is Vector2i):
        var example {
            GRASS : [[GRASS, WATER], [GRASS], [GRASS], [GRASS]],
            WATER : [[GRASS], [WATER], [WATER, GRASS], [WATER]],
        } (where index 0, 1, 2, 3 is up, right, down, left) */
        void parseDictionary(const Dictionary & tileDict);

        // updates neighbouring tiles within radius of center based on tile rules
        void propagate(Vector2i collapsedCoords, int typeIdx);

        // finds all types that don't work with given type
        // removes those types from given tile
        // moves the tile to the right bucket based on new possibilities
        // returns whether its tiles' possibilities changed
        bool updateTile(const Vector2i & tileCoords, const bitset<TILE_TYPE_COUNT> & rules);

        int findTile(Vector2i tileCoords);
    
    public:
        TileMapGenerator();

        // takes a godot dictionary (from the engine) sets typeRules to match it in c++
        void build(Dictionary tileDict, int tileAtlas, int mapDimensions);

        // since the generation moves out from one area, generating a tile within a chunk helps create variety
        // generates one tile for each chunk (so there will be chunks^2 tiles placed)
        // takes in an optional array as to what should be seeded,
        // inOrder is whether it should be chosen randomly from types, or in the order given (wraps)
        // void seed(int chunks, Array types = Array(), bool inOrder = false);
        // SEED IS DEPRECATED AND DOESN'T MAKE SENSE WITH CURRENT BRANCH LOGIC

        // returns true if there are no more tiles in a node after collapsed
        bool hasTilesToCollapse();

        // picks a tile among those with the highest priority > 1
        // picks a random tileType among its possible types
        // adds string to corresponding location on map
        // if given a coordinate, collapses that one instead of doing it randomly
        // void collapseTile(const Vector2i & coords, int typeIdx);

        void collapseRandomTile();

        // goes through all the tiles
        // if a tile is surrounded by 3 different identical ones,
        // switches to that type
        void refine();

        // consumes a sample map
        // returns a dictionary that contains the rules for each tile
        Dictionary parseRules(TileMapLayer * sample, int size);

        void printTiles();
};