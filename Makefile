.PHONY: all clean install

PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

CC = cc
CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE \
	 -Wno-deprecated-declarations

OBJ = poxes.o
HDR = poxes.h

all: libpoxes.a

install: all
	@echo installing to ${PREFIX}
	mkdir -p ${PREFIX}/lib
	cp libpoxes.a ${PREFIX}/lib/libpoxes.a
	cp poxes.h ${PREFIX}/include/poxes.h
	mkdir -p ${MANPREFIX}/man3
	cp poxes.3 ${MANPREFIX}/man3/poxes.3

libpoxes.a: ${OBJ}
	@echo AR $@
	@ar -rc $@ ${OBJ}

.c.o: ${HDR}
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

clean:
	rm libpoxes.a ${OBJ}
