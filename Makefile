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

install: shsub tc
	mkdir -p $(bindir)
	mkdir -p $(libexecdir)/shsub
	mkdir -p $(mandir)/man1
	$(INSTALL) shsub $(bindir)/
	$(INSTALL) tc $(libexecdir)/shsub/
	cp shsub.1 $(mandir)/man1/

test: prefix = testenv
test: install
	@set -e; \
	for in in test/*.in; do \
		bname=`basename $${in%%.*}`; \
		echo running test $${bname}...; \
		out=testenv/$${bname}.out; \
		ans=test/$${bname}.out; \
		<$$in testenv/bin/shsub >$$out; \
		diff -u $$ans $$out; \
	done; \
	echo all tests passed

shsub: shsub.m4 version usage LICENSE
	m4 \
		-Dm4_libexecdir=$(libexecdir) \
		-Dm4_version="`cat version`" \
		-Dm4_license="`cat LICENSE`" \
		-Dm4_usage="`cat usage`" \
		shsub.m4 > shsub

tc: tc.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf shsub tc testenv
