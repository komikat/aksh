#include "headers.h"
#include <stdio.h>
#include <string.h>

void run_wrapper(char *expr, char *save_ptr, char *home_dir) {
    stripspace(expr);
    if (expr[strlen(expr) - 1] == '&') {
        pid_t pid;
        pid = fork();

        if (pid == 0) {
            // child process

            run(expr, save_ptr, home_dir);
        }
    } else {
        run(expr, save_ptr, home_dir);
    }
}

// run the specified command
void run(char *expr, char *save_ptr, char *home_dir) {
    char *cmd = strtok_r(expr, " \n", &save_ptr);
    if (cmd != NULL) {
        if (strcmp(cmd, "warp") == 0) {
            char const *dest = strtok_r(NULL, " \n", &save_ptr);
            warp(dest, home_dir, save_ptr);
        }

        else if (strcmp(cmd, "pastevents")) {
            int ind = 0;
            /* while (pastevents[ind] != NULL) { */
            /*     char *event = pastevents[ind++]; */
            /*     fputs(event, stdout); */
            /*     fputc('\n', stdout); */
            /* } */
        }
    }
}
