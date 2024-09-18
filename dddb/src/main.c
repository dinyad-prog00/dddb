#include <stdio.h>
#include "store/store.h"
#include "server/utils.h"

int main(int argc, char const *argv[])
{

    Config *config = load_config("dddb.conf");
    if (config != NULL)
    {
        //printf("%s, %d, %s, %s, %s\n", config->ip, config->port, config->db_dir, config->log_dir, config->enabled_auth);
        server(config);
    }
    // char *res = handle_request("{\"resource\" : \"tasks\",\"method\":\"GET\" ,\"_id\" : \"650ddc4fd400008b45d2\"}");
    // char *res = handle_request("{\"resource\" : \"tasks\",\"method\":\"GET\"}");

    // char *res = handle_request("{\"resource\" : \"tasks\",\"method\":\"POST\", \"data\": {\"nm\":true}}");

    // printf("%s\n", res);

    // char * task = find_by_id("test","tasks","650ddc4fd400008b45d2");

    // printf("%s",task);
    // insert_one("test","tasks","{\"name\":\"Go home\",\"done\": false}");
    // get_collection("test", "tasks");
    //  create_collection("test","tasks");
    //   create_db("test");

    // cJSON * collection = cJSON_CreateArray();
    // cJSON *data = cJSON_CreateObject();
    // cJSON_AddStringToObject(data, "name", "John");
    // cJSON_AddNumberToObject(data, "age", 30);

    // cJSON_AddItemToArray(collection,data);

    // char *s = cJSON_Print(collection);
    // printf("%s\n", s);
    return 0;
}
