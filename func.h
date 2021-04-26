#ifndef __FORK_PORCESSOR_LANGUAGE_FUNCTIONS_REALISATION_LIBRARY_HEADER_FILE_INCLUDED__
#define __FORK_PORCESSOR_LANGUAGE_FUNCTIONS_REALISATION_LIBRARY_HEADER_FILE_INCLUDED__

typedef struct {
	list *proto;
	list *body;

	int tag;
} func;

void fl_func_list_init();
void fl_add_func(list *proto, list *body, int tag);
void fl_copy_tree_by_index(list *dest, list *src, int indx);
void fl_func_list_free();

#endif
