#ifndef __FORK_PROCESSOR_LANGUAGE_TOKEN_LIBRARY_FILE_H_INCLUDED__
#define __FORK_PROCESSOR_LANGUAGE_TOKEN_LIBRARY_FILE_H_INCLUDED__

typedef struct {
	char *val;
	char *ret;

	int index;
} token;

void tl_add_token(list *prog, char *tok, int index);
void tl_show(list *prog);

void tl_free_token(list *prog);
void tl_free_list(list *prog);

void tl_crawl_list(list *prog, void (*func)(list*));
void tl_crawl_list_reverse(list *prog, void (*func)(list*));

#endif
