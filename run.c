#include "headers.h"
#include <stdio.h>
#include <string.h>

// run the specified command
void run(char *expr, char *save_ptr, char *home_dir, past pastevents,
         int *foreground, child main_process) {

    stripspace(expr);
    char *cmd = strtok_r(expr, " \n", &save_ptr);

    if (cmd != NULL) {
        if (strcmp(cmd, "warp") == 0) {
            char const *dest = strtok_r(NULL, " \n", &save_ptr);
            warp(dest, home_dir, save_ptr);
        }

        else if (strcmp(cmd, "pastevents") == 0) {
            char const *option = strtok_r(NULL, " \n", &save_ptr);

            if (option != NULL && strcmp(option, "purge") == 0) {
                purge_events(pastevents);
            } else if (option != NULL && strcmp(option, "execute") == 0) {
                char const *num = strtok_r(NULL, " \n", &save_ptr);

                if (num != NULL) {
                    char *cmd_to_rn = get_past_event(pastevents, atoi(num));
                    if (cmd_to_rn != NULL) {
                        run(cmd_to_rn, save_ptr, home_dir, pastevents,
                            foreground, main_process);
                        add_event_past(pastevents, cmd_to_rn);
                    }
                }
            } else {
                print_past(pastevents);
            }

        } else { // a system command
            *foreground = 1;
            fputc('\n', stdout);
            if (fork() == 0) {
                // child process
                pid_t pid = getpid();

                char *argument_list[5] = {NULL}; // limited to 5
                argument_list[0] = cmd;
                int arg_index = 1;
                char *arg = strtok_r(NULL, " \n", &save_ptr);

                while (arg != NULL) {
                    argument_list[arg_index++] = arg;
                    arg = strtok_r(NULL, " \n", &save_ptr);
                }

                int status_code = execvp(cmd, argument_list);

                if (status_code == -1) {
                    char *error_string;
                    asprintf(&error_string,
                             "ERROR: '%s' is not a valid command.\n", cmd);
                    fputs(error_string, stdout);
                    *foreground = 0;
                    exit(EXIT_FAILURE);
                }
            } else {
                int status;
                wait(&status);
                *foreground = 0;
            }
        }
    }
}
