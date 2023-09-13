#include "pastevents.h"
#include "headers.h"
#include <stdio.h>
#include <string.h>

past init_past() {
    past new_past = (past)calloc(1, sizeof(struct Past));
    new_past->size = 0;
    return new_past;
}

void add_event_past(past pastevents, char *event_name) {
    // check if same as last
    char *last = get_past_event(pastevents, pastevents->size - 1);

    if (last != NULL &&
        (strcmp(last, event_name) == 0 || strlen(event_name) == 0 ||
         prefixcheck(event_name, "pastevents") == 1)) {
        return;
    }
    char *temp_event_name = (char *)malloc(strlen(event_name) + 1);
    strcpy(temp_event_name, event_name);

    if (pastevents->size < PAST_MAX) {
        pastevents->list[pastevents->size] = temp_event_name;
        pastevents->size++;
    } else {
        for (int i = 0; i < PAST_MAX; i++) {
            pastevents->list[i] = pastevents->list[i + 1];
            pastevents->list[PAST_MAX - 1] = temp_event_name;
        }
    }
}

void print_past(past pastevents) {
    fputc('\n', stdout);
    for (int i = 0; i < pastevents->size; i++) {
        fputs(pastevents->list[i], stdout);
        fputc('\n', stdout);
    }
}

char *get_past_event(past pastevents, int index) {
    if (index < pastevents->size && index >= 0) {
        return pastevents->list[pastevents->size - index - 1];
    }
    return NULL;
}

int filter_events(past pastevents, char *filter_term) {
    // return 1 if found, 0 otherwise
    int found = 0;
    for (int i = 0; i < pastevents->size; i++) {
        char *event = get_past_event(pastevents, i);
        if (event != NULL && prefixcheck(event, filter_term)) {
            fputc('\n', stdout);
            fputs(event, stdout);
            found = 1;
        }
    }
    if (found == 1)
        return 1;
    return 0;
}

void purge_events(past pastevents) {
    for (int i = 0; i < pastevents->size; i++) {
        free(pastevents->list[i]);
    }
    pastevents->size = 0;
}
