#include "headers.h"

int prefixcheck(const char *str, const char *prefix) {
    // empty string prefix of every string
    if (strlen(prefix) == 0)
        return 1;

    if (strncmp(str, prefix, strlen(prefix)) == 0)
        return 1;

    return 0;
}

char *getrpath(char *home_dir) {
    // get relative path
    char *working_dir = (char *)calloc(PATH_MAX, sizeof(char));
    getcwd(working_dir, PATH_MAX);

    if (prefixcheck(working_dir, home_dir) == 0) // if not in the same directory
    {
        return working_dir;
    };

    // reformat working dir and assign the rest of the string to a new variable
    // free the older string from the heap
    *(working_dir + strlen(home_dir) - 1) = '~';
    char *formatted_dir = (char *)calloc(PATH_MAX, sizeof(char));
    strcpy(formatted_dir, working_dir + strlen(home_dir) - 1);

    free(working_dir);
    return formatted_dir;
}

void stripspace(char *str) {
    int i, j;
    for (i = j = 0; str[i]; i++) {
        if (!isspace(str[i]) || (i > 0 && !isspace(str[i - 1]))) {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
    return;
}
