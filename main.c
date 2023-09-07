#include "headers.h"

#include <stdio.h>
#include <unistd.h>

#define USER_LENGTH 257 // max size of username in linux + 1 for NULL character

char home_dir[PATH_MAX]; // beginning path

struct termios orig_term;

// raw mode
void disable_raw() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term); }

int main() {
    char *completions[3];
    completions[0] = "test";
    completions[1] = "warp";
    completions[2] = "echo";

    tcgetattr(STDIN_FILENO, &orig_term);

    // turn off canonical mode
    orig_term.c_lflag &= ~(ICANON | ECHO | ECHOE);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term);
    char backspace = orig_term.c_cc[VERASE];

    char username[USER_LENGTH];
    char hostname[USER_LENGTH];
    char pathname[PATH_MAX];

    getlogin_r(username, USER_LENGTH);
    gethostname(hostname, USER_LENGTH);
    getcwd(home_dir, PATH_MAX);

    char input[4096];
    input[0] = 0;
    char last[4096];
    // Keep accepting commands
    pid_t pid = getpid();
    pid_t last_checked_pid = pid; // getting pid after last process
    while (pid == last_checked_pid) {
        char *save_ptr;
        int tabbed = 0;
        char *rel_pathname = getrpath(home_dir);
        // Print appropriate prompt with username, systemname and directory
        // before accepting input
        prompt(username, hostname, rel_pathname);

        // fgets(input, 4096, stdin);
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

                int found = 0;
                // fputs(input, stdin);

                for (int i = 0; i < 3; i++) {
                    if (prefixcheck(completions[i], input) == 1) {
                        found = 1;
                        fputc('\n', stdout);
                        fputs(completions[i], stdout);
                    }
                }

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
            strcpy(last, input);

            stripspace(input);

            // check if ";"
            char *semicolon_ptr;
            char *expr =
                strtok_r(input, ";", &semicolon_ptr); // the first expression

            // run for every segment
            while (expr != NULL) {
                run_wrapper(expr, save_ptr, home_dir);
                expr = strtok_r(NULL, ";", &semicolon_ptr);
            }
        }
        // reset input
        strcpy(input, "");

        // next prompt
        fputc('\n', stdout);
        last_checked_pid = getpid(); // getting pid after last process
    }
}
