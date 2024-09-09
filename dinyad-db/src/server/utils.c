#include "utils.h"
#include "../store/store.h"
#include "../helpers/helpers.h"

char *handle_cmd(cJSON *req_json)
{
    char *cmd_string = get_string(req_json, "cmd");
    char *db = get_string(req_json, "db");
    Cmd *cmd = create_cmd(cmd_string);
    if (cmd->argc != 0)
    {
        if (!strcmp(cmd->argv[0], "dbs"))
        {
            return make_res(get_dbs_list(), 0, 1);
        }
        else if (!strcmp(cmd->argv[0], "collections"))
        {

            if (cmd->argc == 3 && !strcmp(cmd->argv[1], "--db"))
            {

                return make_res(get_collection_list(cmd->argv[2]), 0, 1);
            }
            else if (cmd->argc == 1)
            {
                return make_res(get_collection_list(db), 0, 1);
            }

            return make_res("bad cmd format", 1, 1);
        }
        else if (!strcmp(cmd->argv[0], "createdb"))
        {
            if (cmd->argc == 2)
            {
                create_db(cmd->argv[1]);
                return make_res("DB created successfully!", 0, 1);
            }
            else
            {
                return make_res("bad cmd format", 1, 1);
            }
        }
        else
        {
            return make_res("unkown cmd", 1, 1);
        }
    }

    return make_res("empy cmd", 1, 1);
}
char *handle_request(char *req)
{
    cJSON *req_json = cJSON_Parse(req);

    char *type = get_string(req_json, "type");

    if (isCMD(type))
    {
        return handle_cmd(req_json);
    }

    char *rsrc = get_string(req_json, "collection");
    char *method = get_string(req_json, "verb");
    char *db = get_string(req_json, "db");

    if (isGET(method))
    {
        printf("call isGET\n");

        if (cJSON_HasObjectItem(req_json, "where"))
        {
            cJSON *filter = cJSON_GetObjectItemCaseSensitive(req_json, "where");
            cJSON *single = cJSON_GetObjectItemCaseSensitive(req_json, "single");
            cJSON *options = cJSON_GetObjectItemCaseSensitive(req_json, "options");
            printf("call find\n");
            return make_res(find(db, rsrc, single, filter, options), 0, 0);
        }
        else
        {
            cJSON *options = cJSON_GetObjectItemCaseSensitive(req_json, "options");
            return make_res(get_all(db, rsrc, options), 0, 0);
        }
    }

    else if (isPOST(method))
    {
        cJSON *data = cJSON_GetObjectItemCaseSensitive(req_json, "data");
        if (data == NULL)
        {
            return make_res("body data is required", 1, 1);
        }
        return make_res(insert_one(db, rsrc, data), 0, 0);
    }
    else if (isDELETE(method))
    {
        if (cJSON_HasObjectItem(req_json, "where"))
        {
            cJSON *filter = cJSON_GetObjectItemCaseSensitive(req_json, "where");
            cJSON *single = cJSON_GetObjectItemCaseSensitive(req_json, "single");
            return make_res(delete (db, rsrc, single, filter), 0, 0);
        }
        else
        {
            return make_res("bad request, trying to delete entire collection", 1, 1);
        }
    }
    else if (isPUT(method))
    {
        if (cJSON_HasObjectItem(req_json, "where"))
        {
            cJSON *filter = cJSON_GetObjectItemCaseSensitive(req_json, "where");
            cJSON *single = cJSON_GetObjectItemCaseSensitive(req_json, "single");
            cJSON *data = cJSON_GetObjectItemCaseSensitive(req_json, "data");
            return make_res(update(db, rsrc, single, filter, data), 0, 0);
        }
        else
        {
            return make_res("bad request, trying to update entire collection", 1, 1);
        }
    }
    else
    {
        return make_res("bad request", 1, 1);
    }
}

void handle_client(int client_socket)
{

    char req[100000];

    while (1)
    {
        // Réception de la req du client
        ssize_t recv_size = recv(client_socket, req, sizeof(req), 0);
        if (recv_size <= 0)
        {
            perror("Erreur lors de la réception de la req du client");
            break;
        }
        req[recv_size] = '\0';

        printf("req : %s\n", req);
        printf("call handle_request\n");
        char *res = handle_request(req);
        send(client_socket, res, strlen(res), 0);
    }

    // Fermeture du socket client
    close(client_socket);
}

char *get_dbs_list()
{
    return listDirectories(DB_DIR);
}

char *get_collection_list(char *db_name)
{
    char *path = get_db_path(db_name);
    if (fileExists(path))
    {
        return listDirectories(path);
    }
    else
    {
        char *s = malloc(sizeof(char) * 100);
        sprintf(s, "db %s does not exist !", db_name);
        return s;
    }
}

char *make_res(char *res_str, int error, int string)
{
    cJSON *resjs = cJSON_CreateObject();
    cJSON_AddStringToObject(resjs, "type", error ? "error" : string ? "str"
                                                                    : "ok");

    if (error)
    {
        cJSON_AddStringToObject(resjs, "message", res_str);
    }
    else
    {

        char *s = malloc(sizeof(char) * 200);
        if (string)
        {
            sprintf(s, "\"%s\"", res_str);
        }
        cJSON_AddItemToObject(resjs, "data", cJSON_Parse(string ? s : res_str));
        free(s); // not here
    }
    return cJSON_Print(resjs);
}

Config *load_config(char *conf_path)
{

    Config *config = malloc(sizeof(Config));

    FILE *file = fopen(conf_path, "r");
    if (file == NULL)
    {
        perror("Error opening config file");
        return NULL;
    }

    char line[255];

    while (fgets(line, sizeof(line), file))
    {
        char *trimmed_line = strtok(line, " \t\n"); // Trim leading whitespace

        if (trimmed_line == NULL || trimmed_line[0] == '#')
        {
            // Skip empty lines and comments
            continue;
        }
        else
        {
            // This line is a key-value pair
            char *key = strtok(trimmed_line, "=");
            char *value = strtok(NULL, "=");

            if (strcmp(key, "ip") == 0)
            {
                strcpy(config->ip, value);
            }
            else if (strcmp(key, "port") == 0)
            {
                config->port = atoi(value);
            }
            else if (strcmp(key, "db_dir") == 0)
            {
                strcpy(config->db_dir, value);
            }
            else if (strcmp(key, "log_dir") == 0)
            {
                strcpy(config->log_dir, value);
            }
            else if (strcmp(key, "enabled_auth") == 0)
            {
                strcpy(config->enabled_auth, value);
            }
        }
    }

    fclose(file);
    return config;
}
