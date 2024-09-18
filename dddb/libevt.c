#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Fonction de rappel appelée lorsqu'une nouvelle connexion est acceptée
static void accept_callback(struct evconnlistener *listener, evutil_socket_t fd,
                            struct sockaddr *address, int socklen, void *ctx) {
    // Obtenez la base d'événements du contexte
    struct event_base *base = evconnlistener_get_base(listener);

    // Créez un nouvel événement pour gérer la connexion
    struct bufferevent *bev = bufferevent_socket_new(base, fd, LEV_OPT_CLOSE_ON_FREE);
    if(bev==NULL){
        printf("bev null");
        return ;
    }

    // Définissez les fonctions de rappel pour la lecture et l'écriture
    bufferevent_setcb(bev, NULL, NULL, NULL, NULL);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    // Envoyez un message de bienvenue au client
    bufferevent_write(bev, "Bienvenue sur le serveur libevent!\n", 32);
}

int main() {
    // Initialisation de la bibliothèque libevent
    struct event_base *base = event_base_new();
    if (!base) {
        fprintf(stderr, "Erreur lors de l'initialisation de libevent.\n");
        return 1;
    }

    // Configuration du point d'écoute du serveur
    struct sockaddr_in sin ;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(12345); // Port d'écoute
    sin.sin_addr.s_addr = INADDR_ANY;

    // Création d'un écouteur de connexion
    struct evconnlistener *listener = evconnlistener_new_bind(
        base, accept_callback, NULL, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1,
        (struct sockaddr *)&sin, sizeof(sin));

    if (!listener) {
        fprintf(stderr, "Erreur lors de la création de l'écouteur.\n");
        return 1;
    }

    // Démarrage de la boucle d'événements
    event_base_dispatch(base);

    // Nettoyage des ressources
    evconnlistener_free(listener);
    event_base_free(base);

    return 0;
}
