#include "tile_map_generator.h"

void TileMapGenerator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("build", "tileDict", "tileAtlas", "mapDimensions", "radius"), &TileMapGenerator::build);
    ClassDB::bind_method(D_METHOD("seed", "chunks", "types", "in_order"), &TileMapGenerator::seed);
    ClassDB::bind_method(D_METHOD("has_tiles_to_collapse"), &TileMapGenerator::hasTilesToCollapse);
    ClassDB::bind_method(D_METHOD("collapse_tile"), &TileMapGenerator::collapseRandomTile);
    ClassDB::bind_method(D_METHOD("refine"), &TileMapGenerator::refine);
    ClassDB::bind_method(D_METHOD("parse_rules", "sample", "size"), &TileMapGenerator::parseRules);
}

TileMapGenerator::TileMapGenerator() {
    set_process(false);
}

void TileMapGenerator::build(Dictionary tileDict, int tileAtlas, int mapDimensions) {
    this->tileAtlas = tileAtlas;
    this->mapDimensions = mapDimensions;

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
            Vector2i currentType = get_cell_atlas_coords({x, y});

            Vector2i upType = get_cell_atlas_coords({x, y-1});
            Vector2i downType = get_cell_atlas_coords({x, y+1});
            Vector2i leftType = get_cell_atlas_coords({x-1, y});
            Vector2i rightType = get_cell_atlas_coords({x+1, y});

            Vector2i types[4] = {upType, downType, leftType, rightType};

            unordered_map<Vector2i, int> count;

            for (Vector2i type : types) {
                if (count.find(type) == count.end()) count[type] = 0;
                count[type]++;
            }

            Vector2i dominantType = Vector2i(-1, -1);
            int maxSoFar = 0;

            for (auto [type, freq] : count) {
                if (freq > maxSoFar && type != Vector2i(-1, -1)) {
                    dominantType = type;
                    maxSoFar = freq; 
                }
            }

            // if has only one neighbor of same type, dies
            // OR if surrounded by 3 or more of another type, switches
            if (count[currentType] <= 1 || maxSoFar >= 3) {
                set_cell({x,y}, tileAtlas, dominantType);
            }
        }
    }
}

bool TileMapGenerator::hasTilesToCollapse() {
    for (int i = 0; i < buckets.size(); i++) {
        if (!buckets[i].isEmpty()) {
            return false;
        }
    }
    return true;
}

void TileMapGenerator::collapseTile(const Vector2i & coords, int typeIdx) {
    int loc = findTile(coords);
    buckets[loc].removeTile(coords);

    set_cell(coords, tileAtlas, intToType[typeIdx]);

    propagate(coords, typeIdx);
}

void TileMapGenerator::collapseRandomTile() {
    int i = 0;
    while (i < buckets.size()) {
        if (!buckets[i].isEmpty()) {
            break;
        } else {
            i++;
        }
    }

    Tile tile = buckets[i].removeRandom();
    Vector2i coords = tile.getCoords();
    int typeIdx = tile.collapseTile();

    set_cell(coords, tileAtlas, intToType[typeIdx]);

    propagate(coords, typeIdx);
}

void TileMapGenerator::propagate(const Vector2i & center, int typeIdx) {
    Vector2i top = center + Vector2i(0, -1);
    updateTile(top, typeRules[typeIdx][Direction::UP]);

    Vector2i right = center + Vector2i(1, 0);
    updateTile(right, typeRules[typeIdx][Direction::RIGHT]);

    Vector2i down = center + Vector2i(0, 1);
    updateTile(down, typeRules[typeIdx][Direction::DOWN]);

    Vector2i left = center + Vector2i(-1, 0);
    updateTile(left, typeRules[typeIdx][Direction::LEFT]);
}

void TileMapGenerator::updateTile(const Vector2i & tileCoords, const bitset<32> & rules) {
    int tileLoc = findTile(tileCoords);

    if (tileLoc != -1) {
        Tile tile = buckets[tileLoc].removeTile(tileCoords);
        tile.removeTypesNotIn(rules);
        int priority = tile.getPriority();

        buckets[priority].insert(tile);
    }
}

int TileMapGenerator::findTile(Vector2i tileCoords) {
    for (int i = 0; i < buckets.size(); i++) {
        if (buckets[i].containsTile(tileCoords)) {
            return i;
        }
    }
    return -1;
}

Dictionary TileMapGenerator::parseRules(TileMapLayer * sample, int size) {
    Dictionary dict;

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            Vector2i type = sample->get_cell_atlas_coords({x,y});
            Array neighbours = sample->get_surrounding_cells({x, y});
            for (int i = 0; i < neighbours.size(); i++) {
                Vector2i neighbourType = sample->get_cell_atlas_coords(neighbours[i]);
                if (neighbourType != Vector2i(-1, -1)) {
                    Array arr = dict.get(type, Array());
                    arr.push_back(neighbourType);
                    dict[type] = arr;
                }
            }
        }
    }

    return dict;
}

