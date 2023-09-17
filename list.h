#ifndef LIST_H
#define LIST_H
#include <sys/_types/_pid_t.h>

struct Elem {
    pid_t pid;
    struct Elem *next;
    int status; // running, stopped
    char *cmd;  // name of the command
};

typedef struct Elem *child; // child process
void insert_child(child *head, pid_t pid, char *cmd);
pid_t delete_node(child head, pid_t pid);
child create_node(pid_t pid, char *cmd);
void print_list(child head);
#endif
