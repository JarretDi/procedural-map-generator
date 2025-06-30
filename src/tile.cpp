#include "tile.h"

Tile::Tile(pair<int, int> coords, set<string> possibleTiles) : coords(coords), possibleTiles(possibleTiles) {

}

int Tile::getPriority() {
    return possibleTiles.size();
}

bool Tile::removePossibleTile(string str) {
    return possibleTiles.erase(str);
}