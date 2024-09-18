#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include "../../lib/cjson/cJSON.h"

#ifndef STORE_H
#define STORE_H

#define DB_DIR "dddb.dbs"

#define SINGLE_REF 0
#define MANY_REF 1
typedef struct
{
    int type;
    char name[50];
    char ref[50];
} RefProps;

char *get_db_path(char db_name[]);
char *get_collection_path(char db_name[], char collection_name[]);

int fileExists(const char *fileName);

int create_db(char name[]);
int create_collection(char db_name[], char collection_name[]);

cJSON *get_collection(char db_name[], char collection_name[]);
int store_collection(cJSON *collection, char *path);

char *generate_uid();
char *insert_one(char db_name[], char collection_name[], cJSON *data);

cJSON_bool check_attr_value(cJSON *item, char *attr, cJSON *value, cJSON_bool case_sensitive);
cJSON_bool pass_filter(cJSON *item, cJSON *filter);
char *find(char db_name[], char collection_name[], cJSON *single, cJSON *filter, cJSON *options);

cJSON *find_by_unique_key(char db_name[], char collection_name[], char *key_name, cJSON *key_value);
char *find_by_id(char db_name[], char collection_name[], cJSON *id);

char *get_all(char db_name[], char collection_name[], cJSON *options);
char *delete(char db_name[], char collection_name[], cJSON *single, cJSON *filter);
char *update(char db_name[], char collection_name[], cJSON *single, cJSON *filter, cJSON *data);
RefProps *make_ref_props(char *attr);
void make_reference(char db_name[], cJSON *item, char *attr, RefProps *prop);
void make_all_references(char db_name[], cJSON *item, cJSON *populate);
// void clear_ref_attr(cJSON * item);
int in_array(cJSON *array, cJSON *item);

#endif