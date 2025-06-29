#pragma once

#include <utility>
#include <vector>
#include <string>

using std::pair;
using std::vector;
using std::string;

class Tile {
    pair<int, int> coords;
    vector<string> possibleTiles;

    Tile(pair<int, int> coords, vector<string> possibleTiles);

    int getPriority();
};