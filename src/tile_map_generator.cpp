#include "tile_map_generator.h"

void TileMapGenerator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("build", "tileDict", "tileAtlas", "mapDimensions", "radius"), &TileMapGenerator::build);
    ClassDB::bind_method(D_METHOD("seed", "chunks", "types", "in_order"), &TileMapGenerator::seed);
    ClassDB::bind_method(D_METHOD("has_tiles_to_collapse"), &TileMapGenerator::hasTilesToCollapse);
    ClassDB::bind_method(D_METHOD("collapse_tile"), &TileMapGenerator::collapseRandomTile);
    ClassDB::bind_method(D_METHOD("refine"), &TileMapGenerator::refine);
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
    if (types.size() == 0) {
        for (Vector2i type : this->types) {
            types.push_back(type);
        }
    }

    int chunkLength = mapDimensions / chunks;
    int index = 0;

    for (int x = 0; x < chunks; x++) {
        for (int y = 0; y < chunks; y++) {
            int chunkx = RandomGenerator::getInt(
                x * chunkLength,
                std::min((x + 1) * chunkLength - 1, mapDimensions - 1));
            int chunky = RandomGenerator::getInt(
                y * chunkLength,
                std::min((y + 1) * chunkLength - 1, mapDimensions - 1));
            
            Vector2i tile(chunkx, chunky);

            if (inOrder) {
                collapseTile(tile, types[index % types.size()]);
                index++;
            } else {
                collapseTile(tile, types[RandomGenerator::getInt(0, types.size() - 1)]);
            }
        }
    }
}

void TileMapGenerator::refine() {
    for (int x = 0; x < mapDimensions; x++) {
        for (int y = 0; y < mapDimensions; y++) {
            Vector2i upType = get_cell_atlas_coords(Vector2i(x, y-1));
            Vector2i downType = get_cell_atlas_coords(Vector2i(x, y+1));
            Vector2i leftType = get_cell_atlas_coords(Vector2i(x-1, y));
            Vector2i rightType = get_cell_atlas_coords(Vector2i(x+1, y));

            Vector2i types[4] = {upType, downType, leftType, rightType};

            Vector2i currentType;

            for (int i = 0; i < 4; i++) {
                if (types[i] != Vector2i(-1,-1)) {
                    currentType = types[i];
                    break;
                }
            }

            bool same = true;

            for (int i = 0; i < 4; i++) {
                if (types[i] != Vector2i(-1,-1) && types[i] != currentType) {
                    same = false;
                    break;
                }
            }

            if (same) {
                set_cell(Vector2i(x, y), tileAtlas, currentType);
            }
        }
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

void TileMapGenerator::collapseTile(const Vector2i & givenCoords, const Vector2i & givenType) {
    BQNode * temp = findTile(givenCoords);
    temp->bucket.removeTile(givenCoords);
    Vector2i coords = givenCoords;
    Vector2i type = givenType;

    set_cell(coords, tileAtlas, type);

    propagate(coords, type);
}

void TileMapGenerator::collapseRandomTile() {
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

    set_cell(coords, tileAtlas, type);

    propagate(coords, type);
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
    BQNode * tileLoc = findTile(tileCoords);

    if (tileLoc != nullptr) {
        Tile tile = tileLoc->bucket.removeTile(tileCoords);
        tile.removeTypesNotIn(typeRules[tileType]);
        int priority = tile.getPriority();

        // find and insert into the right bucket
        BQNode * temp2 = head;
        for (int i = 0; i < priority - 1; i++) {
            temp2 = temp2->next;
        }
        temp2->bucket.insert(tile);
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

