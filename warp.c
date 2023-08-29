#include "headers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syslimits.h>
#include <unistd.h>

void warp(const char *dest, char *file_path, char *save_ptr) {

    // if nothing provided as dest

    // >home directory
    // >> if ~
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
        char *working_dir = (char *)calloc(PATH_MAX, sizeof(char));
        getcwd(working_dir, PATH_MAX);
        int len = strlen(working_dir);
        char *temppath = (char *)calloc(len, sizeof(char));
        strcpy(temppath, working_dir);

        char *slash = strrchr(temppath, '/');
        if (slash != NULL) {
            *slash = 0;
        }

        chdir(temppath);
    }

    else if (dest[0] == '~') {
        char *destupdate = (char *)calloc(PATH_MAX, sizeof(char));
        strcpy(destupdate, file_path);
        strcat(destupdate, &dest[1]);
        chdir(destupdate);
        free(destupdate);
    } else {
        char *destupdate = (char *)calloc(PATH_MAX, sizeof(char));
        char *working_dir = (char *)calloc(PATH_MAX, sizeof(char));
        getcwd(working_dir, PATH_MAX);
        strcpy(destupdate, working_dir);
        strcat(destupdate, "/");
        strcat(destupdate, dest);

        chdir(destupdate);
        free(destupdate);
    }

    char const *intdest = strtok_r(NULL, " \n", &save_ptr);
    if (intdest != NULL) {
        warp(intdest, file_path, save_ptr);
    }

    // strcat(destupdate);
}
