#include "headers.h"

void warp(const char *dest, char *file_path, char *save_ptr) {

    // if nothing provided as dest

    // >home directory
    // >> if ~

    char *destupdate = (char *)calloc(PATH_MAX, sizeof(char));
    char *working_dir = (char *)calloc(PATH_MAX, sizeof(char));
    getcwd(working_dir, PATH_MAX);

    if (dest == NULL) {
        chdir(file_path);
        return;
    } else if (strcmp(dest, "") == 0 || strcmp(dest, "~") == 0 ||
               strcmp(dest, ".") == 0) {
        chdir(file_path);
    }

    // if dash
    else if (dest != NULL && strcmp(dest, "-") == 0) {
        printf("%s\n", file_path);
    }

    // if dot dot
    else if (dest != NULL && strcmp(dest, "..") == 0) {

        int len = strlen(working_dir);
        strcpy(destupdate, working_dir);

        char *slash = strrchr(destupdate, '/');
        if (slash != NULL) {
            *slash = 0;
        }

        chdir(destupdate);
    }

    else if (dest[0] == '~') {

        strcpy(destupdate, file_path);
        strcat(destupdate, &dest[1]);
        chdir(destupdate);

    } else {

        getcwd(working_dir, PATH_MAX);
        strcpy(destupdate, working_dir);
        strcat(destupdate, "/");
        strcat(destupdate, dest);

        chdir(destupdate);
    }

    getcwd(working_dir, PATH_MAX);
    fputs(working_dir, stdout);

    char const *intdest = strtok_r(NULL, " \n", &save_ptr);
    if (intdest != NULL) {
        warp(intdest, file_path, save_ptr);
    }

    free(working_dir);
    free(destupdate);

    // strcat(destupdate);
}
