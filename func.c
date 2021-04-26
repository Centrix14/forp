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

void fl_copy_tree_by_index(list *dest, list *src, int indx) {
	list *node = NULL, *last = NULL;
	token *tk = NULL;
	int is_first = 1;
	size_t tok_size;

	node = src;
	while (node) {
		tk = (token*)node->data;
		if (!tk) {
			node = node->next;

			continue;
		}
		if (tk->index <= indx) {
			if (is_first)
				is_first = 0;
			else
				break;
		}

		// add new node
		list_add_node(dest);
		last = list_get_last(dest);

		// copy data
		tok_size = sizeof(node->data);
		last->data = malloc(tok_size);
		memcpy(last->data, node->data, tok_size);

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
