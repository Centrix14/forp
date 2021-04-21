#ifndef __FORK_PROCESSOR_LANGUAGE_VARIABLE_LIBRARY_HEADER_FILE_INCLUDED__
#define __FORK_PROCESSOR_LANGUAGE_VARIABLE_LIBRARY_HEADER_FILE_INCLUDED__

typedef struct {
	char *name;
	int tag;
} scope;

typedef struct {
	char *name;
	char *value;

	int tag;
} var;

void vl_scope_list_init();
void vl_scope_free();
int vl_scope_new(char *name);
int vl_scope_check(char *name);
int vl_scope_get_tag(char *name);

void vl_var_list_init();
void vl_var_add(char *name, char *value, int tag);
var *vl_var_get(char *name, char *scope_name);
char *vl_var_get_value(char *name, char *scope_name);
void vl_var_list_free();

#endif
