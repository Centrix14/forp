#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tl2/list.h"
#include "pars.h"
#include "token.h"
#include "scope.h"
#include "var.h"
#include "func.h"
#include "lib.h"

list *var_scope;
list *func_scope;

int main(int argc, char *argv[]) {
	list prog = {NULL, NULL, NULL};
	FILE *stream = NULL;

	// init vars and var_scope
	var_scope = sl_scope_list_init();
	vl_var_list_init();

	// init funcs and func_scope
	func_scope = sl_scope_list_init();
	fl_func_list_init();

	// add inital scope - global
	sl_scope_new(var_scope, "global");
	sl_scope_new(func_scope, "global");

	if (argc >= 2) {
		stream = fopen(argv[1], "r");

		// pars file
		pl_pars_stream(&prog, stream);

		// close
		fclose(stream);
	}
	else {
		pl_print_welcome();
		pl_pars_stream(&prog, stdin);
	}

	// free programm buffer
	tl_crawl_list(&prog, tl_free_token);
	tl_free_list(&prog);

	// free var_scope and variables
	vl_var_list_free();
	sl_scope_free(var_scope);

	// free func_scope and funcs
	fl_func_list_free();
	sl_scope_free(func_scope);

	return 0;
}

int pl_get_index(char *str) {
	int i = 0;

	while (str[i] == '\t')
		i++;

	return i;
}

char *pl_get_token(char *str) {
	static char buf[256] = "";
	int j = 0;

	for (int i = 0; str[i]; i++)
		if (str[i] != '\t' && str[i] != '\n')
			buf[j++] = str[i];
	buf[j] = 0;

	return buf;
}

void pl_exec(list *node) {
	// show old tree
	//tl_crawl_list(node, tl_show);
	//puts(""); // just for new line

	// run list
	tl_crawl_list(node, ll_process_spec_operators);
	tl_crawl_list_reverse(node, ll_exec);

	// free old tree
	tl_crawl_list(node, tl_free_token);
	tl_free_list(node);
}

void pl_pars_line(list *tree, char *line) {
	char *tok = NULL;
	int indx = 0;

	indx = pl_get_index(line);
	tok = pl_get_token(line);
	if (!indx)
		pl_exec(tree);
	tl_add_token(tree, tok, indx);
}

void pl_pars_stream(list *tree, FILE *file) {
	char line[256] = "";

	if (!file) {
		perror("forp");

		exit(0);
	}

	// command line welcome
	pl_prompt(file);

	fgets(line, 256, file);
	while (!feof(file)) {
		if (!strncmp(line, "exit", 4))
			exit(0);
		else if ((strlen(line) - 1) > 0) { // + \n
			pl_pars_line(tree, line);
		}

		// command line welcome
		pl_prompt(file);

		fgets(line, 256, file);
	}
	pl_exec(tree);
}

void pl_print_welcome() {
	printf("Welcome to FORP interactive interpreter!\n"\
		   "Use `;` to perform the previous operation\n"\
		   "Use Ctrl-C or `q` to exit\nEnjoy!\n\n");
}

void pl_prompt(FILE *stream) {
	if (stream == stdin)
		printf("| ");
}
