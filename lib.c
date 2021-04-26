#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tl2/list.h"
#include "token.h"
#include "var.h"
#include "lib.h"
#include "scope.h"

extern list *var_scope;

char *__ll_remove_spaces(char *str) {
	static char buf[256] = "";
	int j = 0;

	for (int i = 0; str[i]; i++)
		if (!isspace((int)str[i]))
			buf[j++] = str[i];
	buf[j] = 0;

	return buf;
}

void ll_exec(list *node) {
	token *tk = NULL;
	var *vptr = NULL;
	char *gist = NULL;
	int res = 0;

	tk = (token*)node->data;
	if (!tk)
		return ;

	// get the gist of the string
	gist = __ll_remove_spaces(tk->val);

	// check for std function
	res = ll_is_std(gist);
	if (res > -1) {
		ll_run_std(res, node);

		return ;
	}

	// check for variable
	vptr = vl_var_get_exist_with_syntax(gist);
	if (vptr) {
		ll_eval_var(node, vptr);

		return ;
	}

	ll_make_ret(node);
}

int ll_is_std(char *name) {
	char *names[] = {"print", "help", ";", "+", "-", "*", "/", "let", NULL};
	int i = 0;

	for (; names[i]; i++)
		if (!strcmp(names[i], name))
			return i;
	return -1;
}

void ll_run_std(int code, list *node) {
	void (*std[])(list*) = {ll_cb_print, ll_cb_help, ll_cb_nil, ll_cb_sum, ll_cb_sub, \
	ll_cb_mul, ll_cb_div, ll_cb_let};

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

void ll_eval_var(list *node, var *vptr) {
	char *variable_name, *variable_value, *scope_name;
	int tag = 0;
	token *tk;

	// get scope and name
	variable_name = vptr->name;
	tag = vptr->tag;
	scope_name = sl_scope_get_name(var_scope, tag);

	// get value and tk
	variable_value = vl_var_get_value(variable_name, scope_name);
	tk = (token*)node->data;
	if (!tk) {
		ll_make_ret(node);

		return ;
	}

	// alloc new ret field
	if (tk->ret)
		tk->ret = (char*)realloc(tk->ret, strlen(variable_value) + 1);
	else
		tk->ret = (char*)malloc(strlen(variable_value) + 1);

	// check
	if (!tk->ret) {
		perror("forp");

		exit(0);
	}

	// copy
	strcpy(tk->ret, variable_value);
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

	// especically for *, / and -
	if (oper == __mul)
		acc = 1;
	else if (oper == __div || oper == __sub) {
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
		else
			break;

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

void ll_cb_let(list *node) {
	list *lptr = NULL;
	token *tk, *name_tk, *val_tk, *scope_tk;
	char *name, *value, *scope_str;
	int arg_col = 0, tag = 0, exist = 0;

	// get arg_col
	tk = (token*)node->data;
	if (!tk || !tk->val)
		return;
	arg_col = tk->index + 1;

	// check for scope
	lptr = node->next;
	scope_tk = (token*)lptr->data;
	if (scope_tk->val && (scope_tk->index == arg_col)) {
		scope_str = scope_tk->val;

		if (*scope_str == ':') {
			scope_str = &scope_str[1];

			lptr = lptr->next;
		}
		else
			scope_str = "global";
	}

	// main loop
	while (lptr) {
		name_tk = (token*)lptr->data;
		if (!name_tk || (name_tk->index != arg_col)) {
			lptr = lptr->next;

			continue;
		}
		name = (char*)name_tk->val;

		// check for next
		if (lptr->next)
			val_tk = (token*)lptr->next->data;

		// check fpr value
		if (!val_tk || (val_tk->index != arg_col + 1))
			value = "";
		else
			value = val_tk->val;

		// check for existance
		exist = vl_var_get_exist(name, scope_str);
		if (exist)
			vl_var_change_value(name, value, scope_str);
		else {
			tag = sl_scope_check(var_scope, scope_str);
			vl_var_add(name, value, tag);
		}

		lptr = lptr->next;
	}
}
