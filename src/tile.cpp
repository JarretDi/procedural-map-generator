#include "tile.h"

Tile::Tile(Vector2i coords, set<Vector2i> possibleTiles) : coords(coords), possibleTiles(possibleTiles) {

}

int Tile::getPriority() {
    return possibleTiles.size();
}

void Tile::removeType(Vector2i str) {
    possibleTiles.erase(str);
}

Vector2i Tile::getCoords() {
    return coords;
}

Vector2i Tile::collapseTile() {
    auto it = possibleTiles.begin();

    std::srand(std::time(0));
    int selectedIndex = std::rand() % possibleTiles.size();

    for (int i = 0; i < selectedIndex; i++) {
        it++;
    }

    Vector2i selectedType = *it;

    possibleTiles.clear();

    return selectedType;
}