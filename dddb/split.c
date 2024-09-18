#include <stdio.h>
#include <string.h>

int main() {
    char input[] = "Ceci   est   une   phrase              avec   des   espaces";
    const char delimiter[] = " ";  // L'espace est le délimiteur

    char *token;
    char *rest = input;  // Pointeur pour suivre la position restante dans la chaîne

    // Utilisation de strtok() pour diviser la chaîne en tokens
    while ((token = strtok_r(rest, delimiter, &rest))) {
        printf("%s\n", token);
    }

    return 0;
}


