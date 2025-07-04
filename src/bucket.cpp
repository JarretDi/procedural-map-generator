#include "bucket.h"

void Bucket::insert(Tile tile) {
    tiles.push_back(tile);
    tileToIndex[tile.getCoords()] = tiles.size();
}

void Bucket::updateTile(Vector2i tileCoords, string tileType) {
    int tileIndex = tileToIndex.at(tileCoords);
    return tiles[tileIndex].removeType(tileType);
}

Tile Bucket::removeRandom() {
    std::srand(std::time(0));
    int tileIndex = std::rand() % tiles.size();

    return removeTile(tiles[tileIndex].getCoords());
}

Tile Bucket::removeTile(Vector2i coordsToRemove) {
    int tileIndex = tileToIndex[coordsToRemove];

    Vector2i lastCoords = tiles.back().getCoords();

    std::swap(tiles[tileIndex], tiles.back());

    tileToIndex[lastCoords] = tileIndex;
    tileToIndex.erase(coordsToRemove);

    Tile ret = tiles.back();
    tiles.pop_back();

    return ret;
}

bool Bucket::isEmpty() {
    return tiles.size() == 0;
}