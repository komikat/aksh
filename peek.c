#include "headers.h"

#define WHITE "\x1B[0m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"

void peek(char *name, int hidden, int long_f) {
    // hidden if hidden = 1
    DIR *d;
    d = opendir(name);

    if (d == NULL)
        return;

    struct dirent *entry; // directory entries
    entry = readdir(d);

    int count = 0;
    while ((entry = readdir(d)) != NULL) {
        count++;
    }
    printf("%d", count);

    char *list[count];
    char *type[count]; // type of file

    closedir(d);

    d = opendir(name);

    struct dirent *trav; // traverse again
    trav = readdir(d);
    int i = 0;

    while ((trav = readdir(d)) != NULL) {
        if (hidden == 0 && trav->d_name[0] == '.')
            continue;

        list[i] = strdup(trav->d_name);
        if (access(name, X_OK) != 1)
            type[i++] = GREEN;

        else if (trav->d_type == DT_REG)
            type[i++] = WHITE;

        else if (trav->d_type == DT_DIR)
            type[i++] = BLUE;
    }

    for (int i = 0; i < count; i++) {
        for (int j = i; j < count; j++) {
            if (list[i] != NULL && list[j] != NULL) {
                if (strcmp(list[i], list[j]) > 0) {
                    char *temp = list[i];
                    char *temp_col = type[i];

                    list[i] = list[j];
                    type[i] = type[j];

                    list[j] = temp;
                    type[j] = temp_col;
                }
            }
        }
    }

    for (int i = 0; i < count; i++) {
        if (list[i] != NULL) {
            printf("%s%s\n", type[i], list[i]);
        }
        printf("%s", WHITE);
    }
}
