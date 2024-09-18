#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define MAX_TOPICS 5

// Structure to keep track of subscribers in shared memory
typedef struct {
    int clients[MAX_CLIENTS];
    int count;
} TopicSubscribers;

// Shared memory for storing subscribers
extern int shm_id;
extern TopicSubscribers *subs;

// Array of topic names
char *topic_names[MAX_TOPICS] = {"books", "movies", "music", "sports", "tech"};

void sigchld_handler(int s) {
    // Reap zombie processes
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void add_subscriber_to_topic(int client_fd, const char *topic) {
    for (int i = 0; i < MAX_TOPICS; i++) {
        if (strcmp(topic_names[i], topic) == 0) {
            if (topics[i].count < MAX_CLIENTS) {
                topics[i].clients[topics[i].count++] = client_fd;
                printf("Client %d subscribed to %s\n", client_fd, topic);
            } else {
                printf("Topic %s is full!\n", topic);
            }
            break;
        }
    }
}

void publish_to_topic(const char *topic, const char *message) {
    for (int i = 0; i < MAX_TOPICS; i++) {
        if (strcmp(topic_names[i], topic) == 0) {
            printf("Publishing message to topic %s: %s\n", topic, message);
            for (int j = 0; j < topics[i].count; j++) {
                int client_fd = topics[i].clients[j];
                char notify_message[1024];
                snprintf(notify_message, sizeof(notify_message), "NOTIFY %s %s\n", topic, message);
                send(client_fd, notify_message, strlen(notify_message), 0);
            }
            break;
        }
    }
}

void handle_client(int client_socket) {
    char buffer[1024] = {0};
    
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(client_socket, buffer, 1024);
        if (valread <= 0) {
            printf("Client disconnected\n");
            close(client_socket);
            break;
        }

        printf("Received: %s\n", buffer);

        if (strncmp(buffer, "SUBSCRIBE ", 10) == 0) {
            // Subscribe the client to a topic
            char *topic = buffer + 10;
            topic[strcspn(topic, "\n")] = 0;  // Strip newline if present
            add_subscriber_to_topic(client_socket, topic);
            char *message = "Subscribed successfully!\n";
            send(client_socket, message, strlen(message), 0);
        } else if (strncmp(buffer, "PUBLISH ", 8) == 0) {
            // Publish data to a topic
            char *topic_and_message = buffer + 8;
            char *topic = strtok(topic_and_message, " ");
            char *message = strtok(NULL, "\n");
            if (topic && message) {
                publish_to_topic(topic, message);
            }
        } else {
            char *message = "Unknown command!\n";
            send(client_socket, message, strlen(message), 0);
        }
    }
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Create shared memory for topic subscribers
    shm_id = shmget(IPC_PRIVATE, sizeof(TopicSubscribers) * MAX_TOPICS, IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }
    topics = shmat(shm_id, NULL, 0);
    if (topics == (TopicSubscribers *) -1) {
        perror("shmat");
        exit(1);
    }
    memset(topics, 0, sizeof(TopicSubscribers) * MAX_TOPICS);  // Initialize shared memory

    // Signal handler for child processes (zombie cleanup)
    signal(SIGCHLD, sigchld_handler);
    
    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the specified IP and port
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        return -1;
    }

    printf("Waiting for connections...\n");

    while (1) {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            return -1;
        }

        // Fork a new process to handle the client
        pid_t child_pid = fork();

        if (child_pid == 0) {
            // In the child process, handle the client
            close(server_socket);
            handle_client(client_socket);
            exit(EXIT_SUCCESS);
        } else if (child_pid > 0) {
            // In the parent process, close the client socket and continue listening
            close(client_socket);
        } else {
            perror("Error creating child process");
            exit(EXIT_FAILURE);
        }
    }

    // Detach shared memory before exiting
    shmdt(topics);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
