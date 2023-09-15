#include "headers.h"
#include "prompt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// run the specified command
void run(char *expr, char *save_ptr, char *home_dir, past pastevents,
         int *foreground, child main_process, char *username, char *hostname,
         char *rel_pathname) {

    stripspace(expr);
    int length_expr = strlen(expr); // before tokenizing
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
                            foreground, main_process, username, hostname,
                            rel_pathname);
                        add_event_past(pastevents, cmd_to_rn);
                    }
                }
            } else {
                print_past(pastevents);
            }

        } else if (strcmp(cmd, "proclore") == 0) {
            char const *pid = strtok_r(NULL, " \n", &save_ptr);

            if (pid == NULL) {
                proclore(main_process->pid);
            } else {
                proclore(atoi(pid));
            }

        }

        else { // a system command
            if (expr[length_expr - 1] == '&') {
                expr[length_expr - 1] = '\0';

                // run in background

                // spawn a child process
                // which, in turn spawns another process
                // and waits for it complete
                if (fork() == 0) {
                    // first child process
                    int pid_child = fork();

                    if (pid_child == 0) {

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
                            printf("ERROR: '%s' is not a valid command.\n",
                                   cmd);
                            *foreground = 0;
                            exit(EXIT_FAILURE);
                        }
                    } else {
                        int status;
                        waitpid(pid_child, &status, 0);
                        printf("\n%d ended succesfully\n", pid_child);
                        prompt(username, hostname, rel_pathname);
                        exit(EXIT_SUCCESS);
                    }
                }

            } else {
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
                        printf("ERROR: '%s' is not a valid command.\n", cmd);
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
}
