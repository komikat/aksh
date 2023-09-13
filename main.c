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

void die(const char *s) {
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

// forground
int foreground = 0; // if process running on the foreground

int main() {
    past pastevents = init_past();

    char username[USER_LENGTH];
    char hostname[USER_LENGTH];

    getlogin_r(username, USER_LENGTH);
    gethostname(hostname, USER_LENGTH);
    getcwd(home_dir, PATH_MAX);

    // input stuff
    char *input = malloc(sizeof(char) * 100);
    char c;

    char last[100] = "None";

    // child processes list
    child main_process = create_node(getpid());

    int index_cmd = 0; // index of the command

    // Keep accepting commands
    // only run if no command in foreground
    while (1) {
        setbuf(stdout, NULL);
        enableRawMode();
        if (getpid() == main_process->pid) {
            if (foreground == 0) {
                // when no foreground process
                char *save_ptr;
                int tabbed = 0;
                char *rel_pathname = getrpath(home_dir);
                // Print appropriate prompt with username, systemname and
                // directory before accepting input
                prompt(username, hostname, rel_pathname);
                memset(input, '\0', 100);

                int i = 0;
                int pt = 0;

                while (read(STDIN_FILENO, &c, 1) == 1) {
                    if (iscntrl(c)) {
                        if (c == 10)
                            break;
                        else if (c == 27) {
                            char buf[3];
                            buf[2] = 0;
                            if (read(STDIN_FILENO, buf, 2) ==
                                2) { // length of escape code
                                printf("\rarrow key: %s", buf);
                            }
                        } else if (c == 127) { // backspace
                            if (pt > 0) {
                                if (input[pt - 1] == 9) {
                                    for (int i = 0; i < 7; i++) {
                                        printf("\b");
                                    }
                                }
                                input[--pt] = '\0';
                                printf("\b \b");
                            }
                        } else if (c == 9) { // TAB character
                            input[pt++] = c;
                            tabbed = 1;

                        } else if (c == 4) {
                            exit(0);
                        } else {
                            printf("%d\n", c);
                        }
                    } else {
                        input[pt++] = c;
                        printf("%c", c);
                    }
                }
                disableRawMode();

                if (tabbed == 0) {
                    printf(input);
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
