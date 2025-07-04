#include "bucket_queue.h"

BucketQueue::BucketQueue(unordered_map<string, set<string>> & typeRules, int mapDimensions, int radius) {
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

    map = vector(mapDimensions, vector<string>(mapDimensions, ""));

    while (hasTilesToCollapse()) {
        collapseTile();
    }
}

BucketQueue::~BucketQueue() {
    BQNode * temp = head;

    while (temp != nullptr) {
        BQNode * next = temp->next;
        delete temp;
        temp = next;
    }

    head = nullptr;
    tail = nullptr;
}

void BucketQueue::createBuckets(int size) {
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

bool BucketQueue::hasTilesToCollapse() {
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

string BucketQueue::collapseTile() {
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
    string type = tile.collapseTile();

    map[coords.x][coords.y] = type;
    propogate(coords, type);
    return type;
}

void BucketQueue::propogate(Vector2i center, string type) {
    for (int x = center.x - radius; x <= center.x + radius; x++) {
        for (int y = center.y - radius; y <= center.y + radius; y++) {
            updateTile(center, type);
        }
    }
}

void BucketQueue::updateTile(Vector2i tileCoords, string tileType) {
    set<string> valid = typeRules[tileType];

    set<string> toRemove;

    for (string type : types) {
        //i.e. if it isnt in valid
        if (valid.find(type) == valid.end()) {
            toRemove.insert(type);
        }
    }

    for (BQNode * temp = head; temp != nullptr; temp = temp->next) {
        try {
            for (string type : toRemove) {
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