#pragma once

#include <utility>
#include <set>
#include <string>

#include <cstdlib>
#include <ctime> 

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

        void removeType(string str);

        pair<int, int> getCoords();

        // picks a random tileType among its options, and then returns it
        string collapseTile();
};