#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tl2/list.h"
#include "token.h"
#include "scope.h"
#include "func.h"
#include "var.h"

static list *func_list;
extern list *var_scope;

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

size_t __fl_get_tok_size(token *tk) {
	size_t tok_size = 0;

	tok_size = 2 * sizeof(int);
	if (tk->val)
		tok_size += strlen(tk->val);
	if (tk->ret)
		tok_size += strlen(tk->ret);

	return tok_size;
}

void __fl_copy_tk(token *dest_tk, token *src_tk) {
	// copy integer fields
	dest_tk->index = src_tk->index;
	dest_tk->eval_me = src_tk->eval_me;

	// copy text
	if (src_tk->val) {
		dest_tk->val = (char*)malloc(strlen(src_tk->val));
		strcpy(dest_tk->val, src_tk->val);
	}
	if (src_tk->ret) {
		dest_tk->ret = (char*)malloc(strlen(src_tk->ret));
		strcpy(dest_tk->ret, src_tk->ret);
	}
}

void __fl_create_util(list *node, list *dest) {
	list *last;
	token *tk, *last_tk;
	size_t tok_size;

	tk = (token*)node->data;
	if (!tk)
		return ;

	list_add_node(dest);
	last = list_get_last(dest);

	// get token size
	tok_size = __fl_get_tok_size(tk);

	// allocate memory
	last->data = malloc(tok_size);
	last_tk = (token*)last->data;

	// copy
	__fl_copy_tk(last_tk, tk);
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
		puts("proto\n===");

		tl_crawl_list(fptr->proto, fl_func_show_struct);

		puts("===\n");
	}
	if (fptr->body) {
		puts("body\n===");

		tl_crawl_list(fptr->body, fl_func_show_struct);

		puts("===\n");
	}

	printf("tag = %d\n", fptr->tag);
}

void fl_func_show_all() {
	tl_crawl_list(func_list, fl_func_show);
}

char *fl_func_get_name(func *fptr) {
	list *lptr;
	token *tk;

	lptr = fptr->proto->next;
	tk = (token*)lptr->data;

	return (char*)tk->val;
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

func *fl_func_get_with_syntax(list *target_scope, char *name) {
	char *scope_name, *func_name;
	char *cpy;

	cpy = (char*)malloc(strlen(name) + 1);
	if (!cpy) {
		perror("forp");

		exit(0);
	}
	strcpy(cpy, name);

	// if has syntax
	if (strchr(cpy, ':')) {
		scope_name = strtok(cpy, ":");
		func_name = strtok(NULL, ":");
	}
	else {
		scope_name = "global";
		func_name = cpy;
	}

	// return func
	return fl_func_get(target_scope, func_name, scope_name);
}

void __fl_func_set_parameters(func *fptr, list *node) {
	list *arg_node, *param_node;
	token *arg_tk, *param_tk;
	char *parameter_scope, *parameter_scope_cpy;
	int tag = 0;

	// get scope name
	parameter_scope = fl_func_get_name(fptr);
	parameter_scope_cpy = (char*)malloc(strlen(parameter_scope) + 1);
	if (!parameter_scope_cpy) {
		perror("forp");

		exit(0);
	}
	strcpy(parameter_scope_cpy, parameter_scope);
	strcat(parameter_scope_cpy, "-prm-scope");

	// get scope tag
	tag = sl_scope_check(var_scope, parameter_scope_cpy);

	// main loop
	arg_node = node->next;
	param_node = fptr->proto->next->next;
	while (arg_node && param_node) {
		arg_tk = (token*)arg_node->data;
		param_tk = (token*)param_node->data;

		printf("[%d] %s = %s\n", tag, param_tk->val, arg_tk->ret);
		vl_var_add(param_tk->val, arg_tk->ret, tag);

		arg_node = arg_node->next;
		param_node = param_node->next;
	}
}

void __fl_func_call(func *fptr) {
	
}

void __fl_func_unset_parameters(func *fptr) {
	
}

void fl_func_call(func *fptr, list *node) {
	// create variables for function
	__fl_func_set_parameters(fptr, node);

	// call function
	__fl_func_call(fptr);

	// unset variables
	__fl_func_unset_parameters(fptr);
}
