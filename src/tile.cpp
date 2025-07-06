#include "tile.h"

Tile::Tile(const Vector2i & coords, const set<Vector2i> & possibleTiles) : coords(coords), possibleTiles(possibleTiles) {

}

int Tile::getPriority() const {
    return possibleTiles.size();
}

void Tile::removeType(const Vector2i & str) {
    possibleTiles.erase(str);
}

Vector2i Tile::getCoords() const {
    return coords;
}

Vector2i Tile::collapseTile(){
    if (possibleTiles.empty()) {
        return Vector2i(0, 0);
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, possibleTiles.size() - 1);

    int selectedIndex = dist(gen);

    auto it = possibleTiles.begin();

    for (int i = 0; i < selectedIndex; i++) {
        it++;
    }

    Vector2i selectedType = *it;

    possibleTiles.clear();

    return selectedType;
}