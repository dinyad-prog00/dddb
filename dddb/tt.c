#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    uint32_t timestamp;
    uint16_t processId;
    uint32_t counter;
} ObjectId;

void generateObjectId(ObjectId *id) {
    id->timestamp = (uint32_t)time(NULL);
    id->processId = (uint16_t)getpid();
    id->counter = rand() % 0xFFFFFF; // Utilisation d'un compteur aléatoire
}

int main() {
    ObjectId id;
    generateObjectId(&id);

    printf("Generated ObjectId: %08x%04x%08x\n %d", id.timestamp, id.processId, id.counter,id.processId);

    return 0;
}