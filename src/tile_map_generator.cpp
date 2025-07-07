#include "tile_map_generator.h"

void TileMapGenerator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("build", "tileDict", "tileAtlas", "mapDimensions", "radius"), &TileMapGenerator::build);
    ClassDB::bind_method(D_METHOD("has_tiles_to_collapse"), &TileMapGenerator::hasTilesToCollapse);
    ClassDB::bind_method(D_METHOD("collapse_tile"), &TileMapGenerator::collapseTile);
}

TileMapGenerator::TileMapGenerator() {
    set_process(false);
}

void TileMapGenerator::build(Dictionary tileDict, int tileAtlas, int mapDimensions, int radius) {
    this->tileAtlas = tileAtlas;
    this->mapDimensions = mapDimensions;
    this->radius = radius;

    Array tiles = tileDict.keys();

    for (int i = 0; i < tiles.size(); i++) {
        Vector2i tileType = tiles[i];
        Array validTypes = tileDict[tileType];

        std::set<Vector2i> valid;

        for (int j = 0; j < validTypes.size(); j++) {
            Vector2i validType = validTypes[j];
            valid.insert(validType);
        }

        typeRules[tileType] = valid;
    }

    createBuckets(typeRules.size());

    for (auto it = typeRules.begin(); it != typeRules.end(); it++) {
        types.insert(it->first);
    }

    for (int x = 0; x < mapDimensions; x++) {
        for (int y = 0; y < mapDimensions; y++) {
            tail->bucket.insert(Tile({x,y}, types));
        }
    }
}

void TileMapGenerator::seed(int chunks, Array types, bool inOrder) {
    
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
    set_cell(coords, tileAtlas, type);

    propagate(coords, type);
    return type;
}

void TileMapGenerator::propagate(const Vector2i & center, const Vector2i & type) {
    for (int x = center.x - radius; x <= center.x + radius; x++) {
        for (int y = center.y - radius; y <= center.y + radius; y++) {
            Vector2i neighbour = Vector2i(x, y);
            if (center != neighbour) {
                updateTile(neighbour, type);
            }
        }
    }
}

void TileMapGenerator::updateTile(const Vector2i & tileCoords, const Vector2i & tileType) {
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

TileMapGenerator::BQNode * TileMapGenerator::findTile(Vector2i tileCoords) {
    for (BQNode * temp = head; temp != nullptr; temp = temp->next) {
        if (temp->bucket.containsTile(tileCoords)) {
            return temp;
        }
    }
    return nullptr;
}