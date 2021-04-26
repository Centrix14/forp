#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tl2/list.h"
#include "token.h"

void tl_add_token(list *prog, char *tok, int index) {
	token *tk = NULL;
	list *last = NULL;

	// create token
	tk = (token*)malloc(sizeof(token));
	tk->val = malloc(strlen(tok) + 1);
	tk->ret = NULL;
	tk->index = index;
	tk->eval_me = 1;

	strcpy(tk->val, tok);

	// add to list
	list_add_node(prog);

	last = list_get_last(prog);
	list_set_data(last, tk);
}

void tl_free_token(list *prog) {
	token *tk = NULL;

	tk = (token*)prog->data;
	if (!tk)
		return;

	if (tk->val)
		free(tk->val);
	if (tk->ret)
		free(tk->ret);
	free(tk);
}

void tl_show(list *prog) {
	token *tk = NULL;

	tk = (token*)prog->data;
	if (tk)
		printf("%s\n", tk->val);
}

void tl_crawl_list(list *prog, void (*func)(list*)) {
	list *node = NULL, *next = NULL;

	node = prog;
	while (node) {
		next = node->next;

		(*func)(node);

		node = next;
	}
}

void tl_free_list(list *prog) {
	list *node = NULL, *next = NULL;

	node = prog;
	while (node) {
		next = node->next;

		if (node->next)
			node->next = NULL;

		if (node->prev)
			free(node);

		node = next;
	}
}

void tl_crawl_list_reverse(list *prog, void (*func)(list*)) {
	list *last = NULL, *node = NULL, *prev = NULL;

	last = list_get_last(prog);

	node = last;
	while (node) {
		prev = node->prev;

		if (node->data)
			(*func)(node);

		node = prev;
	}
}
