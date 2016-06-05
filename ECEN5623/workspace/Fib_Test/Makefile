INCLUDE_DIRS =
LIB_DIRS =
CC=gcc

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS=

HFILES=
CFILES= Fib_Test.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	Fib_Test

clean:
	-rm -f *.o *.d
	-rm -f Fib_Test

Fib_Test: Fib_Test.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o -lpthread

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<

