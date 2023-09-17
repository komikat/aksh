#include "headers.h"

child create_node(pid_t pid, char *cmd) {
    child new = (child)calloc(1, sizeof(struct Elem));
    new->pid = pid;
    new->next = NULL;
    new->status = 1; // running
    new->cmd = strdup(cmd);

    return new;
}

void insert_child(child *head, pid_t pid, char *cmd) {

    child new = create_node(pid, cmd);
    if (*head == NULL) {
        *head = new;
    } else {
        child current = *head;

        while (current->next != NULL) {
            current = current->next;
        }

        current->next = new;
    }
}

void print_list(child head) {
    child current = head;

    while (current != NULL) {
        printf("%d: %s ", current->pid, current->cmd);

        if (current->status == 1) {
            printf("- Running\n");
        } else {
            printf("- Stopped\n");
        }

        current = current->next;
    }
}
