#ifndef __FORK_PROCESSOR_LANGUAGE_STANDART_LIBRARY_HEADER_FILE_INCLUDED__
#define __FORK_PROCESSOR_LANGUAGE_STANDART_LIBRARY_HEADER_FILE_INCLUDED__

void ll_exec(list *node);
void ll_run_std(int code, list *node);
void ll_process_spec_operators(list *node);

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
void ll_cb_poly(list *node);
void ll_cb_if(list *node);
void ll_cb_big(list *node);
void ll_cb_small(list *node);
void ll_cb_big_eq(list *node);
void ll_cb_small_eq(list *node);
void ll_cb_eq(list *node);
void ll_cb_not_eq(list *node);

// for internal using only!
void __sum(double *acc, double arg);
void __sub(double *acc, double arg);
void __mul(double *acc, double arg);
void __div(double *acc, double arg);

#endif
