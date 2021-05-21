#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tl2/list.h"
#include "scope.h"

static int call_level_count = 0;
extern int current_scope_tag;
extern char *current_scope;

int sl_scope_get_tag(list *scope_list, char *name) {
	list *node = NULL;
	scope *sc = NULL;

	node = scope_list;
	while (node) {
		if (!node->data) {
			node = node->next;

			continue;
		}

		sc = (scope*)node->data;
		if (sc->name)
			if (!strcmp(sc->name, name))
				return sc->tag;

		node = node->next;
	}

	return -1;
}

int sl_scope_check(list *scope_list, char *name, int is_check) {
	int tag = -1;

	// get tag
	if (is_check)
		tag = sl_scope_get_tag(scope_list, name);

	// return tag
	if (tag >= 0)
		return tag;
	else
		return sl_scope_new(scope_list, name);
}

int sl_scope_new(list *scope_list, char *name) {
	static int count = 0;
	list *last = NULL;
	scope *sc = NULL;

	// init
	sc = (scope*)malloc(sizeof(scope));
	if (!sc) {
		perror("forp");

		exit(0);
	}

	sc->name = (char*)malloc(strlen(name) + 1);
	sc->tag = count++;
	sc->call_level = 0;

	// copy name to scope
	strcpy(sc->name, name);

	// add scope to list
	list_add_node(scope_list);
	last = list_get_last(scope_list);
	list_set_data(last, sc);

	return sc->tag;
}

void sl_scope_free(list *scope_list) {
	list *node = NULL, *next = NULL;
	scope *sc = NULL;

	node = scope_list;
	while (node) {
		next = node->next;

		if (!node->data) {
			node = node->next;

			continue;
		}

		// free scope
		sc = (scope*)node->data;
		if (sc) {
			if (sc->name)
				free(sc->name);
			free(sc);
		}

		// free node
		free(node);

		node = next;
	}
}

list *sl_scope_list_init() {
	return list_init_node(NULL);
}

char *sl_scope_get_name(list *scope_list, int tag) {
	list *node = NULL;
	scope *sc = NULL;

	node = scope_list;
	while (node) {
		sc = (scope*)node->data;
		if (sc)
			if (sc->tag == tag)
				return sc->name;

		node = node->next;
	}

	return NULL;
}

void sl_scope_remove(list *scope_list, int tag) {
	list *node = NULL, *next = NULL;
	scope *sc = NULL;
	
	node = scope_list;
	while (node) {
		next = node->next;

		sc = (scope*)node->data;
		if (!sc) {
			node = node->next;

			continue;
		}

		if (sc->tag == tag)
			sl_scope_delete(node, sc);

		node = next;
	}
}

void sl_scope_delete(list *node, scope *sc) {
	// unbound
	if (node->prev)
		node->prev->next = node->next;
	if (node->next)
		node->next->prev = node->prev;

	// free sc
	if (sc->name)
		free(sc->name);
	free(sc);

	// free node
	free(node);
}

void sl_scope_call(list *node, int tag) {
	list *lptr = NULL;
	scope *sc = NULL;

	lptr = node;
	while (lptr) {
		sc = (scope*)lptr->data;
		if (!sc) {
			lptr = lptr->next;

			continue;
		}

		if (sc->tag == tag) {
			sc->call_level = call_level_count++;
			current_scope_tag = sl_scope_get_by_call_level(node, call_level_count - 1);
			current_scope = sl_scope_get_name(node, current_scope_tag);
		}

		lptr = lptr->next;
	}
}

void sl_scope_revoke(list *node, int tag) {
	list *lptr = NULL;
	scope *sc = NULL;

	lptr = node;
	while (lptr) {
		sc = (scope*)lptr->data;
		if (!sc) {
			lptr = lptr->next;

			continue;
		}

		if (sc->tag == tag) {
			sc->call_level = 0;
			
			call_level_count--;
			current_scope_tag = sl_scope_get_by_call_level(node, call_level_count - 1);
			current_scope = sl_scope_get_name(node, current_scope_tag);
		}

		lptr = lptr->next;
	}
}

int sl_scope_get_by_call_level(list *node, int level) {
	list *lptr = NULL;
	scope *sc = NULL;

	lptr = node;
	while (lptr) {
		sc = (scope*)lptr->data;
		if (!sc) {
			lptr = lptr->next;

			continue;
		}

		if (sc->call_level == level)
			return sc->tag;

		lptr = lptr->next;
	}

	return -1;
}

char *sl_scope_get_active(list *node) {
	list *lptr = NULL;
	scope *sc = NULL;

	lptr = node;
	while (lptr) {
		if (!lptr->data) {
			lptr = lptr->next;

			continue;
		}

		sc = (scope*)lptr->data;
		if (sc->call_level == (call_level_count - 1))
			return sc->name;

		lptr = lptr->next;
	}

	return NULL;
}

void sl_scope_show(list *node) {
	scope *sc;

	if (!node->data)
		return ;
	sc = (scope*)node->data;

	printf("%s {%3d} [%3d]\n", sc->name, sc->tag, sc->call_level);
}
