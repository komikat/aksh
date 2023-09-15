#include "headers.h"
#include "prompt.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_s_ifmt.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <unistd.h>

// run the specified command
void run(char *expr, char *save_ptr, char *home_dir, past pastevents,
         int *foreground, child main_process, char *username, char *hostname,
         char *rel_pathname) {
    fputc('\n', stdout);
    stripspace(expr);
    int length_expr = strlen(expr); // before tokenizing

    int save_term = dup(STDOUT_FILENO); // save terminal stdout
    int save_term_input = dup(STDIN_FILENO);

    int file_not_exist = 0; // if input file doesnt exist

    char *delim_ptr;

    char *copy_expr = (char *)malloc(strlen(expr) + 1);
    strcpy(copy_expr, expr);

    char *input_file = NULL, *output_file = NULL;

    char *cmd_expr = strtok_r(expr, "<>", &delim_ptr);

    if (strchr(copy_expr, '<') != NULL) {
        // input file present
        input_file = strtok_r(NULL, "<>", &delim_ptr);
        char *trimmed_input_file = strtok(input_file, "\r\t\n ");

        if (access(trimmed_input_file, F_OK) == 0) {
            int input_file_desc = open(trimmed_input_file, O_RDONLY);
            dup2(input_file_desc, STDIN_FILENO);

            close(input_file_desc);
        } else {
            printf("\nFile doesn't exist!");
            file_not_exist = 1;
        }
    }
    if (strchr(copy_expr, '>') != NULL) {
        output_file = strtok_r(NULL, "<>", &delim_ptr);

        char *trimmed_output_file = strtok(output_file, "\r\t\n ");
        int output_file_desc;

        if (strstr(copy_expr, ">>")) {
            // if append and output file exists
            output_file_desc =
                open(trimmed_output_file, O_WRONLY | O_CREAT | O_APPEND);
        } else {
            output_file_desc =
                open(trimmed_output_file, O_WRONLY | O_CREAT | O_TRUNC);
        }
        dup2(output_file_desc, STDOUT_FILENO); // stdout
        dup2(output_file_desc, STDERR_FILENO); // stderr

        close(output_file_desc);
    }

    char *expr_ptr;
    char *cmd = strtok_r(cmd_expr, " \n", &expr_ptr);

    if (cmd != NULL && file_not_exist == 0) {

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

        else { // a system command
            if (expr[length_expr - 1] == '&') {
                expr[length_expr - 1] = '\0';

                // run in background

                // spawn a child process
                // which, in turn spawns another process
                // and waits for it complete

                int pid_runner = fork();

                if (pid_runner == 0) {

                    // first child process
                    int pid_child = fork();
                    if (pid_child == 0) {

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
                            printf("ERROR: '%s' is not a valid command.\n",
                                   cmd);

                            exit(EXIT_FAILURE);
                        }
                    } else {
                        int status;
                        printf("%d spawned succesfully.\n", pid_child);

                        prompt(username, hostname, rel_pathname);
                        waitpid(pid_child, &status, 0);

                        printf("\n%s", cmd);
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
        dup2(save_term, STDOUT_FILENO);
        dup2(save_term, STDERR_FILENO);
        dup2(save_term_input, STDIN_FILENO);
    }
}
