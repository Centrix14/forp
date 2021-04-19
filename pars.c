#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tl2/list.h"
#include "pars.h"
#include "token.h"
#include "lib.h"

int main(int argc, char *argv[]) {
	list prog = {NULL, NULL, NULL};
	FILE *stream = NULL;

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

	// free list
	tl_crawl_list(&prog, tl_free_token);
	tl_free_list(&prog);

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
		if (!strcmp(line, "exit\n"))
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
		   "Use Ctrl-C or `exit` to exit\nEnjoy!\n\n");
}

void pl_prompt(FILE *stream) {
	if (stream == stdin)
		printf("| ");
}
