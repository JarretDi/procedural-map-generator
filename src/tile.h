#pragma once

#include <set>
#include <random>

#include <godot_cpp/variant/vector2i.hpp>

using std::set;
using namespace godot;

class Tile {
    private:
        // Vector2i used for coords on map
        Vector2i coords;

        // Vector2i used to get atlas coords
        set<Vector2i> possibleTiles;
    
    public:

        Tile(const Vector2i coords, const set<Vector2i> possibleTiles);

        int getPriority() const;

        void removeType(const Vector2i type);

        Vector2i getCoords() const;

        // picks a random tileType among its options, and then returns it
        Vector2i collapseTile();
};