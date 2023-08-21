#include "headers.h"
#include <unistd.h>

void prompt(char *username, char *hostname, char *pathname) {
    // Do not hardcode the prmopt
    printf("<%s@%s:%s>", username, hostname, pathname);
}
