#ifndef __FORK_PROCESSOR_LANGUAGE_PARSER_LIBRARY_FILE_H_INCLUDED__
#define __FORK_PROCESSOR_LANGUAGE_PARSER_LIBRARY_FILE_H_INCLUDED__

int pl_get_index(char *str);
void pl_pars_file(list *tree, char *file_name);
void pl_pars_line(list *tree, char *line);

#endif
