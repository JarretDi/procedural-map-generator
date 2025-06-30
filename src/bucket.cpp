#include "bucket.h"

void Bucket::insert(Tile tile) {
    tiles.push_back(tile);
    tileToIndex[tile.getCoords()] = tiles.size();
}

void Bucket::updateTile(Tile tile, string tileType) {
    int tileIndex = tileToIndex[tile.getCoords()];
    tiles[tileIndex].removePossibleTile(tileType);
}

Tile Bucket::removeRandom(Tile tile) {
    std::srand(std::time(0));
    int tileIndex = std::rand() % tiles.size();

    pair<int, int> curCoords  = tiles[tileIndex].getCoords();
    pair<int, int> lastCoords = tiles.back().getCoords();

    std::swap(tiles[tileIndex], tiles.back());

    tileToIndex[lastCoords] = tileIndex;
    tileToIndex.erase(curCoords);

    Tile ret = tiles.back();
    tiles.pop_back();
    return ret;
}