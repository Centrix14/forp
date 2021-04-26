#ifndef __FORK_PROCESSOR_LANGUAGE_STANDART_LIBRARY_HEADER_FILE_INCLUDED__
#define __FORK_PROCESSOR_LANGUAGE_STANDART_LIBRARY_HEADER_FILE_INCLUDED__

void ll_exec(list *node);
void ll_run_std(int code, list *node);

int ll_is_std(char *name);
void ll_make_ret(list *node);
void ll_eval_var(list *node, var *vptr);

void ll_cb_print(list *node);
void ll_cb_help(list *node);
void ll_cb_nil(list *node);
void ll_cb_sum(list *node);
void ll_cb_sub(list *node);
void ll_cb_mul(list *node);
void ll_cb_div(list *node);
void ll_cb_let(list *node);
void ll_cb_func(list *node);

// for internal using only!
void __sum(double *acc, double arg);
void __sub(double *acc, double arg);
void __mul(double *acc, double arg);
void __div(double *acc, double arg);

#endif
