#include "headers.h"
#include <stdio.h>
#include <string.h>

void run_wrapper(char *expr, char *save_ptr, char *home_dir,
                 char *pastevents[]) {
    stripspace(expr);
    if (expr[strlen(expr) - 1] == '&') {
        pid_t pid;
        pid = fork();

        if (pid == 0) {
            // child process

            run(expr, save_ptr, home_dir, pastevents);
        }
    } else {
        run(expr, save_ptr, home_dir, pastevents);
    }
}

// run the specified command
void run(char *expr, char *save_ptr, char *home_dir, char *pastevents[]) {
    char *cmd = strtok_r(expr, " \n", &save_ptr);
    if (cmd != NULL) {
        if (strcmp(cmd, "warp") == 0) {
            char const *dest = strtok_r(NULL, " \n", &save_ptr);
            warp(dest, home_dir, save_ptr);
        }

        else if (strcmp(cmd, "pastevents") == 0) {
            int ind = 0;
            while (pastevents[ind] != NULL) {
                fputc('\n', stdout);
                char *event = pastevents[ind++];
                fputs(event, stdout);
            }
        }
    }
}
