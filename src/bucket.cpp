#include "bucket.h"

void Bucket::insert(Tile tile) {
    tiles.push_back(tile);
    tileToIndex[tile.getCoords()] = tiles.size();
}

Tile * Bucket::updateTile(pair<int, int> tileCoords, string tileType) {
    int tileIndex = tileToIndex[tileCoords];
    return tiles[tileIndex].removeType(tileType);
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