#ifndef __FORK_PORCESSOR_LANGUAGE_FUNCTIONS_REALISATION_LIBRARY_HEADER_FILE_INCLUDED__
#define __FORK_PORCESSOR_LANGUAGE_FUNCTIONS_REALISATION_LIBRARY_HEADER_FILE_INCLUDED__

typedef struct {
	list *proto;
	list *body;

	int tag;
} func;

void fl_func_list_init();
void fl_func_list_free();

void fl_add_func(list *proto, list *body, int tag);
void fl_copy_tree_by_index(list *dest, list *src, int indx);

void fl_func_show(list *lptr);
void fl_func_show_all();

char *fl_func_get_name(func *fptr);
func *fl_func_get(list *target_scope, char *name, char *scope_name);
func *fl_func_get_with_syntax(list *target_scope, char *name);
char *fl_func_get_return_value(func *fptr);

void fl_func_call(func *fptr, list *node);

#endif
