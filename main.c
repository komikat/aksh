#include "headers.h"
#include <string.h>
#include <sys/syslimits.h>
#include <unistd.h>

#define USER_LENGTH 257 // max size of username in linux + 1 for NULL character

char file_path[PATH_MAX]; // beginning path

char *getrpath(char *file_path) {
    char *working_dir = (char *)calloc(PATH_MAX, sizeof(char));

    getcwd(working_dir, PATH_MAX);
    if (strstr(file_path, working_dir) == NULL) // if not in the same directory
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
        // Print appropriate prompt with username, systemname and directory
        // before accepting input
        prompt(username, hostname, rel_pathname);
        char input[4096];
        fgets(input, 4096, stdin);
    }
}
