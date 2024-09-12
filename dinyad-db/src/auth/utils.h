#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "../../lib/cjson/cJSON.h"

#ifndef AUTH_UTILS_H
#define AUTH_UTILS_H

cJSON *find_user(char *username);
int is_credentials_valid(cJSON * auth_json);
#endif