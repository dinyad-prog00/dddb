#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// Define a message structure for events
struct EventMessage {
    long messageType;
    char messageData[256];  // You can adjust the size based on your needs
};

// Global message queue ID
int msgQueueID;

// Function to handle client subscriptions
void* clientHandler(void* clientID) {
    int clientId = *((int*)clientID);
    struct EventMessage event;
    event.messageType = clientId;  // Use client ID as the message type

    while (1) {
        // Simulate some database events (e.g., new data inserted)
        sprintf(event.messageData, "New data received for client %d", clientId);

        // Send the event to the message queue
        if (msgsnd(msgQueueID, &event, sizeof(event.messageData), 0) == -1) {
            perror("msgsnd");
        }

        // Sleep for a while to simulate real-time updates
        sleep(3);
    }
}

// Function to handle message processing for clients
void* messageProcessor(void* arg) {
    while (1) {
        struct EventMessage event;

        // Receive events from the message queue
        if (msgrcv(msgQueueID, &event, sizeof(event.messageData), 0, 0) == -1) {
            perror("msgrcv");
            break;
        }

        // Process the received event (e.g., send it to connected clients)
        printf("Received: %s\n", event.messageData);
    }
}

int main() {
    // Create a message queue
    key_t key = ftok("/tmp", 'A');
    msgQueueID = msgget(key, 0666 | IPC_CREAT);
    if (msgQueueID == -1) {
        perror("msgget");
        exit(1);
    }

    // Create a thread for message processing
    pthread_t messageThread;
    if (pthread_create(&messageThread, NULL, messageProcessor, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }

    // Create threads for multiple clients (simulated)
    pthread_t clientThreads[3];
    int clientIDs[3] = {1, 2, 3};  // Simulated client IDs

    for (int i = 0; i < 3; i++) {
        if (pthread_create(&clientThreads[i], NULL, clientHandler, &clientIDs[i]) != 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    // Wait for client threads to finish (which won't happen in this example)
    for (int i = 0; i < 3; i++) {
        pthread_join(clientThreads[i], NULL);
    }

    // Clean up (this code will not be reached)
    pthread_join(messageThread, NULL);
    msgctl(msgQueueID, IPC_RMID, NULL);

    return 0;
}
