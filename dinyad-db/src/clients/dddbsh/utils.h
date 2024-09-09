#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "../../../lib/cjson/cJSON.h"
#include "../../helpers/helpers.h"

#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#define SERVER_PORT 33301
#define MAX_LENGTH 10000



typedef struct
{
    char collection[256];
    char method[256];
    char _id[30];
    char filter[512];
    char data[512];
} QueryInfo;

typedef struct
{
    char collection[100];
    char method[100];
    char _id[24];
    char data[1000];
    char verb[10];
} req_type;

QueryInfo *extract_infos(char *query);
req_type *extract_infos2(char *req_string);
int is_cmd(char *str);
char *trim(char *str);
char *custom_completion(const char *text, int state);

#endif