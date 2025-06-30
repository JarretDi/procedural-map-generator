#pragma once

#include <utility>
#include <set>
#include <string>

using std::pair;
using std::set;
using std::string;

class Tile {
    private:

        pair<int, int> coords;
        set<string> possibleTiles;
    
    public:

        Tile(pair<int, int> coords, set<string> possibleTiles);

        int getPriority();

        bool removePossibleTile(string str);

        pair<int, int> getCoords();
};