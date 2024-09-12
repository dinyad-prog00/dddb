#include "utils.h"
#include "../store/store.h"
#include "../helpers/helpers.h"

cJSON *find_user(char *username)
{
    return find_by_unique_key("admin", "users", "username", cJSON_CreateString(username));
}

int is_credentials_valid(cJSON *auth_json)
{
    char *username = get_string(auth_json, "username");
    char *password = get_string(auth_json, "password");

    if (username == NULL || password == NULL || strcmp(username, "")==0 || strcmp(password, "")==0)
    {
        return 0;
    }

    printf("Hellllllllll user pwd %s, %s, %d\n", username, password, strcmp(username, ""));

    cJSON *user = find_user(username);

    if (cJSON_IsNull(user))
    {
        return 0;
    }
    char *pwd = get_string(user, "password");

    return strcmp(password, pwd) == 0;
}
