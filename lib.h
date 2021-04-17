#ifndef __FORK_PROCESSOR_LANGUAGE_STANDART_LIBRARY_HEADER_FILE_INCLUDED__
#define __FORK_PROCESSOR_LANGUAGE_STANDART_LIBRARY_HEADER_FILE_INCLUDED__

void ll_exec(list *node);
void ll_run_std(int code, list *node);

int ll_is_std(char *name);

void ll_cb_print(list *node);
void ll_cb_help(list *node);

#endif
