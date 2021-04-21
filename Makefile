all:
	gcc -Wall -O0 -g -o forp pars.c token.c lib.c var.c tl2/list.c
