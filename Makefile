CC = gcc

DB_FLAGS = -Wall -O0 -g
CFLAGS = -o
OPT_FLAGS = -O2

RESULT = forp
FILES = pars.c token.c lib.c var.c scope.c func.c tl2/list.c

all:
	$(CC) $(DB_FLAGS) $(CFLAGS) $(RESULT) $(FILES)

rel:
	$(CC) $(OPT_FLAGS) $(CFLAGS) $(RESULT) $(FILES)

test:
	./$(RESULT) samples/hello-world.frp
	./$(RESULT) samples/math.frp
	./$(RESULT) samples/variables.frp
	./$(RESULT) samples/functions.frp
	./$(RESULT) samples/if.frp
