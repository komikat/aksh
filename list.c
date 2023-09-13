#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/_types/_pid_t.h>

child create_node(pid_t pid) {
    child new = (child)calloc(1, sizeof(struct Elem));
    new->pid = pid;
    new->next = NULL;

    return new;
}

void insert_child(child head, pid_t pid) {
    child new = create_node(pid);
    if (head == NULL) {
        head = new;
    } else {
        child current = head;
        while (current->next != NULL) {
            current = current->next;
        }

        current->next = new;
    }
}

pid_t delete_node(child head, pid_t pid) {
    child current = head;
    child prev = NULL;

    while (current->next != NULL && current->pid != pid) {
        prev = current;
        current = current->next;
    }
    if (current->pid == pid) {
        if (prev != NULL) {
            pid_t removed = current->pid;
            prev->next = current->next->next;
            free(current);
            return removed;
        } else {
            // trying to delete main node
            // failure
            return -1;
        }
    } else {
        // node not found
        return -1;
    }
}
