#include "bucket.h"

void Bucket::insert(Tile tile) {
    //std::cerr << "Inserting tile at" << tile.getCoords().x << ", " << tile.getCoords().y << "\n";
    //std::cerr << "tiles has size: " << tiles.size() << ", and tileToIndex has size: " << tileToIndex.size() << "\n";
    tiles.push_back(tile);
    //std::cerr << "tile has been pushed in \n"; 
    tileToIndex[tile.getCoords()] = tiles.size() - 1;
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

const Tile & Bucket::getTile(const Vector2i & coords) const {
    return tiles[tileToIndex.at(coords)];
}

bool Bucket::containsTile(const Vector2i & tileCoords) const {
    return tileToIndex.find(tileCoords) != tileToIndex.end();
}

bool Bucket::isEmpty() const {
    return tiles.empty();
}