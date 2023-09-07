#include "headers.h"

#include <_ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/syslimits.h>
#include <unistd.h>

#include <termios.h>

#define USER_LENGTH 257 // max size of username in linux + 1 for NULL character

char file_path[PATH_MAX]; // beginning path

struct termios orig_term;

// raw mode
void enable_rawmode() {}

void disable_raw() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term); }

char *getrpath(char *file_path) {
    // get relative path
    char *working_dir = (char *)calloc(PATH_MAX, sizeof(char));
    getcwd(working_dir, PATH_MAX);

    if (strstr(working_dir, file_path) == NULL) // if not in the same directory
    {
        return working_dir;
    };

    // reformat working dir and assign the rest of the string to a new variable
    // free the older string from the heap
    *(working_dir + strlen(file_path) - 1) = '~';
    char *formatted_dir = (char *)calloc(PATH_MAX, sizeof(char));
    strcpy(formatted_dir, working_dir + strlen(file_path) - 1);

    free(working_dir);
    return formatted_dir;
}

void stripspace(char *str) {
    int i, j;
    for (i = j = 0; str[i]; i++) {
        if (!isspace(str[i]) || (i > 0 && !isspace(str[i - 1]))) {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
    return;
}

int prefixcheck(const char *str, const char *prefix) {
    // empty string prefix of every string
    if (strlen(prefix) == 0)
        return 1;

    if (strncmp(str, prefix, strlen(prefix)) == 0)
        return 1;

    return 0;
}

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
    getcwd(file_path, PATH_MAX);

    char *rel_pathname = getrpath(file_path);

    char input[4096];
    input[0] = 0;
    char last[4096];
    // Keep accepting commands
    while (1) {
        char *save_ptr;
        int tabbed = 0;

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
        fputc('\n', stdout);
        if (tabbed == 0) {

            input[i] = 0;
            strcpy(last, input);

            stripspace(input);

            // check if ";"

            char *cmd = strtok_r(input, " \n", &save_ptr);

            if (cmd != NULL && (strcmp(cmd, "warp") == 0)) {
                char const *dest = strtok_r(NULL, " \n", &save_ptr);
                warp(dest, file_path, save_ptr);
                rel_pathname = getrpath(file_path);
            }

            // warp command
        }
        strcpy(input, "");
    }
    disable_raw();
}
