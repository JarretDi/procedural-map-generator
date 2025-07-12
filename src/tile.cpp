#include "tile.h"

Tile::Tile(const Vector2i & coords, int typeCount) : coords(coords) {
    for (int i = 0; i < typeCount; i++) {
        possibleTiles.set(i);
    }
}

bool Tile::removeTypesNotIn(const bitset<TILE_TYPE_COUNT> & types) {
    bitset<TILE_TYPE_COUNT> old = possibleTiles;
    possibleTiles &= types;
    return old != possibleTiles;
}

int Tile::collapseTile(const vector<int> & freqMap) {
    if (possibleTiles.count() == 0) {
        return -1;
    }

    vector<int> weights;
    weights.resize(possibleTiles.size());
    for (int i = 0; i < possibleTiles.size(); i++) {
        if (possibleTiles[i]) {
            weights[i] = freqMap[i];
        }
    }

    int idx = RandomGenerator::getIntDistribution(weights);
    possibleTiles.reset();
    possibleTiles.set(idx);

    return idx;
}

int Tile::getPriority() const {
    return possibleTiles.count();
}

Vector2i Tile::getCoords() const {
    return coords;
}

bitset<TILE_TYPE_COUNT> Tile::getPossibleTiles() const {
    return possibleTiles;
}