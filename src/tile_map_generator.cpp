#include "tile_map_generator.h"

void TileMapGenerator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("build", "tileDict", "tileAtlas", "mapDimensions"), &TileMapGenerator::build);
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

    parseDictionary(tileDict);

    int typeNum = idxToType.size();
    buckets.resize(typeNum);

    for (int x = 0; x < mapDimensions; x++) {
        for (int y = 0; y < mapDimensions; y++) {
            buckets[buckets.size() - 1].insert(Tile({x,y}, typeNum));
        }
    }
}

void TileMapGenerator::parseDictionary(const Dictionary & tileDict) {
    unordered_map<Vector2i, int> tempMap;
    TypedArray<Vector2i> tileTypes = tileDict.keys();
    int typeAmount = tileTypes.size();

    typeRules.resize(typeAmount);
    idxToType.resize(typeAmount);

    // First, assign each type to a unique index
    for (int i = 0; i < typeAmount; i++) {
        Vector2i type = tileTypes[i];
        idxToType[i] = type;
        tempMap[type] = i;
    }

    // Then, go through a second pass and set bitsets for each type and direction
    for (int i = 0; i < typeAmount; i++) {
        TypedArray<Array> tileType = tileDict[tileTypes[i]];
        for (int j = Direction::UP; j <= Direction::LEFT; j++) {
            TypedArray<Vector2i> lateralRules = tileType[j];
            for (int k = 0; k < lateralRules.size(); k++) {
                Vector2i rule = lateralRules[k];
                int index = tempMap.at(rule);
                typeRules[i][j].set(index);
            }
        }
    }
}

void TileMapGenerator::seed(int chunks, Array types, bool inOrder) {
    if (types.size() == 0) {
        for (Vector2i type : this->idxToType) {
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
            return true;
        }
    }
    return false;
}

void TileMapGenerator::collapseTile(const Vector2i & coords, int typeIdx) {
    int loc = findTile(coords);
    buckets[loc].removeTile(coords);

    set_cell(coords, tileAtlas, idxToType[typeIdx]);

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

    set_cell(coords, tileAtlas, idxToType[typeIdx]);

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

bool TileMapGenerator::updateTile(const Vector2i & tileCoords, const bitset<TILE_TYPE_COUNT> & rules) {
    if (tileCoords.x < 0 || tileCoords.y < 0 || tileCoords.x >= mapDimensions || tileCoords.y >= mapDimensions) return false;
    int tileLoc = findTile(tileCoords);

    if (tileLoc == -1) return false;
    
    Tile tile = buckets[tileLoc].removeTile(tileCoords);
    bool changed = tile.removeTypesNotIn(rules);
    int priority = tile.getPriority();

    buckets[priority].insert(tile);
    return changed;    
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
            
            array<Vector2i, 4> neighbourTypes = {
                sample->get_cell_atlas_coords({x, y - 1}),
                sample->get_cell_atlas_coords({x + 1, y}),
                sample->get_cell_atlas_coords({x, y + 1}),
                sample->get_cell_atlas_coords({x - 1, y})
            };

            Array typeEntry;

            if (!dict.has(type)) {
                typeEntry.resize(4);
            } else {
                typeEntry = dict[type];
            }

            for (int i = Direction::UP; i <= Direction::LEFT; i++) {
                Array latType = typeEntry[i];
                Vector2i neighbourType = neighbourTypes[i];

                if (neighbourType != Vector2i(-1, -1) && !latType.has(neighbourType)) {
                    latType.push_back(neighbourType);
                }

                typeEntry[i] = latType;
            }
            dict[type] = typeEntry;
        }
    }

    return dict;
}