#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tl2/list.h"
#include "token.h"
#include "var.h"
#include "lib.h"
#include "scope.h"
#include "func.h"

extern list *var_scope;
extern list *func_scope;

char *current_scope = "global";
int marked_index = -1;

char *__ll_remove_spaces(char *str) {
	static char buf[256] = "";
	int j = 0;

	for (int i = 0; str[i]; i++)
		if (!isspace((int)str[i]))
			buf[j++] = str[i];
	buf[j] = 0;

	return buf;
}

int __ll_is_spec(char *str) {
	char *specs[] = {"func", ";", "?", NULL};

	for (int i = 0; specs[i]; i++)
		if (!strcmp(specs[i], str))
			return i;
	return -1;
}

void ll_process_spec_operators(list *node) {
	token *tk = NULL;
	char *gist = NULL;

	// extract token
	tk = (token*)node->data;
	if (!tk)
		return ;

	// extract gist
	gist = __ll_remove_spaces(tk->val);

	// set marked index
	if (__ll_is_spec(gist) > -1) {
		if (marked_index == -1)
			marked_index = tk->index;
		else
			tk->eval_me = 0;

		return ;
	}

	// check for marked index
	if (marked_index < 0) {
		tk->eval_me = 1;

		return ;
	}

	if (tk->index > marked_index)
		tk->eval_me = 0;
	else {
		tk->eval_me = 1;
		marked_index = -1;
	}
}

void ll_exec(list *node) {
	token *tk = NULL;
	var *vptr = NULL;
	func *fptr = NULL;
	char *gist = NULL;
	int res = 0;

	// extract token
	tk = (token*)node->data;
	if (!tk || !tk->eval_me)
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

	// check for function
	fptr = fl_func_get_with_syntax(func_scope, gist);
	if (fptr) {
		fl_func_call(fptr, node);
		//printf("yey!\n");

		return ;
	}

	ll_make_ret(node);
}

int ll_is_std(char *name) {
	char *names[] = {"print", "help", ";", "+", "-", "*", "/", "let", "func", \
		"poly", "?", ">", "<", ">=", "<=", "=", "!=", NULL};
	int i = 0;

	for (; names[i]; i++)
		if (!strcmp(names[i], name))
			return i;
	return -1;
}

