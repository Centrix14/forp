#ifndef __FORK_PROCESSOR_LANGUAGE_PARSER_LIBRARY_FILE_H_INCLUDED__
#define __FORK_PROCESSOR_LANGUAGE_PARSER_LIBRARY_FILE_H_INCLUDED__

int pl_get_index(char *str);

void pl_pars_stream(list *tree, FILE *file);
void pl_pars_line(list *tree, char *line);

void pl_print_welcome();
void pl_prompt(FILE *stream);

#endif
