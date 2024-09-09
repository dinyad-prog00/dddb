#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
char *suggestions[] = {
    "tasks.getOne",
    "tasks.find",
    "tasks.update",
    "tasks.insert",
    "collections.create",
    "collections.delete",
    "help",
    "exit",
    NULL};
// Fonction de complétion personnalisée
char **custom_completion(const char *text, int start, int end);
char *custom_completion_generator(const char *text, int state);
char *custom_completion2(const char *text, int state);

int main()
{
    char *input;

    // Chemin du fichier d'historique persistant
    char *history_file = "dddbsh_history";

    // Initialise l'historique readline et charge l'historique depuis le fichier
    using_history();
    read_history(history_file);

    // Configure la fonction de complétion personnalisée
    // rl_attempted_completion_function = custom_completion;
    rl_completion_entry_function = custom_completion2;
    rl_readline_name = "test>";

    while (1)
    {
        input = readline("test>"); // Affiche un prompt et lit l'entrée utilisateur

        if (!input)
        {
            // L'utilisateur a appuyé sur Ctrl+D (EOF)
            printf("\nExiting REPL.\n");
            break;
        }

        // Ajoute la commande à l'historique
        if (input[0] != '\0')
        {
            add_history(input);
            append_history(1, history_file); // Enregistre l'historique dans le fichier
        }

        // Ici, vous pouvez évaluer et traiter la commande de l'utilisateur
        // Par exemple, vous pouvez l'envoyer à votre SGBD personnalisé.

        // Affiche la commande pour l'exemple
        printf("Command entered: %s\n", input);

        // Libère la mémoire allouée par readline
        free(input);
    }

    // Libère les ressources readline
    rl_clear_history();

    return 0;
}

// Fonction de complétion personnalisée
char **custom_completion(const char *text, int start, int end)
{
    char **matches = NULL;
    rl_completion_append_character = '\0';
    // Si la chaîne de saisie n'est pas vide
    if (text && text[0] != '\0')
    {
        // Initialise la liste des suggestions

        matches = rl_completion_matches(text, custom_completion_generator);
    }

    return matches;
}

// Générateur de suggestions pour la complétion personnalisée
char *custom_completion_generator(const char *text, int state)
{
    static int list_index, len;
    char *name;

    // Lors de l'initialisation, obtenez la longueur de la chaîne de saisie
    if (!state)
    {
        list_index = 0;
        len = strlen(text);
    }

    // Parcourez les suggestions jusqu'à en trouver une correspondance
    while ((name = suggestions[list_index]))
    {
        list_index++;
        // Si la suggestion commence par le texte saisi
        if (strncmp(name, text, len) == 0)
        {
            // Retourne la suggestion (ajoutée à la liste de correspondances)
            return strdup(name);
        }
    }

    // Aucune suggestion trouvée, retourne NULL
    return NULL;
}

char *custom_completion2(const char *text, int state)
{
    static int list_index, len;
    char *name;
    rl_completion_append_character = '\0';
    // Lors de l'initialisation, obtenez la longueur de la chaîne de saisie
    if (!state)
    {
        list_index = 0;
        len = strlen(text);
    }

    // Parcourez les suggestions jusqu'à en trouver une correspondance
    while ((name = suggestions[list_index]))
    {
        list_index++;
        // printf("%s\n", name);
        //  Si la suggestion commence par le texte saisi
        if (strncmp(name, text, len) == 0)
        {
            // Retourne la suggestion
            return strdup(name);
        }
    }

    // Aucune suggestion trouvée, retourne NULL pour terminer la recherche
    return NULL;
}