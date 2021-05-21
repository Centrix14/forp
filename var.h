#ifndef __FORK_PROCESSOR_LANGUAGE_VARIABLE_LIBRARY_HEADER_FILE_INCLUDED__
#define __FORK_PROCESSOR_LANGUAGE_VARIABLE_LIBRARY_HEADER_FILE_INCLUDED__

typedef struct {
	char *name;
	char *value;

	int tag;
} var;

void vl_var_add(char *name, char *value, int tag);
void vl_var_remove(char *name, int tag);

void vl_var_change_value(char *name, char *value, int tag);

void vl_var_list_free();
void vl_var_list_show();
void vl_var_list_init();

var *vl_var_get(char *name, int tag);
char *vl_var_get_value(char *name, int tag);

int vl_var_get_exist(char *name, int tag);
var *vl_var_get_exist_with_syntax(char *name);

void vl_var_free(list *node, var *vptr);

#endif
