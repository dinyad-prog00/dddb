
#include "helpers.h"

char *get_string(cJSON *json, char *key)
{
    cJSON *sj = cJSON_GetObjectItemCaseSensitive(json, key);
    if (sj == NULL)
    {
        return NULL;
    }
    char *s = cJSON_Print(sj);
    s[strlen(s) - 1] = '\0';
    s++;
    return s;
}

char *listDirectories(const char *directory_path)
{
    // Open the directory
    DIR *dir = opendir(directory_path);

    if (dir == NULL)
    {
        perror("opendir");
        return NULL; // Failed to open the directory
    }

    // Create a buffer to accumulate directory names
    size_t buffer_size = 1024; 
    char *buffer = (char *)malloc(buffer_size);
    if (buffer == NULL)
    {
        closedir(dir);
        return NULL; // Memory allocation failed
    }

    buffer[0] = '\0'; // Initialize the buffer as an empty string

    // Read directory entries
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {

        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            // Append the directory name followed by a newline
            strncat(buffer, removeExtension(entry->d_name), buffer_size - strlen(buffer) - 1);
            strncat(buffer, "|", buffer_size - strlen(buffer) - 1);
        }
    }

    // Close the directory
    closedir(dir);

    return buffer;
}

char *removeExtension(char *str)
{

    char *lastDot = strrchr(str, '.');
    char *s = malloc(sizeof(char) * 50);
    if (lastDot != NULL)
    {

        size_t lengthToKeep = lastDot - str;

        strncpy(s, str, lengthToKeep);
        s[lengthToKeep] = '\0';
        return s;
    }

    return str;
}

void cleanStr(char *str)
{
    // Reset the string length to zero
    str[0] = '\0';

    // Overwrite the string data with zeroes
    memset(str, 0, strlen(str));
}

char *apply_newline(char *str)
{
     size_t buffer_size = 1024;
    char *strn = malloc(sizeof(char) *buffer_size );
    strn[0] = '\0';
    char *tkn;
    char *rest = str;

    while ((tkn = strtok_r(rest, "|", &rest)))
    {
        strncat(strn, tkn, buffer_size - strlen(strn) - 1);
        strncat(strn, "\n", buffer_size - strlen(strn) - 1);
    }

    return strn;
}

Cmd *create_cmd(char *cmd_string)
{
    Cmd *cmd = malloc(sizeof(Cmd));
    char *tkn;
    char *rest = cmd_string;
    cmd->argc = 0;

    while ((tkn = strtok_r(rest, " ", &rest)))
    {
        cmd->argv[cmd->argc++] = tkn;
    }

    return cmd;
}