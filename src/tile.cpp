#include "tile.h"

Tile::Tile(const Vector2i & coords, int typeCount) : coords(coords) {
    for (int i = 0; i < typeCount; i++) {
        possibleTiles.set(i);
    }
}

int Tile::getPriority() const {
    return possibleTiles.count();
}

bool Tile::removeTypesNotIn(const bitset<TILE_TYPE_COUNT> & types) {
    bitset<TILE_TYPE_COUNT> old = possibleTiles;
    possibleTiles &= types;
    return old != possibleTiles;
}

Vector2i Tile::getCoords() const {
    return coords;
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