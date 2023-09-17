#include "headers.h"

// TODO: file not exists
void run_expr(char *cmd_expr, char *home_dir, past pastevents, int *foreground,
              char *expr, char *save_ptr, child main_process, char *username,
              char *hostname, char *rel_pathname, int length_expr,
              int save_term, int save_term_input) {
    char *expr_ptr;
    char *cmd = strtok_r(cmd_expr, " \n", &expr_ptr);
    if (cmd != NULL) {

        // check if ">"
        if (strcmp(cmd, "warp") == 0) {
            char const *dest = strtok_r(NULL, " \n", &expr_ptr);
            warp(dest, home_dir, expr_ptr);
        }

        else if (strcmp(cmd, "pastevents") == 0) {
            char const *option = strtok_r(NULL, " \n", &expr_ptr);

            if (option != NULL && strcmp(option, "purge") == 0) {
                purge_events(pastevents);
            } else if (option != NULL && strcmp(option, "execute") == 0) {
                char const *num = strtok_r(NULL, " \n", &expr_ptr);

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
            char const *pid = strtok_r(NULL, " \n", &expr_ptr);

            if (pid == NULL) {
                proclore(main_process->pid);
            } else {
                proclore(atoi(pid));
            }

        }

        else if (strcmp(cmd, "peek") == 0) {
            char *dest = strtok_r(NULL, " \n", &expr_ptr);
            if (dest != NULL)
                peek(dest, 1, 1);
            else
                peek(".", 1, 1);

        }

        else if (strcmp(cmd, "iMan") == 0) {
            char *cmd = strtok_r(NULL, " \n", &expr_ptr);

            if (cmd != NULL)
                iman(cmd);
            else
                printf("What manual page do you want?\n");

        }

        else if (strcmp(cmd, "activities") == 0) {
            print_list(main_process);
        }

        else { // a system command
            if (expr[length_expr - 1] == '&') {
                expr[length_expr - 1] = '\0';
                // run in background

                // spawn a child process
                // which, in turn spawns another process
                // and waits for it complete

                int pid_runner = fork();

                char *arg_ptr;

                insert_child(&main_process, pid_runner,
                             cmd); // saving the runner address instead of child
                                   // address since cant access main memory

                if (pid_runner == 0) {

                    // first child process
                    int pid_child = fork();
                    if (pid_child == 0) {

                        char *argument_list[5] = {NULL}; // limited to 5
                        argument_list[0] = strtok_r(expr, " \n", &arg_ptr);
                        int arg_index = 1;
                        char *arg = strtok_r(NULL, " \n", &arg_ptr);

                        while (arg != NULL) {
                            argument_list[arg_index++] = arg;
                            arg = strtok_r(NULL, " \n", &arg_ptr);
                        }

                        int status_code = execvp(cmd, argument_list);

                        if (status_code == -1) {
                            printf("ERROR: '%s' is not a valid command.\n",
                                   cmd);

                            exit(EXIT_FAILURE);
                        }
                    } else {
                        int status;

                        printf("%d spawned succesfully.\n", pid_child);

                        prompt(username, hostname, rel_pathname);
                        waitpid(pid_child, &status, 0);

                        printf("%s", cmd);

                        if (WIFEXITED(status)) {
                            printf(" exited normally");
                        } else {
                            printf(" ended abnormally");
                        }
                        printf(" (%d)\n", pid_child);
                        prompt(username, hostname, rel_pathname);
                        exit(EXIT_SUCCESS);
                    }
                }

            } else {
                // foreground processes

                pid_t fore_child = fork();
                if (fore_child == 0) {
                    // child process
                    pid_t pid = getpid();

                    char *argument_list[5] = {NULL}; // limited to 5
                    argument_list[0] = cmd;
                    int arg_index = 1;
                    char *arg = strtok_r(NULL, " \n", &expr_ptr);

                    while (arg != NULL) {
                        argument_list[arg_index++] = arg;
                        arg = strtok_r(NULL, " \n", &expr_ptr);
                    }

                    int status_code = execvp(cmd, argument_list);

                    if (status_code == -1) {
                        printf("ERROR: '%s' is not a valid command.\n", cmd);
                        exit(EXIT_FAILURE);
                    }
                } else {
                    int status;
                    waitpid(fore_child, &status, 0);
                }
            }
        }
    }
    dup2(save_term, STDOUT_FILENO);
    dup2(save_term, STDERR_FILENO);
    dup2(save_term_input, STDIN_FILENO);
}
