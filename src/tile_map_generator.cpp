#include "tile_map_generator.h"

void TileMapGenerator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("build", "tileDict", "tileAtlas", "mapDimensions"), &TileMapGenerator::build);
    ClassDB::bind_method(D_METHOD("has_tiles_to_collapse"), &TileMapGenerator::hasTilesToCollapse);
    ClassDB::bind_method(D_METHOD("collapse_tile"), &TileMapGenerator::collapseRandomTile);
    ClassDB::bind_method(D_METHOD("parse_rules", "sample", "size"), &TileMapGenerator::parseRules);
}

TileMapGenerator::TileMapGenerator() {
    set_process(false);
}

void TileMapGenerator::build(Dictionary tileDict, Dictionary freqMap, int tileAtlas, int mapDimensions) {
    this->tileAtlas = tileAtlas;
    this->mapDimensions = mapDimensions;

    parseDictionary(tileDict, freqMap);

    int typeNum = idxToType.size();
    buckets.resize(typeNum + 1);

    for (int x = 0; x < mapDimensions; x++) {
        for (int y = 0; y < mapDimensions; y++) {
            buckets[buckets.size() - 1].insert(Tile({x,y}, typeNum));
        }
    }
}

void TileMapGenerator::parseDictionary(const Dictionary & tileDict, const Dictionary & freqMap) {
    unordered_map<Vector2i, int> tempMap;
    TypedArray<Vector2i> tileTypes = tileDict.keys();
    int typeAmount = tileTypes.size();

    typeRules.resize(typeAmount);
    idxToType.resize(typeAmount);
    tileFreq.resize(typeAmount);

    // First, assign each type to a unique index
    // additionally, set up the freq map
    for (int i = 0; i < typeAmount; i++) {
        Vector2i type = tileTypes[i];
        idxToType[i] = type;
        tempMap[type] = i;

        tileFreq[i] = freqMap[type];
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
    //std::cerr << "Set cell at (" << coords.x << ", " << coords.y << ") to "<< typeIdx << "\n";

    propagate(coords, typeIdx);
}

void TileMapGenerator::propagate(Vector2i collapsedCoords, int typeIdx) {
    queue<Vector2i> toVisit;

    // initialize the queue by pushing neighbours first
    for (int dir = Direction::UP; dir <= Direction::LEFT; dir++) {
        Vector2i neighbourCoords = collapsedCoords + CARDINAL_VECTORS[dir];
        if (neighbourCoords.x < 0 || neighbourCoords.y < 0 || neighbourCoords.x >= mapDimensions || neighbourCoords.y >= mapDimensions) {
            continue;
        }

        bitset<TILE_TYPE_COUNT> valid = typeRules[typeIdx][dir];

        //std::cerr << "Propogating to (" << neighbourCoords.x << ", " << neighbourCoords.y << ") with types " << valid.to_string().c_str() << "\n";

        bool changed = updateTile(neighbourCoords, valid);

        if (changed) toVisit.push(neighbourCoords);
    }

    while (!toVisit.empty()) {
        Vector2i currentCoords = toVisit.front();
        toVisit.pop();

        int bucket = findTile(currentCoords); 
        if (bucket == -1) {
            continue;
        }

        const Tile & currentTile = buckets[bucket].getTile(currentCoords);

        bitset<TILE_TYPE_COUNT> currentPossibilities = currentTile.getPossibleTiles();

        for (int dir = Direction::UP; dir <= Direction::LEFT; dir++) {
            Vector2i newTile = currentCoords + CARDINAL_VECTORS[dir];
            bitset<TILE_TYPE_COUNT> toUpdate;

            // we need to find the union of possible states in a direction
            for (int i = 0; i < currentPossibilities.size(); i++) {
                if (currentPossibilities[i]) {
                    // we have landed on a possible index
                    bitset<TILE_TYPE_COUNT> rules = typeRules[i][dir];
                    // or because we want each possible option
                    // for all current possible states for this tile
                    toUpdate |= rules;
                }
            }

            bool changed = updateTile(newTile, toUpdate);
            if (changed) toVisit.push(newTile);
        }
    }
}

bool TileMapGenerator::updateTile(const Vector2i & tileCoords, const bitset<TILE_TYPE_COUNT> & rules) {
    if (tileCoords.x < 0 || tileCoords.y < 0 || tileCoords.x >= mapDimensions || tileCoords.y >= mapDimensions) return false;
    int tileLoc = findTile(tileCoords);

    if (tileLoc == -1) return false;
    
    Tile tile = buckets[tileLoc].removeTile(tileCoords);

    //std::cerr << "Updating tile at (" << tile.getCoords().x << ", " << tile.getCoords().y << ") with types " << tile.getPossibleTiles().to_string().c_str() << "\n";

    bool changed = tile.removeTypesNotIn(rules);
    int priority = tile.getPriority();

    //std::cerr << "Tile has priority " << priority << ", and changed is " << changed << "\n";
    //std::cerr << "Tile has been updated with new type:" << tile.getPossibleTiles().to_string().c_str() << "\n";

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

                if (neighbourType != NULL_VEC && !latType.has(neighbourType)) {
                    latType.push_back(neighbourType);
                }

                typeEntry[i] = latType;
            }
            dict[type] = typeEntry;
        }
    }

    return dict;
}

Dictionary TileMapGenerator::parseFrequency(TileMapLayer * sample, int size) {
    Dictionary dict;

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            Vector2i type = sample->get_cell_atlas_coords({x,y});
            if (type == Vector2i(-1, -1)) continue;
            
            if (!dict.has(type)) {
                dict[type] = 0;
            }

            int newVal = dict[type];
            newVal++;
            dict[type] = newVal;
        }
    }

    return dict;
}