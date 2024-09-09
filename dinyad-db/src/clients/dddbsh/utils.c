#include "utils.h"
char *methods[] = {
    "find",
    "findOne",
    "findById",
    "create",
    "update",
    "updateOne",
    "updateById",
    "detele",
    "deleteOne",
    "deleteById", NULL};

QueryInfo *extract_infos(char *query)
{
        QueryInfo *info = malloc(sizeof(QueryInfo));

        if (info == NULL)
        {
                return NULL;
        }

        const char *collectionEnd = strchr(query, '.');
        if (collectionEnd != NULL)
        {
                strncpy(info->collection, query, collectionEnd - query);
                info->collection[collectionEnd - query] = '\0';
        }
        else
        {
                return NULL;
        }

        collectionEnd++;
        const char *methodEnd = strchr(collectionEnd, '(');
        if (methodEnd != NULL)
        {
                strncpy(info->method, collectionEnd, methodEnd - collectionEnd);
                info->method[methodEnd - collectionEnd] = '\0';
        }
        else
        {
                return NULL;
        }

        char *args = malloc(sizeof(char) * 400);
        if (args == NULL)
        {
                return NULL;
        }

        const char *argsStart = strchr(query, '(');
        const char *argsEnd = strrchr(query, ')');
        if (argsStart != NULL && argsEnd != NULL && argsEnd > argsStart)
        {
                strncpy(args, argsStart + 1, argsEnd - argsStart - 1);
                args[argsEnd - argsStart - 1] = '\0';
        }

        if (strchr(args, '{') == NULL)
        {
                args[strlen(args) - 1] = '\0';
                args++;
                strcpy(info->_id, args);

                // free(args);

                return info;
        }

        char *arg1 = malloc(sizeof(char) * 250);
        const char *arg1Start = strchr(args, '{');
        const char *arg1End = strrchr(args, '}');

        if (arg1Start != NULL && arg1End != NULL && arg1End > arg1Start)
        {
                strncpy(arg1, arg1Start + 1, arg1End - arg1Start - 1);
                arg1[arg1End - arg1Start - 1] = '\0';
        }

        return info;
}

req_type *extract_infos2(char *req_string) // tasks.getOne("kjkjjkjkjkj")
{
        strcpy(req_string, trim(req_string));
        req_type *req_infos = malloc(sizeof(req_type));

        const char *dot = strchr(req_string, '.');
        if (dot == NULL)
        {
                return req_infos;
        }

        strncpy(req_infos->collection, req_string, dot - req_string);

        const char *openParen = strchr(req_string, '(');
        if (openParen == NULL)
        {
                return req_infos;
        }
        strncpy(req_infos->method, dot + 1, openParen - dot - 1);
        strcpy(req_infos->verb, strstr(req_infos->method, "get") != NULL ? "GET" : "POST");

        const char *closeParen = strchr(openParen, ')');

        if (closeParen == NULL)
        {
                return req_infos;
        }

        strncpy(req_infos->data, openParen + 1, closeParen - openParen - 2 > 0 ? closeParen - openParen - 1 : 0);

        if (req_infos->data[0] == '"')
        {
                strncpy(req_infos->_id, req_infos->data + 1, strlen(req_infos->data) - 2);
                strcpy(req_infos->data, "");
        }

        // printf("%s, %s, %s, %s\n", req_infos->collection, req_infos->method, req_infos->_id,req_infos->data);
        return req_infos;
}

char *trim(char *str)
{
        // Pointer to the first non-whitespace character
        char *start = str;

        // Pointer to the last non-whitespace character
        char *end = str + strlen(str) - 1;

        // Trim leading whitespace
        while (isspace(*start))
        {
                start++;
        }

        // Trim trailing whitespace
        while (end > start && isspace(*end))
        {
                end--;
        }

        // Null-terminate the trimmed string
        *(end + 1) = '\0';

        return start;
}

int is_cmd(char *str)
{
        return strchr(str, '.') == NULL && strchr(str, '(') == NULL && strchr(str, '{') == NULL && strchr(str, ')') == NULL && strchr(str, '}') == NULL && strchr(str, '"') == NULL;
}

char *custom_completion(const char *text, int state)
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
        while ((name = methods[list_index]))
        {
                list_index++;
                // printf("%s\n", name);
                //  Si la suggestion commence par le texte saisi
                char *point = strchr(text, '.');
                if (point == NULL)
                {
                        return NULL;
                }
                point++;
                if (strncmp(name, point, strlen(point)) == 0)
                {
                        // Retourne la suggestion
                        char *str = malloc(sizeof(char)*100), s[50];
                        strncpy(s,text,point-text-1);
                        s[point-text-1]='\0';
                        sprintf(str,"%s.%s",s,name);
                        return str;
                        //return strdup(name);
                }
        }

        // Aucune suggestion trouvée, retourne NULL pour terminer la recherche
        return NULL;
}