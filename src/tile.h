#pragma once

#include <set>
#include <string>

#include <cstdlib>
#include <ctime> 
#include <godot_cpp/variant/vector2i.hpp>

using std::set;
using std::string;
using namespace godot;

class Tile {
    private:

        Vector2i coords;
        set<string> possibleTiles;
    
    public:

        Tile(Vector2i coords, set<string> possibleTiles);

        int getPriority();

        void removeType(string str);

        Vector2i getCoords();

        // picks a random tileType among its options, and then returns it
        string collapseTile();
};