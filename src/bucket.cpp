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

    int tileIndex = RandomGenerator::getInt(0, tiles.size() - 1);

    return removeTile(tiles[tileIndex].getCoords());
}

Tile Bucket::removeTile(const Vector2i & coordsToRemove) {
    int tileIndex = tileToIndex.at(coordsToRemove);

    Vector2i lastCoords = tiles.back().getCoords();

    if (tileIndex != tiles.size() - 1) {
        std::swap(tiles[tileIndex], tiles.back());
        tileToIndex[lastCoords] = tileIndex;
    } 

    tileToIndex.erase(coordsToRemove);

    Tile ret = tiles.back();
    tiles.pop_back();

    return ret;
}

bool Bucket::containsTile(const Vector2i & tileCoords) const {
    return tileToIndex.find(tileCoords) != tileToIndex.end();
}

bool Bucket::isEmpty() const {
    return tiles.empty();
}

const Tile & Bucket::operator[](const Vector2i & tileCoords) const {
    auto index = tileToIndex.find(tileCoords);
    if (index != tileToIndex.end()) {
        return tiles[index->second];
    } else {
        throw std::out_of_range("Tile not found in bucket");
    }
}