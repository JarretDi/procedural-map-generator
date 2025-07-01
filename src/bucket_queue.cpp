#include "bucket_queue.h"

BucketQueue::BucketQueue(unordered_map<string, set<string>> & typeRules, int mapDimensions, int radius = 1) {
    createBuckets(typeRules.size());
    radius = radius;

    for (int x = 0; x < mapDimensions; x++) {
        for (int y = 0; y < mapDimensions; y++) {
            set<string> types;

            for (auto it = typeRules.begin(); it != typeRules.end(); it++) {
                types.insert(it->first);
            }

            tail->bucket.insert(Tile({x,y}, types));
        }
    }

    map = vector(mapDimensions, vector<string>(mapDimensions, ""));

    while (hasTilesToCollapse()) {
        collapseTile();
    }
}

void BucketQueue::createBuckets(int size) {
    head = new BQNode();
    BQNode * temp = head;

    for (int i = 0; i < size - 1; i++) {
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

    //note be careful of bucket pointer vs copy
    Tile * tile = temp->bucket.removeRandom();
    pair<int, int> coords = tile->getCoords();
    string type = tile->collapseTile();
    delete tile;

    map[coords.first][coords.second] = type;
    propogate(coords, type);
}

void BucketQueue::propogate(pair<int, int> center, string type) {
    for (int x = center.first - radius; x <= center.first + radius; x++) {
        for (int y = center.second - radius; y <= center.second + radius; y++) {
            updateTile(center, type);
        }
    }
}