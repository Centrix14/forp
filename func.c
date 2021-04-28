#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tl2/list.h"
#include "token.h"
#include "scope.h"
#include "func.h"

static list *func_list;

void fl_func_list_init() {
	func_list = list_init_node(NULL);
}

void fl_add_func(list *proto, list *body, int tag) {
	list *last;
	func *fptr;
	token *proto_tk, *body_tk;
	int proto_index, body_index;

	// create new node
	list_add_node(func_list);
	last = list_get_last(func_list);

	// create function
	fptr = (func*)malloc(sizeof(func));
	if (!fptr) {
		perror("forp");

		exit(0);
	}

	// init fields
	fptr->tag = tag;
	fptr->proto = list_init_node(NULL);
	fptr->body = list_init_node(NULL);

	// extract tokens
	proto_tk = (token*)proto->data;
	body_tk = (token*)body->data;

	if (!proto_tk || !body_tk)
		return ;

	// get index
	proto_index = proto_tk->index;
	body_index = body_tk->index;

	// copy proto tree to function
	fl_copy_tree_by_index(fptr->proto, proto, proto_index);
	 
	// copy body tree to function
	fl_copy_tree_by_index(fptr->body, body, body_index);

	// add func to list
	list_add_node(func_list);
	last = list_get_last(func_list);
	list_set_data(last, fptr);
}

void __fl_create_util(list *node, list *dest) {
	list *last;
	token *tk;
	size_t tok_size;

	tk = (token*)node->data;
	if (!tk)
		return ;

	list_add_node(dest);
	last = list_get_last(dest);

	// copy data
	tok_size = sizeof(node->data);
	last->data = malloc(tok_size);
	memcpy(last->data, node->data, tok_size);
}

void fl_copy_tree_by_index(list *dest, list *src, int indx) {
	list *node = NULL;
	token *tk = NULL;

	// copy first node
	__fl_create_util(src, dest);

	// main loop
	node = src->next;
	while (node) {
		tk = (token*)node->data;
		if (!tk) {
			node = node->next;

			continue;
		}
		if (tk->index <= indx)
			break;

		// add new node
		__fl_create_util(node, dest);

		node = node->next;
	}
}

void fl_func_list_free() {
	list *node = NULL, *next = NULL;
	func *fptr = NULL;

	node = func_list;
	while (node) {
		next = node->next;

		if (!node->data) {
			node = node->next;

			continue;
		}

		// free func
		fptr = (func*)node->data;
		if (fptr) {
			if (fptr->proto)
				list_free_list(fptr->proto);
			if (fptr->body)
				list_free_list(fptr->body);
			free(fptr);
		}

		// free node
		free(node);

		node = next;
	}
}

void fl_func_show_struct(list *node) {
	token *tk;

	tk = (token*)node->data;
	if (!tk)
		return ;

	printf("val: %s\n", tk->val);
	printf("ret: %s\n", tk->ret);
}

void fl_func_show(list *lptr) {
	func *fptr;

	// extract func
	fptr = (func*)lptr->data;
	if (!fptr)
		return ;

	// show fields
	if (fptr->proto) {
		puts("===\nproto\n");

		tl_crawl_list(fptr->proto, fl_func_show_struct);

		puts("===\nproto\n");
	}
	if (fptr->body) {
		puts("===\nbody\n");

		tl_crawl_list(fptr->body, fl_func_show_struct);

		puts("===\nbody\n");
	}

	printf("tag = %d\n", fptr->tag);
}

void fl_func_show_all() {
	tl_crawl_list(func_list, fl_func_show);
}

char *fl_func_get_name(func *fptr) {
	list *lptr;

	lptr = fptr->proto->next;

	return (char*)lptr->data;
}

func *fl_func_get(list *target_scope, char *name, char *scope_name) {
	list *node;
	func *fptr;
	int tag = 0;
	char *func_name;

	// get tag
	tag = sl_scope_get_tag(target_scope, scope_name);

	// main loop
	node = func_list;
	while (node) {
		if (!node->data) {
			node = node->next;

			continue;
		}

		fptr = (func*)node->data;
		if (fptr->tag == tag) {
			func_name = fl_func_get_name(fptr);

			if (!strcmp(func_name, name))
				return fptr;
		}

		node = node->next;
	}

	return NULL;
}
