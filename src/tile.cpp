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

int Tile::collapseTile(){
    int count = possibleTiles.count();

    if (count == 0) {
        return -1;
    }

    int index = RandomGenerator::getInt(0, count - 1);

    for (int i = 0; i < possibleTiles.size(); i++) {
        if (possibleTiles[i]) {
            if (index == 0) {
                return i;
            } else {
                index--;
            }
        }
    }
    return -1;
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