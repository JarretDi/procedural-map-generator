#include "tile.h"

Tile::Tile(const Vector2i & coords, const set<Vector2i> & possibleTiles) : coords(coords), possibleTiles(possibleTiles) {

}

int Tile::getPriority() const {
    return possibleTiles.size();
}

void Tile::removeTypesNotIn(const set<Vector2i> & types) {
    for (auto it = possibleTiles.begin(); it != possibleTiles.end(); ) {
        // i.e. if it is not in types
        if (types.find(*it) == types.end()) {
            it = possibleTiles.erase(it);
        } else {
            it++;
        }
    }
}

Vector2i Tile::getCoords() const {
    return coords;
}

Vector2i Tile::collapseTile(){
    if (possibleTiles.empty()) {
        return Vector2i(0, 0);
    }

    int selectedIndex = RandomGenerator::getInt(0, possibleTiles.size() - 1);

    auto it = possibleTiles.begin();

    for (int i = 0; i < selectedIndex; i++) {
        it++;
    }

    Vector2i selectedType = *it;

    possibleTiles.clear();

    return selectedType;
}