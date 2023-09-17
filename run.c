#include "headers.h"

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

    char *pipe_str = (char *)malloc(strlen(expr) + 1);
    strcpy(pipe_str, expr);

    int pipe_cmds = 1;
    for (int i = 0; i < strlen(pipe_str); i++) {
        if (pipe_str[i] == '|') {
            pipe_cmds++;
        }
    }

    // parse first statement before |
    char *pipe_ptr;
    char *pipe_expr = strtok_r(pipe_str, "|", &pipe_ptr);

    char *final_exp = (char *)calloc(strlen(pipe_expr + 1), 1);
    strcpy(final_exp, pipe_expr);

    char *cmd_expr = strtok_r(pipe_expr, "<>", &delim_ptr);

    // input output files for io
    char *input_file = NULL, *output_file = NULL;

    int prev = STDIN_FILENO;

    if (strchr(expr, '<') != NULL) {
        // input file present
        input_file = strtok_r(NULL, "<>", &delim_ptr);
        char *trimmed_input_file = strtok(input_file, "\r\t\n ");

        if (access(trimmed_input_file, F_OK) == 0) {

            int input_file_desc = open(trimmed_input_file, O_RDONLY);
            dup2(input_file_desc, prev);

            close(input_file_desc);
        } else {
            printf("\nFile doesn't exist!");
            file_not_exist = 1;
        }
    }

    // if input file exists
    if (file_not_exist == 0) {
        if (pipe_cmds > 1) {

            size_t i, n;
            n = pipe_cmds;
            int prev, fd[2];

            for (int i = 0; i < n - 1; i++) {
                pipe(fd);
                if (fork() == 0) {
                    if (prev != STDIN_FILENO) {
                        dup2(prev, STDIN_FILENO);
                        close(prev);
                    }

                    dup2(fd[1], STDOUT_FILENO);
                    close(fd[1]);
                    run_expr(pipe_expr, home_dir, pastevents, foreground, expr,
                             save_ptr, main_process, username, hostname,
                             rel_pathname, length_expr, save_term,
                             save_term_input);

                    exit(0);
                }

                close(prev);
                close(fd[1]);
                prev = fd[0];
                pipe_expr = strtok_r(NULL, "|", &pipe_ptr);
            }
            if (prev != STDIN_FILENO) {
                dup2(prev, STDIN_FILENO);
                close(prev);
            }

            final_exp = pipe_expr;
        }

        // i/o redirection

        char *final_delim;
        char *final_cmd = strtok_r(final_exp, "<>", &final_delim);

        if (strchr(expr, '>') != NULL) {
            output_file = strtok_r(NULL, ">", &final_delim);

            char *trimmed_output_file = strtok(output_file, "\r\t\n ");
            int output_file_desc;

            if (strstr(expr, ">>")) {
                // if append and output file exists
                output_file_desc =
                    open(trimmed_output_file, O_WRONLY | O_CREAT | O_APPEND);
            } else {
                output_file_desc =
                    open(trimmed_output_file, O_WRONLY | O_CREAT | O_TRUNC,
                         S_IWUSR | S_IRUSR);
            }
            dup2(output_file_desc, STDOUT_FILENO); // stdout
            dup2(output_file_desc, STDERR_FILENO); // stderr

            close(output_file_desc);
        }

        run_expr(final_cmd, home_dir, pastevents, foreground, expr, save_ptr,
                 main_process, username, hostname, rel_pathname, length_expr,
                 save_term, save_term_input);
    }
}
