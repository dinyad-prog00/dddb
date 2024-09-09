#include "utils.h"
int server(Config * config) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Création du socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config->port);  // Port du serveur
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Liaison du socket à l'adresse et au port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur lors de la liaison du socket");
        exit(EXIT_FAILURE);
    }

    // Écoute du socket pour les connexions entrantes
    if (listen(server_socket, 5) == -1) {
        perror("Erreur lors de l'écoute du socket");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en attente de connexions...\n");

    // Boucle pour accepter les connexions entrantes et créer un processus pour chaque client
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Erreur lors de l'acceptation de la connexion");
            exit(EXIT_FAILURE);
        }

        printf("Client connecté.\n");

        // Créez un nouveau processus pour gérer la connexion client
        pid_t child_pid = fork();

        if (child_pid == 0) {
            // Dans le processus enfant, gérez la connexion client
            close(server_socket);
            printf("call handle_client\n");
            handle_client(client_socket);
            exit(EXIT_SUCCESS);
        } else if (child_pid > 0) {
            // Dans le processus parent, fermez le socket client et continuez à écouter
            close(client_socket);
        } else {
            perror("Erreur lors de la création du processus enfant");
            exit(EXIT_FAILURE);
        }
    }

    // Fermeture du socket serveur
    close(server_socket);

    return 0;
}
