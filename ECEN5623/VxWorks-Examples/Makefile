INCLUDE_DIRS = 
LIB_DIRS = 

CDEFS=
CFLAGS= -O -g $(INCLUDE_DIRS) $(CDEFS) -DLINUX
LIBS=-lpthread -lrt

HFILES=
CFILES1= heap_mq.c 
CFILES2= posix_mq.c

SRCS1= ${HFILES} ${CFILES1}
SRCS2= ${HFILES} ${CFILES2}
OBJS1= ${CFILES1:.c=.o}
OBJS2= ${CFILES2:.c=.o}

all: heap_mq posix_mq

clean:
	-rm -f *.o *.d *.exe heap_mq posix_mq

heap_mq: heap_mq.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(OBJS1) $(LIBS)

posix_mq: posix_mq.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(OBJS2) $(LIBS)

depend:

.c.o:
	$(CC) -MD $(CFLAGS) -c $<
