#include <stdio.h>
#include <string.h>

#include "tl2/list.h"
#include "token.h"
#include "lib.h"

void ll_exec(list *node) {
	token *tk = NULL;
	int res = 0;

	tk = (token*)node->data;
	if (!tk)
		return ;

	res = ll_is_std(tk->val);
	if (res > -1)
		ll_run_std(res, node);
}

int ll_is_std(char *name) {
	char *names[] = {"print", "help", NULL};
	int i = 0;

	for (; names[i]; i++)
		if (!strcmp(names[i], name))
			return i;
	return -1;
}

void ll_run_std(int code, list *node) {
	void (*std[])(list*) = {ll_cb_print, ll_cb_help};

	(*std[code])(node);
}

int __ll_is_esc(char *str) {
	if (!strcmp(str, ".n") || !strcmp(str, ".t"))
		return 1;
	return 0;
}

void __ll_print_esc(char *str) {
	if (!strcmp(str, ".n"))
		putc('\n', stdout);
	else if (!strcmp(str, ".t"))
		putc('\t', stdout);
}

void ll_cb_print(list *node) {
	token *tk = NULL, *arg_tk = NULL;
	list *lptr = NULL;
	int arg_col = 0;

	tk = (token*)node->data;
	arg_col = tk->index + 1;

	lptr = node->next;
	while (lptr) {
		if (!lptr->data)
			break;

		arg_tk = (token*)lptr->data;
		if (arg_tk->index == arg_col) {
			if (__ll_is_esc(arg_tk->val))
				__ll_print_esc(arg_tk->val);
			else
				printf("%s", arg_tk->val);
		}

		lptr = lptr->next;
	}
}

void ll_cb_help(list *node) {
	printf("help");
}
