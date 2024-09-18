#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "../../lib/cjson/cJSON.h"

#ifndef REALTIME_H
#define REALTIME_H
#define MAX_CLIENTS 10
#define MAX_CHANGE_SUB 5

#ifndef MESSAGE_MAX_L
#define MESSAGE_MAX_L 100000
#endif

// Structure to keep track of subscribers in shared memory

typedef struct
{
    char doc[50];
    int clients[MAX_CLIENTS];
    int count;
} ChangeSubscriber;

typedef struct
{
    ChangeSubscriber list[MAX_CHANGE_SUB];
    int count;
} ChangeSubscribersList;

// Shared memory for storing subscribers

extern ChangeSubscribersList *subs;

int init_subs();
int find_doc_sub(const char *doc);
int add_change_subscriber(int client_socket, const char *doc);
char *create_notification(const char *doc,  cJSON *data);
int notify(const char *doc, cJSON *data);

#endif