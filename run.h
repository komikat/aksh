#ifndef RUN_H
#define RUN_H
#include "list.h"
#include "pastevents.h"
#include <sys/_types/_pid_t.h>

void run(char *cmd, char *save_ptr, char *home_dir, past pastevents,
         int *foreground, child main_process, char *username, char *hostname,
         char *rel_pathname);

#endif
