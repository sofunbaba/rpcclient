TARGET=github
SRC=$(wildcard *.c)

CFLAGS=-g -Wall
LDFLAGS=
LIBS=-lcurl -ljansson -lpthread

CROSS_COMPILE ?=
CC = ${CROSS_COMPILE}gcc

${TARGET}:${SRC}
	${CC} -o $@ ${CFLAGS} $^ ${LDFLAGS} ${LIBS}

.PHONY:clean

clean:
	$(RM) ${TARGET}
