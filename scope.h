#ifndef __FORK_PROCESSOR_LANGUAGE_SCOPE_LIBRARY_HEADER_FILE_INCLUDED__
#define __FORK_PROCESSOR_LANGUAGE_SCOPE_LIBRARY_HEADER_FILE_INCLUDED__

typedef struct {
	char *name;

	int tag;
	int call_level;
} scope;

int sl_scope_check(list *scope_list, char *name);

list *sl_scope_list_init();
void sl_scope_free(list *scope_list);

char *sl_scope_get_name(list *scope_list, int tag);
int sl_scope_get_tag(list *scope_list, char *name);
char *sl_scope_get_by_call_level(list *node, int level);

int sl_scope_new(list *scope_list, char *name);
void sl_scope_remove(list *scope_list, int tag);
void sl_scope_delete(list *node, scope *sc);

void sl_scope_call(list *node, char *name);
void sl_scope_revoke(list *node, char *name);

#endif
