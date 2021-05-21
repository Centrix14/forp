CC = gcc

DB_FLAGS = -Wall -O0 -g
CFLAGS = -o
OPT_FLAGS = -O2

RESULT = forp
LIBS = pars.o token.o lib.o var.o scope.o func.o list.o

all: pars token lib var scope func list build

build:
	$(CC) $(DB_FLAGS) $(CFLAGS) $(RESULT) $(LIBS)

build-rel:
	$(CC) $(OPT_FLAGS) $(CFLAGS) $(RESULT) $(LIBS)

test:
	./$(RESULT) samples/hello-world.frp
	./$(RESULT) samples/math.frp
	./$(RESULT) samples/variables.frp
	./$(RESULT) samples/functions.frp
	./$(RESULT) samples/if.frp

pars:
	$(CC) $(DB_FLAGS) -c pars.c

token:
	$(CC) $(DB_FLAGS) -c token.c

lib:
	$(CC) $(DB_FLAGS) -c lib.c

var:
	$(CC) $(DB_FLAGS) -c var.c

scope:
	$(CC) $(DB_FLAGS) -c scope.c

func:
	$(CC) $(DB_FLAGS) -c func.c

list:
	$(CC) $(DB_FLAGS) -c tl2/list.c
