#include "bucket.h"

void Bucket::insert(const Tile & tile) {
    tiles.push_back(tile);
    tileToIndex[tile.getCoords()] = tiles.size() - 1;
}

void Bucket::updateTile(const Vector2i & tileCoords, const Vector2i & tileType) {
    int tileIndex = tileToIndex.at(tileCoords);
    tiles[tileIndex].removeType(tileType);
}

Tile Bucket::removeRandom() {
    if (tiles.empty()) {
        throw std::runtime_error("Bucket is empty");
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, tiles.size() - 1);

    int tileIndex = dist(gen);

    return removeTile(tiles[tileIndex].getCoords());
}

Tile Bucket::removeTile(const Vector2i & coordsToRemove) {
    int tileIndex = tileToIndex[coordsToRemove];

    Vector2i lastCoords = tiles.back().getCoords();

    std::swap(tiles[tileIndex], tiles.back());

    tileToIndex[lastCoords] = tileIndex;
    tileToIndex.erase(coordsToRemove);

    Tile ret = tiles.back();
    tiles.pop_back();

    return ret;
}

bool Bucket::isEmpty() const {
    return tiles.size() == 0;
}