#include "bucket_queue.h"

BucketQueue::BucketQueue(set<string> types, int mapDimensions) {
    createBuckets(types.size());

    for (int x = 0; x < mapDimensions; x++) {
        for (int y = 0; y < mapDimensions; y++) {
            tail->bucket.insert(Tile({x,y}, types));
        }
    }

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

