#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "../../lib/cjson/cJSON.h"

#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#define SERVER_PORT 3331

#define isGET(method) strcmp(method, "GET") == 0
#define isPOST(method) strcmp(method, "POST") == 0
#define isPUT(method) strcmp(method, "PUT") == 0
#define isDELETE(method) strcmp(method, "DELETE") == 0
#define isCMD(type) strcmp(type, "cmd") == 0

typedef struct
{
    char ip[14];
    int port;
    char db_dir[50];
    char log_dir[50];
    char enabled_auth[12];
} Config ;

void handle_client(int client_socket);
char *handle_request(char *req);
char *handle_cmd(cJSON *req_json);
char *get_dbs_list();
char *get_collection_list(char *db_name);
char *make_res(char *res_str, int error, int string);
Config * load_config(char * conf_path);

int server(Config * config);

#endif