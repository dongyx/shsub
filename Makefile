.PHONY: all install test clean

name		=	shsub
prefix		=	/usr/local
bindir		=	$(prefix)/bin
datarootdir	=	$(prefix)/share
mandir		=	$(datarootdir)/man
INSTALL		=	install
CC		=	cc

all: shsub

install: all
	name='$(name)' ./shsub shsub.1.tpl >shsub.1
	mkdir -p $(bindir) $(mandir)/man1
	$(INSTALL) shsub $(bindir)/$(name)
	$(INSTALL) -m644 shsub.1 $(mandir)/man1/$(name).1

shsub: shsub.c
	$(CC) $(CFLAGS) -o $@ $<

test: all
	chmod +x test
	./test

clean:
	rm -rf shsub shsub.1 *.dSYM
