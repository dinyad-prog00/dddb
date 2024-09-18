#include <sys/shm.h>
#include <sys/ipc.h>
#include "utils.h"
#include "../auth/utils.h"
#include "../helpers/helpers.h"
#include "../realtime/realtime.h"

int server(Config *config)
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    //SUB
    int shm_id;

    // Create shared memory segment for ChangeSubscribersList
    shm_id = shmget(IPC_PRIVATE, sizeof(ChangeSubscribersList), IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach the shared memory to the process
    subs = (ChangeSubscribersList *)shmat(shm_id, NULL, 0);
    if (subs == (ChangeSubscribersList *)-1) {
        perror("shmat");
        exit(1);
    }

    // Initialize the shared memory (only needed by the first process)
    subs->count = 0;

    //init_subs();

    // Création du socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config->port); // Port du serveur
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Liaison du socket à l'adresse et au port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Erreur lors de la liaison du socket");
        exit(EXIT_FAILURE);
    }

    // Écoute du socket pour les connexions entrantes
    if (listen(server_socket, 5) == -1)
    {
        perror("Erreur lors de l'écoute du socket");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en attente de connexions...\n");

    // Boucle pour accepter les connexions entrantes et créer un processus pour chaque client
    while (1)
    {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1)
        {
            perror("Erreur lors de l'acceptation de la connexion");
            exit(EXIT_FAILURE);
        }

        // Autenticate
        char auth[1000];
        ssize_t recv_size = recv(client_socket, auth, sizeof(auth), 0);
        if (recv_size <= 0)
        {
            perror("Erreur lors de la réception de la req du client");
            break;
        }
        auth[recv_size] = '\0';

        cJSON *auth_json = cJSON_Parse(auth);

        char *type = get_string(auth_json, "type");

        if (isSUB(type))
        {
            char *doc = get_string(auth_json, "doc");
            add_change_subscriber(client_socket, doc);

            char *message = "Client subscribed successful!";
            send(client_socket, message, strlen(message), 0);
        }

        else if (!isLOGIN(type))
        {
            char *message = "Authentication is required!";
            send(client_socket, message, strlen(message), 0);
            printf("Authentication is required!. Closing connection.\n");
            close(client_socket);
        }
        else if (is_credentials_valid(auth_json))
        {

            char *message = "Authentication successful!";
            send(client_socket, message, strlen(message), 0);
            printf("Authentication passed.\n");
            printf("Client connecté.\n");

            // Créez un nouveau processus pour gérer la connexion client
            pid_t child_pid = fork();

            if (child_pid == 0)
            {
                // Dans le processus enfant, gérez la connexion client
                close(server_socket);
                printf("call handle_client\n");
                handle_client(client_socket);
                exit(EXIT_SUCCESS);
            }
            else if (child_pid > 0)
            {
                // Dans le processus parent, fermez le socket client et continuez à écouter
                close(client_socket);
            }
            else
            {
                perror("Erreur lors de la création du processus enfant");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            char *message = "Authentication failed!";
            send(client_socket, message, strlen(message), 0);
            printf("Authentication failed. Closing connection.\n");
            close(client_socket);
        }
    }

    // Fermeture du socket serveur
    close(server_socket);

        // Detach shared memory
    shmdt(subs);

    // Remove shared memory segment (only needed after both processes are done using it)
    shmctl(shm_id, IPC_RMID, NULL);


    return 0;
}
