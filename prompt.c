#include "headers.h"

void prompt(char *username, char *hostname, char *pathname) {
    // creates the prompt string and prints it
    char *promptstr;
    asprintf(&promptstr, "<%s@%s:%s> ", username, hostname, pathname);
    fputs(promptstr, stdout);
}
