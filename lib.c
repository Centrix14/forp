#include <stdio.h>
#include <stdlib.h>
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
	else
		ll_make_ret(node);
}

int ll_is_std(char *name) {
	char *names[] = {"print", "help", ";", "+", "-", "*", "/", NULL};
	int i = 0;

	for (; names[i]; i++)
		if (!strcmp(names[i], name))
			return i;
	return -1;
}

void ll_run_std(int code, list *node) {
	void (*std[])(list*) = {ll_cb_print, ll_cb_help, ll_cb_nil, ll_cb_sum, ll_cb_sub, \
	ll_cb_mul, ll_cb_div};

	(*std[code])(node);
}

void ll_make_ret(list *node) {
	token *tk = NULL;

	if (!node || !node->data)
		return ;

	tk = (token*)node->data;
	tk->ret = (char*)malloc(strlen(tk->val) + 1);
	if (!tk->ret) {
		perror("forp");

		exit(0);
	}

	strcpy(tk->ret, tk->val);
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
			if (__ll_is_esc(arg_tk->ret))
				__ll_print_esc(arg_tk->ret);
			else
				printf("%s", arg_tk->ret);
		}

		lptr = lptr->next;
	}
}

void ll_cb_help(list *node) {
	printf("help");
}

void ll_cb_nil(list *node) {
	// the empty operation
}

void __ll_cb_math(list *node, void (*oper)(double*, double)) {
	token *tk = NULL, *arg_tk = NULL;
	list *lptr = NULL;
	int arg_col = 0;
	double acc = 0;

	// inital settings
	tk = (token*)node->data;
	arg_col = tk->index + 1;
	lptr = node->next;

	// especically for * and /
	if (oper == __mul)
		acc = 1;
	else if (oper == __div) {
		lptr = node->next->next;

		arg_tk = (token*)node->next->data;
		acc = atof(arg_tk->ret);
	}

	// main loop
	while (lptr) {
		if (!lptr->data)
			break;

		arg_tk = (token*)lptr->data;
		if (arg_tk->index == arg_col)
			(*oper)(&acc, atof(arg_tk->ret));

		lptr = lptr->next;
	}

	// alloc ret field of token
	if (tk->ret)
		tk->ret = realloc(tk->ret, strlen(tk->val) + 1);
	else
		tk->ret = malloc(strlen(tk->val) + 1);

	if (!tk->ret) {
		perror("forp");

		exit(0);
	}

	sprintf(tk->ret, "%g", acc);
}

void __sum(double *acc, double arg) {
	*acc = *acc + arg;
}

void __sub(double *acc, double arg) {
	*acc = *acc - arg;
}

void __mul(double *acc, double arg) {
	*acc = *acc * arg;
}

void __div(double *acc, double arg) {
	if (!arg) {
		fprintf(stderr, "forp: division by zero\n");

		exit(0);
	}

	*acc = *acc / arg;
}

void ll_cb_sum(list *node) {
	__ll_cb_math(node, __sum);
}

void ll_cb_sub(list *node) {
	__ll_cb_math(node, __sub);
}

void ll_cb_mul(list *node) {
	__ll_cb_math(node, __mul);
}

void ll_cb_div(list *node) {
	__ll_cb_math(node, __div);
}
