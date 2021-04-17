#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tl2/list.h"
#include "pars.h"
#include "token.h"
#include "lib.h"

int main(int argc, char *argv[]) {
	list prog = {NULL, NULL, NULL};

	if (argc < 2) {
		printf("Usage: %s <filename>\n", argv[0]);

		return 0;
	}

	// pars file
	pl_pars_file(&prog, argv[1]);

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
	tl_crawl_list(node, ll_exec);

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

void pl_pars_file(list *tree, char *file_name) {
	FILE *file = fopen(file_name, "r");
	char line[256] = "";

	if (!file) {
		perror("forp");

		exit(0);
	}

	fgets(line, 256, file);
	while (!feof(file)) {
		if ((strlen(line) - 1) > 0) { // + \n
			pl_pars_line(tree, line);
		}

		fgets(line, 256, file);
	}
	pl_exec(tree);

	fclose(file);
}
