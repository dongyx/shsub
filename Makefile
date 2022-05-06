.PHONY: all install test clean

version		= 0.0.0
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
	mkdir -p testenv; \
	for in in test/*.in; do \
		bname=`basename $${in%%.*}`; \
		echo running test $${bname}...; \
		out=testenv/$${bname}.out; \
		ans=test/$${bname}.out; \
		<$$in ./shsub >$$out; \
		diff -u $$ans $$out; \
	done; \
	echo all tests passed

shsub: shsub.sh version usage LICENSE
	m4 \
		-D__version__='$(version)' \
		-D__license__="`cat LICENSE`" \
		-D__usage__="`cat usage`" \
		shsub.sh > shsub
	chmod +x shsub

tc: tc.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf shsub tc testenv
