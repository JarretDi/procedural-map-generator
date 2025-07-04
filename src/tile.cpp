#include "tile.h"

Tile::Tile(Vector2i coords, set<string> possibleTiles) : coords(coords), possibleTiles(possibleTiles) {

}

int Tile::getPriority() {
    return possibleTiles.size();
}

void Tile::removeType(string str) {
    possibleTiles.erase(str);
}

Vector2i Tile::getCoords() {
    return coords;
}

string Tile::collapseTile() {
    auto it = possibleTiles.begin();

    std::srand(std::time(0));
    int selectedIndex = std::rand() % possibleTiles.size();

    for (int i = 0; i < selectedIndex; i++) {
        it++;
    }

    string selectedType = *it;

    possibleTiles.clear();

    return selectedType;
}