.PHONY: all install test clean lint fuzz

name		=	shsub
prefix		=	/usr/local
bindir		=	$(prefix)/bin
datarootdir	=	$(prefix)/share
mandir		=	$(datarootdir)/man
INSTALL		=	install
CC		=	cc

all: shsub

install: all
	name='$(name)' \
	version=$$(./shsub --version | head -n1 | awk '{print $$2}') \
	./shsub shsub.1.tpl >shsub.1
	mkdir -p $(bindir) $(mandir)/man1
	$(INSTALL) shsub $(bindir)/$(name)
	$(INSTALL) -m644 shsub.1 $(mandir)/man1/$(name).1

shsub: shsub.c
	$(CC) $(CFLAGS) -o $@ $<

test: all
	chmod +x test
	./test

clean:
	rm -rf shsub shsub.1 *.dSYM lint fuzz

lint:
	@rm -rf lint
	@mkdir -p lint
	@cp Makefile *.c lint
	@cd lint && make 'CFLAGS= \
	-std=c89 \
	-D_POSIX_C_SOURCE=200809L \
	-pedantic-errors \
	-Wextra \
	-Wno-error'

fuzz:
	@rm -rf fuzz
	@mkdir -p fuzz
	@cp -r Makefile *.c cases test fuzz
	@cd fuzz && make test \
	'CFLAGS= \
	-std=c89 \
	-D_POSIX_C_SOURCE=200809L \
	-pedantic-errors \
	-Wextra \
	-Wno-error \
	-O3 \
	-fno-sanitize-recover \
	-fsanitize=address,undefined'
