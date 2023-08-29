#include "headers.h"

#include <_ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/syslimits.h>
#include <unistd.h>

#define USER_LENGTH 257 // max size of username in linux + 1 for NULL character

char file_path[PATH_MAX]; // beginning path

char *getrpath(char *file_path) {
    // get relative path
    char *working_dir = (char *)calloc(PATH_MAX, sizeof(char));
    getcwd(working_dir, PATH_MAX);

    if (strstr(working_dir, file_path) == NULL) // if not in the same directory
    {
        return working_dir;
    };

    // reformat working dir and assign the rest of the string to a new variable
    // free the older string from the heap
    *(working_dir + strlen(file_path) - 1) = '~';
    char *formatted_dir = (char *)calloc(PATH_MAX, sizeof(char));
    strcpy(formatted_dir, working_dir + strlen(file_path) - 1);

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

int main() {
    char username[USER_LENGTH];
    char hostname[USER_LENGTH];
    char pathname[PATH_MAX];

    getlogin_r(username, USER_LENGTH);
    gethostname(hostname, USER_LENGTH);
    getcwd(file_path, PATH_MAX);

    char *rel_pathname = getrpath(file_path);

    // Keep accepting commands
    while (1) {
        char *save_ptr;

        // Print appropriate prompt with username, systemname and directory
        // before accepting input
        prompt(username, hostname, rel_pathname);
        char input[4096];
        fgets(input, 4096, stdin);
        stripspace(input);

        char *cmd = strtok_r(input, " \n", &save_ptr);

        if (cmd != NULL && (strcmp(cmd, "warp") == 0)) {
            char const *dest = strtok_r(NULL, " \n", &save_ptr);
            warp(dest, file_path, save_ptr);
            rel_pathname = getrpath(file_path);
        }

        // warp command
    }
}
