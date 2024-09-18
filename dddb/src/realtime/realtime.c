#include "realtime.h"
ChangeSubscribersList *subs = NULL;

int init_subs()
{
    subs = malloc(sizeof(ChangeSubscribersList));
    if (subs == NULL)
    {
        perror("Erreur subs alloc");
        exit(EXIT_FAILURE);
    }

    subs->count = 0;
}

int find_doc_sub(const char *doc)
{
    for (int i = 0; i < subs->count; i++)
    {
        if (strcmp(subs->list[i].doc, doc) == 0)
        {
            return i;
        }
    }

    return -1;
}

int add_change_subscriber(int client_socket, const char *doc)
{
    int index = find_doc_sub(doc);
    if (index == -1)
    {
        ChangeSubscriber chgs;
        strcpy(chgs.doc, doc);
        chgs.clients[0] = client_socket;
        chgs.count = 1;
        subs->list[subs->count++] = chgs;
    }
    else
    {
        subs->list[index].clients[subs->list[index].count++] = client_socket;
    }

    printf("Client %d subscribed to %s\n", client_socket, doc);
    return 0;
}

char *create_notification(const char *doc, cJSON *data)
{
    cJSON *notif = cJSON_CreateObject();
    cJSON_AddStringToObject(notif, "type", "NOTIFY");
    cJSON_AddStringToObject(notif, "doc", doc);
    cJSON_AddItemToObject(notif, "data", data);
    return cJSON_Print(notif);
}

int notify(const char *doc, cJSON *data)
{
    int index = find_doc_sub(doc);
    if (index != -1)
    {
        ChangeSubscriber chgs = subs->list[index];
        char *message = create_notification(doc, data);
        printf("Publishing message to topic %s\n", doc);

        for (int j = 0; j < chgs.count; j++)
        {
            printf("Sending to %d message:\n ====== \n%s\n ======\n", chgs.clients[j], message);
            send(chgs.clients[j], message, strlen(message), 0);
        }
        return 0;
    }

    printf("No no no message to topic '%s'\n", doc);
    printf("count %d\n", subs->count);

    return 1;
}