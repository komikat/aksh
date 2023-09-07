#include "headers.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_PROMPT 8192

void prompt(char *username, char *hostname, char *pathname) {
    // Do not hardcode the prmopt
    char *promptstr = (char *)calloc(MAX_PROMPT, sizeof(char));

    strcpy(promptstr, "<");
    strcat(promptstr, username);
    strcat(promptstr, "@");
    strcat(promptstr, hostname);
    strcat(promptstr, ":");
    strcat(promptstr, pathname);
    strcat(promptstr, "> ");

    fputs(promptstr, stdout);
}
