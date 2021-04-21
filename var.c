#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tl2/list.h"
#include "var.h"

static list *scope_list;
static list *var_list;

int vl_scope_get_tag(char *name) {
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

int vl_scope_check(char *name) {
	int tag = 0;

	tag = vl_scope_get_tag(name);
	if (tag > 0)
		return tag;
	else
		return vl_scope_new(name);
}

int vl_scope_new(char *name) {
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

void vl_scope_free() {
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

void vl_scope_list_init() {
	scope_list = list_init_node(NULL);
}

void vl_var_list_init() {
	var_list = list_init_node(NULL);
}

void vl_var_add(char *name, char *value, int tag) {
	list *last = NULL;
	var *new_var = NULL;

	// init
	new_var = (var*)malloc(sizeof(var));
	if (!new_var) {
		perror("forp");

		exit(0);
	}

	new_var->name = (char*)malloc(strlen(name) + 1);
	new_var->value = (char*)malloc(strlen(value) + 1);
	new_var->tag = tag;

	// copy to fields
	strcpy(new_var->name, name);
	strcpy(new_var->value, value);

	// add var to list
	list_add_node(var_list);
	last = list_get_last(var_list);
	list_set_data(last, new_var);
}

var *vl_var_get(char *name, char *scope_name) {
	list *node = NULL;
	var *vptr = NULL;
	int tag = 0;

	tag = vl_scope_get_tag(scope_name);
	node = var_list;
	while (node) {
		if (!node->data) {
			node = node->next;

			continue;
		}

		vptr = (var*)node->data;
		if (vptr)
			if (vptr->tag == tag)
				if (!strcmp(vptr->name, name))
					return vptr;
		node = node->next;
	}

	return NULL;
}

char *vl_var_get_value(char *name, char *scope_name) {
	var *vptr = NULL;

	vptr = vl_var_get(name, scope_name);
	if (vptr)
		return vptr->value;
	return NULL;
}

void vl_var_list_free() {
	list *node = NULL, *next = NULL;
	var *vptr = NULL;

	node = var_list;
	while (node) {
		next = node->next;

		if (!node->data) {
			node = node->next;

			continue;
		}

		// free scope
		vptr = (var*)node->data;
		if (vptr) {
			if (vptr->name)
				free(vptr->name);
			if (vptr->value)
				free(vptr->value);
			free(vptr);
		}

		// free node
		free(node);

		node = next;
	}
}
