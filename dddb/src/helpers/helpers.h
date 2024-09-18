#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../../lib/cjson/cJSON.h"
#ifndef HELPERS_H
#define HELPERS_H
typedef struct
{
    int argc;
    char *argv[10];
} Cmd;

char *get_string(cJSON *json, char *key);
char *listDirectories(const char *directory_path);
char *removeExtension(char *str);
void cleanStr(char *str);
char *apply_newline(char *str);
Cmd *create_cmd(char *cmd_string);
#endif