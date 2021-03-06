#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tl2/list.h"
#include "var.h"
#include "scope.h"

static list *var_list;
extern list *var_scope;
extern int current_scope_tag;
//extern char *current_scope;

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

var *vl_var_get(char *name, int tag) {
	list *node = NULL;
	var *vptr = NULL;

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

char *vl_var_get_value(char *name, int tag) {
	var *vptr = NULL;

	vptr = vl_var_get(name, tag);
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

		// free var
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

void vl_var_list_show() {
	list *node = NULL;
	var *vptr = NULL;

	node = var_list;
	while (node) {
		if (node->data) {
			vptr = (var*)node->data;
			if (vptr)
				printf("[%d] %s = %s\n", vptr->tag, vptr->name, vptr->value);
		}

		node = node->next;
	}
}

int vl_var_get_exist(char *name, int tag) {
	if (!vl_var_get(name, tag))
		return 0;
	return 1;
}

void vl_var_change_value(char *name, char *value, int tag) {
	var *vptr = NULL;

	vptr = vl_var_get(name, tag);
	if (vptr) {
		vptr->value = (char*)realloc(vptr->value, strlen(value) + 1);
		if (!vptr->value) {
			perror("forp");

			exit(0);
		}

		strcpy(vptr->value, value);
	}
}

var *vl_var_get_exist_with_syntax(char *name) {
	char *scope_name, *variable_name;
	char *cpy;
	int tag;

	cpy = (char*)malloc(strlen(name) + 1);
	if (!cpy) {
		perror("forp");

		exit(0);
	}
	strcpy(cpy, name);

	if (strchr(cpy, ':')) {
		scope_name = strtok(cpy, ":");
		variable_name = strtok(NULL, ":");
	}
	else {
		tag = current_scope_tag;
		variable_name = cpy;
	}

	if (!variable_name || !scope_name)
		return NULL;

	return vl_var_get(variable_name, tag);
}

void vl_var_free(list *node, var *vptr) {
	// remove node fomr list
	if (node->prev)
		node->prev->next = node->next;
	if (node->next)
		node->next->prev = node->prev;

	// free var
	if (vptr->name)
		free(vptr->name);
	if (vptr->value)
		free(vptr->value);
	free(vptr);

	// free node
	free(node);
}

void vl_var_remove(char *name, int tag) {
	list *node = NULL, *next = NULL;
	var *vptr = NULL;

	node = var_list;
	while (node) {
		next = node->next;

		vptr = (var*)node->data;
		if (!vptr) {
			node = node->next;

			continue;
		}

		if (vptr->tag == tag)
			if (!strcmp(vptr->name, name))
				vl_var_free(node, vptr);

		node = next;
	}
}
