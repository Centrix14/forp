#ifndef __FORK_PROCESSOR_LANGUAGE_SCOPE_LIBRARY_HEADER_FILE_INCLUDED__
#define __FORK_PROCESSOR_LANGUAGE_SCOPE_LIBRARY_HEADER_FILE_INCLUDED__

#define CHK_STD 1
#define CHK_REC 0

typedef struct {
	char *name;

	int tag;
	int call_level;
} scope;

int sl_scope_check(list *scope_list, char *name, int is_check);

list *sl_scope_list_init();
void sl_scope_free(list *scope_list);

char *sl_scope_get_name(list *scope_list, int tag);
int sl_scope_get_tag(list *scope_list, char *name);
int sl_scope_get_by_call_level(list *node, int level);
char *sl_scope_get_active(list *node);

int sl_scope_new(list *scope_list, char *name);
void sl_scope_remove(list *scope_list, int tag);
void sl_scope_delete(list *node, scope *sc);

void sl_scope_call(list *node, int tag);
void sl_scope_revoke(list *node, int tag);

void sl_scope_show(list *node);

#endif
