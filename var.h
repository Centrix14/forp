#ifndef __FORK_PROCESSOR_LANGUAGE_VARIABLE_LIBRARY_HEADER_FILE_INCLUDED__
#define __FORK_PROCESSOR_LANGUAGE_VARIABLE_LIBRARY_HEADER_FILE_INCLUDED__

typedef struct {
	char *name;
	char *value;

	int tag;
} var;

void vl_var_add(char *name, char *value, int tag);
void vl_var_change_value(char *name, char *value, char *scope_name);

void vl_var_list_free();
void vl_var_list_show();
void vl_var_list_init();

int vl_var_get_exist(char *name, char *scope_name);
var *vl_var_get(char *name, char *scope_name);
char *vl_var_get_value(char *name, char *scope_name);
var *vl_var_get_exist_with_syntax(char *name);

#endif
