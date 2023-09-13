#include "headers.h"
#include "pastevents.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define USER_LENGTH 257 // max size of username in linux + 1 for NULL character

char home_dir[PATH_MAX];     // beginning path
int child_pids[1000] = {-1}; // pids of child processes
int child_current_idx = 0;   // index of the current child id

struct termios orig_term;

// raw mode
void disable_raw() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term); }

void add_child(int id) { child_pids[child_current_idx++] = id; }

// forground
int foreground = 0; // if process running on the foreground

int main() {
    past pastevents = init_past();

    tcgetattr(STDIN_FILENO, &orig_term);

    // turn off canonical mode
    orig_term.c_lflag &= ~(ICANON | ECHO | ECHOE);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term);
    char backspace = orig_term.c_cc[VERASE];

    char username[USER_LENGTH];
    char hostname[USER_LENGTH];

    getlogin_r(username, USER_LENGTH);
    gethostname(hostname, USER_LENGTH);
    getcwd(home_dir, PATH_MAX);

    // input stuff
    char input[4096];
    input[0] = 0;
    char last[4096] = "None";

    // child processes list
    child main_process = create_node(getpid());

    int index_cmd = 0; // index of the command

    // Keep accepting commands
    // only run if no command in foreground
    while (1) {
        if (getpid() == main_process->pid) {
            if (foreground == 0) {
                // when no foreground process
                char *save_ptr;
                int tabbed = 0;
                char *rel_pathname = getrpath(home_dir);
                // Print appropriate prompt with username, systemname and
                // directory before accepting input
                prompt(username, hostname, rel_pathname);

                int i = 0;

                char c;
                while ((c = getchar()) != EOF && c != '\n') {
                    // fputs(input, stdout);
                    if (c == backspace && i > 0) {
                        fputs("\b \b", stdout);

                        input[--i] = 0x00;
                    }

                    else if (c == '\t') {
                        tabbed = 1;

                        int found = filter_events(pastevents, input);

                        if (found == 1) {
                            break;
                        }
                    } else if (c != backspace && c != '\t') {
                        input[i++] = c;
                        input[i] = 0;
                        fputc(c, stdout);
                    }
                }

                if (tabbed == 0) {

                    input[i] = 0;
                    add_event_past(pastevents, input);

                    stripspace(input);

                    // check if ";"
                    char *semicolon_ptr;
                    char *expr =
                        strtok_r(input, ";",
                                 &semicolon_ptr); // the first expression

                    // run for every segment
                    while (expr != NULL) {
                        run(expr, save_ptr, home_dir, pastevents, &foreground,
                            main_process);
                        expr = strtok_r(NULL, ";", &semicolon_ptr);
                    }
                }

                strcpy(last, input);
                // reset input
                strcpy(input, "");

                // next prompt
                fputc('\n', stdout);
                // foreground = 0;
            }
        } else {
            fputs("im in the child process now\n", stdout);
        }
        // in child process
    }
}
