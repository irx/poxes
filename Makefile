.PHONY: clean examples all

PREFIX = /usr/local
MANPREFIX = ${PREFIX}/man

CC = cc
CFLAGS = -std=c99 -pedantic -Wall \
	 -Wno-deprecated-declarations

OBJ = poxes.o
HDR = poxes.h

all: libpoxes.a

install: all
	@echo installing to ${PREFIX}
	cp libpoxes.a ${PREFIX}/lib/libpoxes.a
	cp poxes.h ${PREFIX}/include/poxes.h
	cp poxes.3 ${MANPREFIX}/man3/poxes.3

libpoxes.a: ${OBJ}
	@echo AR $@
	@ar -rc $@ ${OBJ}

.c.o: ${HDR}
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

clean:
	rm libpoxes.a ${OBJ}
