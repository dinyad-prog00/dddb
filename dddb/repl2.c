//#dfghj
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>


const char *words[] = {"add", "remove", "rm", "update", "child", "children", "wife", "wives",NULL};

// Generator function for word completion.
char *my_generator (const char *text, int state)
{
    static int list_index, len;
    const char *name;

    if (!state)
    {
        list_index = 0;
        len = strlen (text);
    }

    while (name = words[list_index])
    {
        list_index++;
        
        if (strncmp (name, text, len) == 0) return strdup (name);
    }

    // If no names matched, then return NULL.
    return ((char *) NULL);
}

// Custom completion function
static char **my_completion (const char *text, int start, int end)
{
    // This prevents appending space to the end of the matching word
    rl_completion_append_character = '\0';

    char **matches = (char **) NULL;
    if (start == 0)
    {
        matches = rl_completion_matches ((char *) text, my_generator);
    }
    // else rl_bind_key ('\t', rl_abort);
    return matches;
}

int main (int argc, char *argv[])
{
    char *buf;
    rl_attempted_completion_function = my_completion;

    while ((buf = readline(">> ")) != NULL)
    {
        // rl_bind_key ('\t', rl_complete);

        if (strcmp (buf, "exit") == 0) break;
        else if (buf[0] == '\0')
        {
            free (buf);
            continue;
        }
        else
        {
            printf("enteres: %s",buf);
            add_history (buf);
        }

        free (buf);
        buf = NULL;
    }
    if (buf != NULL) free (buf);

    return 0;
}