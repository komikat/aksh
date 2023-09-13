#ifndef LIST_H
#define LIST_H
#include <sys/_types/_pid_t.h>

struct Elem {
    pid_t pid;
    struct Elem *next;
};

typedef struct Elem *child; // child process
void insert_child(child head, pid_t pid);
pid_t delete_node(child head, pid_t pid);
child create_node(pid_t pid);
#endif
