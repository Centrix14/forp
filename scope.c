#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tl2/list.h"
#include "scope.h"

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

int sl_scope_check(list *scope_list, char *name) {
	int tag = 0;

	tag = sl_scope_get_tag(scope_list, name);
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
