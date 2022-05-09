.PHONY: all install test clean

prefix		= /usr/local
exec_prefix	= $(prefix)
bindir		= $(exec_prefix)/bin
libexecdir	= $(exec_prefix)/libexec
datarootdir	= $(prefix)/share
mandir		= $(datarootdir)/man

CC		= cc
CFLAGS		= -ansi -pedantic-errors -Wno-error=all
INSTALL		= install

all: shsub tc

install: all
	mkdir -p $(bindir)
	mkdir -p $(libexecdir)/shsub
	mkdir -p $(mandir)/man1
	$(INSTALL) -m 755 shsub $(libexecdir)/shsub/
	$(INSTALL) -m 755 tc $(libexecdir)/shsub/
	cp shsub.1 $(mandir)/man1/
	echo '#!/bin/sh' > $(bindir)/shsub
	echo >> $(bindir)/shsub
	echo $(libexecdir)/shsub/shsub '"$$@"' >> $(bindir)/shsub
	chmod 755 $(bindir)/shsub

test: all
	@set -e; \
	rm -rf testenv; \
	cp -R test testenv; \
	for t in testenv/*; do \
		echo running test $$t...; \
		( \
			PATH="`pwd`:$$PATH"; \
			cd $$t; \
			chmod +x run; \
			./run \
		); \
	done; \
	echo all tests passed

shsub: shsub.sh usage LICENSE version
	m4 \
		-D__version__="`cat version`" \
		-D__license__="`cat LICENSE`" \
		-D__usage__="`cat usage`" \
		shsub.sh > shsub
	chmod +x shsub

tc: tc.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf shsub tc testenv
