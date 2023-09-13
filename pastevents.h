#ifndef PAST_H_
#define PAST_H_

#define PAST_MAX 15
#include <stdio.h>
#include <stdlib.h>

struct Past {
    char *list[PAST_MAX];
    int size;
};

typedef struct Past *past;

past init_past();
void add_event_past(past pastevents, char *event_name);
void print_past(past pastevents);
char *get_past_event(past pastevents, int index);
int filter_events(past pastevents, char *filter_term);
void purge_events(past pastevents);
#endif
