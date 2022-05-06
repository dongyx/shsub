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
	mkdir -p testenv; \
	for in in test/*.in; do \
		bname=`basename $${in%%.*}`; \
		echo running test $${bname}...; \
		out=testenv/$${bname}.out; \
		err=testenv/$${bname}.err; \
		ansout=test/$${bname}.out; \
		anserr=test/$${bname}.err; \
		<$$in ./shsub >$$out 2>$$err; \
		[ -f $$ansout ] && diff -u $$ansout $$out; \
		[ -f $$anserr ] && diff -u $$anserr $$err; \
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
