#ifndef RUN_EXPR_H
#define RUN_EXPR_H
void run_expr(char *cmd_expr, char *home_dir, past pastevents, int *foreground,
              char *expr, char *save_ptr, child main_process, char *username,
              char *hostname, char *rel_pathname, int length_expr,
              int save_term, int save_term_input);

#endif