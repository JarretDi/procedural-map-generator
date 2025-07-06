#include "tile_map_generator.h"

TileMapGenerator::TileMapGenerator(unordered_map<Vector2i, std::set<Vector2i>> & typeRules, int mapDimensions, int radius) {
    createBuckets(typeRules.size());
    radius = radius;

    for (auto it = typeRules.begin(); it != typeRules.end(); it++) {
        types.insert(it->first);
    }

    for (int x = 0; x < mapDimensions; x++) {
        for (int y = 0; y < mapDimensions; y++) {
            tail->bucket.insert(Tile({x,y}, types));
        }
    }

    while (hasTilesToCollapse()) {
        collapseTile();
    }
}

TileMapGenerator::~TileMapGenerator() {
    BQNode * temp = head;

    while (temp != nullptr) {
        BQNode * next = temp->next;
        delete temp;
        temp = next;
    }

    head = nullptr;
    tail = nullptr;
}

void TileMapGenerator::createBuckets(int size) {
    head = new BQNode();
    BQNode * temp = head;

    for (int i = 0; i < size; i++) {
        BQNode * next = new BQNode();
        temp->next = next;
        next->prev = temp;
        temp = next;
    }

    tail = temp;
}

bool TileMapGenerator::hasTilesToCollapse() {
    BQNode * temp = head;

    while (temp != nullptr) {
        if (!temp->bucket.isEmpty()) {
            return true;
        } else {
            temp = temp->next;
        }
    }
    return false;
}

Vector2i TileMapGenerator::collapseTile() {
    BQNode * temp = head;

    while (temp != nullptr) {
        if (!temp->bucket.isEmpty()) {
            break;
        } else {
            temp = temp->next;
        }
    }

    Tile tile = temp->bucket.removeRandom();
    Vector2i coords = tile.getCoords();
    Vector2i type = tile.collapseTile();

    //map[coords.x][coords.y] = type;
    propogate(coords, type);
    return type;
}

void TileMapGenerator::propogate(Vector2i center, Vector2i type) {
    for (int x = center.x - radius; x <= center.x + radius; x++) {
        for (int y = center.y - radius; y <= center.y + radius; y++) {
            updateTile(center, type);
        }
    }
}

void TileMapGenerator::updateTile(Vector2i tileCoords, Vector2i tileType) {
    std::set<Vector2i> valid = typeRules[tileType];

    std::set<Vector2i> toRemove;

    for (Vector2i type : types) {
        //i.e. if it isnt in valid
        if (valid.find(type) == valid.end()) {
            toRemove.insert(type);
        }
    }

    for (BQNode * temp = head; temp != nullptr; temp = temp->next) {
        try {
            for (Vector2i type : toRemove) {
                temp->bucket.updateTile(tileCoords, type);
            }

            //if we get here, we haven't caught out_of_range
            Tile tile = temp->bucket.removeTile(tileCoords);
            int priority = tile.getPriority();

            // find and insert into the right bucket
            BQNode * temp2 = head;
            for (int i = 0; i < priority - 1; i++) {
                temp2 = temp2->next;
            }
            temp2->bucket.insert(tile);
            return;

        } catch (const std::out_of_range& e) {
            continue;
        }
    }
}