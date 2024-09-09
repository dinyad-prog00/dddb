
#include "utils.h"

int main(int argc, char const *argv[])
{
    int client_socket;
    struct sockaddr_in server_addr;
    char *req;
    char *res;
    char *db = malloc(sizeof(char) * 50);
    char prompt[20];
    int running = 1;
    strcpy(db, "test");

    char *history_file = "dddbsh_history";
    char *prev_command = NULL;
    using_history();
    read_history(history_file);
    rl_completion_entry_function = custom_completion;

    // Création du socket client
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Erreur lors de la création du socket client");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (argc == 2)
    {

        server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    }
    else
    {
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    }

    // Connexion au serveur
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Erreur lors de la connexion au serveur");
        exit(EXIT_FAILURE);
    }

    printf("Connecté au serveur dddb://%s:%d\n", argc == 2 ? argv[1] : "127.0.0.1", SERVER_PORT);

    // Boucle pour la communication
    while (running)
    {
        res = malloc(sizeof(char) * MAX_LENGTH);
        // Saisie de la req
        sprintf(prompt, "\n%s>", db);
        req = readline(prompt);

        if (!req)
        {
            // L'utilisateur a appuyé sur Ctrl+D (EOF)
            printf("\nExiting REPL.\n");
            break;
        }

        // Ajoute la commande à l'historique
        if (req[0] != '\0' && (prev_command == NULL || strcmp(req, prev_command) != 0))
        {

            add_history(req);
            append_history(1, history_file);
        }

        // QueryInfo *infos = extract_infos(req);
        // printf("%s, %s, %s, %s, %s\n", infos->collection, infos->method, infos->_id, infos->filter, infos->data);
        // return 0;
        if (req == NULL || strcmp(req, "exit") == 0 || strcmp(req, "quit") == 0)
        {
            if (req == 0)
                printf("\nexit\n");
            rl_callback_handler_remove();
            running = 0;
        }

        else if (!strcmp(req, "clear"))
        {
            system("clear");
        }
        else if (!strncmp(req, "use", 3))
        {
            Cmd *cmd = create_cmd(req);
            if (cmd->argc != 2)
            {
                printf("\x1b[31mBad cmd format!\x1b[0m");
            }
            else
            {
                free(db);
                db = malloc(sizeof(char) * 50);
                strcpy(db, cmd->argv[1]);
            }
        }
        else
        {

            if (is_cmd(req))
            {
                cJSON *reqjs = cJSON_CreateObject();
                cJSON_AddStringToObject(reqjs, "type", "cmd");

                cJSON_AddStringToObject(reqjs, "cmd", req);
                cJSON_AddStringToObject(reqjs, "db", db);
                char *ss = cJSON_Print(reqjs);
                // printf("ss=%s", ss);
                send(client_socket, ss, strlen(ss), 0);
                cJSON_Delete(reqjs);
            }
            else
            {
                req_type *req_infos = extract_infos2(req);
                cJSON *reqjs = cJSON_CreateObject();
                cJSON_AddStringToObject(reqjs, "type", "req");
                cJSON_AddStringToObject(reqjs, "collection", trim(req_infos->collection));
                cJSON_AddStringToObject(reqjs, "verb", req_infos->verb);
                cJSON_AddStringToObject(reqjs, "db", db);

                if (strlen(req_infos->_id) != 0)
                {
                    cJSON *filter = cJSON_CreateObject();
                    cJSON_AddStringToObject(filter, "_id", req_infos->_id);
                    cJSON_AddItemToObject(reqjs, "filter", filter);
                    cJSON_AddItemToObject(reqjs, "single", cJSON_CreateTrue());
                }

                if (strlen(req_infos->data) != 0)
                {
                    cJSON *data = cJSON_Parse(req_infos->data);
                    // printf("data=%s",req_infos->data);
                    cJSON_AddItemToObject(reqjs, "data", data);
                }
                char *ss = cJSON_Print(reqjs);
                // printf("ss=%s", ss);
                send(client_socket, ss, strlen(ss), 0);
                cJSON_Delete(reqjs);
                free(req_infos);
            }

            // Réception de la réponse du serveur
            if (recv(client_socket, res, MAX_LENGTH, 0) <= 0)
            {
                perror("Erreur lors de la réception de la réponse du serveur");
                break;
            }
            cJSON *resjs = cJSON_Parse(res);
            char *type = get_string(resjs, "type");
            if (!strcmp(type, "error"))
            {
                char *message = get_string(resjs, "message");
                printf("\x1b[31m%s\x1b[0m", message);
            }
            else if (!strcmp(type, "str"))
            {
                char *message = get_string(resjs, "data");
                printf("\x1b[32m%s\x1b[0m", apply_newline(message));
            }
            else
            {
                char *data = cJSON_Print(cJSON_GetObjectItemCaseSensitive(resjs, "data"));
                printf("\x1b[34m%s\x1b[0m", data);
            }
        }
        if (prev_command != NULL)
        {
            free(prev_command);
        }
        prev_command = strdup(req);

        free(req);
        free(res);
    }

    // Fermeture du socket client
    close(client_socket);

    return 0;
}