void ll_run_std(int code, list *node) {
	void (*std[])(list*) = {ll_cb_print, ll_cb_help, ll_cb_nil, ll_cb_sum, \
		ll_cb_sub, ll_cb_mul, ll_cb_div, ll_cb_let, ll_cb_func, ll_cb_poly, \
		ll_cb_if, ll_cb_big, ll_cb_small, ll_cb_big_eq, ll_cb_small_eq, \
		ll_cb_eq, ll_cb_not_eq};

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
		if (arg_tk->index < arg_col)
			break;
		else if (arg_tk->index > arg_col) {
			lptr = lptr->next;

			continue;
		}

		if (__ll_is_esc(arg_tk->ret))
			__ll_print_esc(arg_tk->ret);
		else
			printf("%s", arg_tk->ret);

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
		else if (arg_tk->index > arg_col) {
			lptr = lptr->next;

			continue;
		}
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
			scope_str = current_scope;
	}

	// main loop
	while (lptr) {
		name_tk = (token*)lptr->data;
		if (name_tk->index < arg_col)
			break;
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
			value = val_tk->ret;

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

void ll_cb_func(list *node) {
	list *lptr, *proto, *body;
	token *scope_tk, *head_tk, *proto_tk, *body_tk;
	char *scope_name;
	int arg_col = 0, tag = 0;

	// check for next
	lptr = node->next;
	if (!lptr)
		return ;
	
	// get arg_col
	head_tk = (token*)node->data;
	if (!head_tk)
		return ;
	arg_col = head_tk->index + 1;

	// check for scope name
	scope_tk = (token*)lptr->data;
	if (!scope_tk)
		return ;

	if (*scope_tk->val == ':') {
		scope_name = &(scope_tk->val[1]);

		lptr = lptr->next;
	}
	else
		scope_name = current_scope;

	// check scope
	tag = sl_scope_check(func_scope, scope_name);

	// get proto
	proto_tk = (token*)lptr->data;
	if (!proto_tk || proto_tk->index != arg_col)
		return ;
	proto = lptr;
	lptr = lptr->next;

	// get body
	body_tk = (token*)lptr->data;
	while (body_tk->index > arg_col) {
		lptr = lptr->next;

		body_tk = (token*)lptr->data;
	}
	body = lptr;
	body_tk = (token*)body->data;

	// add function
	fl_add_func(proto, body, tag);
}

void ll_cb_poly(list *node) {
	list *lptr = NULL;
	token *tk = NULL, *arg_tk = NULL, *targ_tk = NULL;
	char *ret = NULL;
	int indx = 0;

	// get indx
	tk = (token*)node->data;
	indx = tk->index + 1;

	// main loop
	lptr = node->next;
	while (lptr) {
		arg_tk = (token*)lptr->data;
		if (!arg_tk) {
			lptr = lptr->next;

			continue;
		}

		// check indx
		if (arg_tk->index == indx)
			targ_tk = arg_tk;

		lptr = lptr->next;
	}

	// check for null
	if (!targ_tk) {
		fprintf(stderr, "forp: poly can\'t find last tree\n");

		exit(0);
	}

	// get ret
	ret = targ_tk->ret;
	if (!ret)
		ret = "0";

	// check for ret
	if (!tk->ret)
		tk->ret = (char*)malloc(strlen(ret) + 1);
	else
		tk->ret = (char*)realloc(tk->ret, strlen(ret) + 1);
	
	// copy
	strcpy(tk->ret, ret);
}

void __ll_exec_list(list *lptr, int level) {
	marked_index = -1;

	tl_crawl_list(lptr, ll_process_spec_operators);
	tl_crawl_list_level(lptr, level, ll_exec);
}

void ll_cb_if(list *node) {
	list *condition, *body, *dest_ptr, *lptr;
	token *arg_tk, *tk, *cond_tk;
	int arg_col = 0;

	// get arg_col
	tk = (token*)node->data;
	if (!tk) {
		fprintf(stderr, "forp: if: can\'t get head\n");

		exit(0);
	}
	arg_col = tk->index + 1;

	// init lists
	condition = list_init_node(NULL);
	body = list_init_node(NULL);

	dest_ptr = condition;

	// get condition and body
	lptr = node->next;
	while (lptr) {
		arg_tk = (token*)lptr->data;
		if (!arg_tk) {
			lptr = lptr->next;

			continue;
		}

		if (arg_tk->index == arg_col) {
			fl_copy_tree_by_index(dest_ptr, lptr, arg_col);
			dest_ptr = body;
		}

		lptr = lptr->next;
	}

	// eval condition
	__ll_exec_list(condition, arg_col);

	// check condition
	cond_tk = (token*)condition->next->data;
	if (!cond_tk) {
		fprintf(stderr, "forp: if: can\'t get condition value\n");

		exit(0);
	}

	if (strcmp(cond_tk->ret, "0"))
		__ll_exec_list(body, arg_col);
}

double __ll_get_arg(list *lptr) {
	token *tk;

	tk = (token*)lptr->data;
	if (!tk) {
		fprintf(stderr, "forp: can\'t get argument\n");

		exit(0);
	}

	return atof(tk->ret);
}

char *__ll_cb_cmp_big(double a1, double a2) {
	return (a1 > a2) ? "1" : "0";
}

char *__ll_cb_cmp_small(double a1, double a2) {
	return (a1 < a2) ? "1" : "0";
}

char *__ll_cb_cmp_big_eq(double a1, double a2) {
	return (a1 >= a2) ? "1" : "0";
}

char *__ll_cb_cmp_small_eq(double a1, double a2) {
	return (a1 <= a2) ? "1" : "0";
}

char *__ll_cb_cmp_eq(double a1, double a2) {
	return (a1 == a2) ? "1" : "0";
}

char *__ll_cb_cmp_not_eq(double a1, double a2) {
	return (a1 != a2) ? "1" : "0";
}

void __ll_cb_cmp(list *node, char *(*func)(double, double)) {
	token *tk;
	double arg1, arg2;
	char *res;

	// get argiments
	arg1 = __ll_get_arg(node->next);
	arg2 = __ll_get_arg(node->next->next);

	// get res
	res = (*func)(arg1, arg2);

	// set ret
	tk = node->data;
	if (!tk) {
		fprintf(stderr, "forp: can\'t get head\n");

		exit(0);
	}

	// alloc ret
	if (!tk->ret)
		tk->ret = (char*)malloc(strlen(res) + 1);
	else
		tk->ret = (char*)realloc(tk->ret, strlen(res) + 1);

	// check ret
	if (!tk->ret) {
		fprintf(stderr, "forp: can\'t allocate return field\n");

		exit(0);
	}

	// copy
	strcpy(tk->ret, res);
}

void ll_cb_big(list *node) {
	__ll_cb_cmp(node, __ll_cb_cmp_big);
}

void ll_cb_small(list *node) {
	__ll_cb_cmp(node, __ll_cb_cmp_small);
}

void ll_cb_big_eq(list *node) {
	__ll_cb_cmp(node, __ll_cb_cmp_big_eq);
}

void ll_cb_small_eq(list *node) {
	__ll_cb_cmp(node, __ll_cb_cmp_small_eq);
}

void ll_cb_eq(list *node) {
	__ll_cb_cmp(node, __ll_cb_cmp_eq);
}

void ll_cb_not_eq(list *node) {
	__ll_cb_cmp(node, __ll_cb_cmp_not_eq);
}
