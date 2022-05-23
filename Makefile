.PHONY: all install test clean

prefix		=	/usr/local
exec_prefix	=	$(prefix)
bindir		=	$(exec_prefix)/bin
libexecdir	=	$(exec_prefix)/libexec
datarootdir	=	$(prefix)/share
datadir		=	$(prefix)/share
mandir		=	$(datarootdir)/man
INSTALL		=	install
CC		=	cc
CFLAGS		= 	-std=c89 \
			-D_POSIX_C_SOURCE=200809L \
			-pedantic-errors \
			-Wno-error=all

all: cli tc

install: all
	$(INSTALL) -d \
		$(bindir) \
		$(libexecdir)/shsub \
		$(datadir)/shsub \
		$(mandir)/man1
	$(INSTALL) cli tc $(libexecdir)/shsub/
	>$(bindir)/shsub
	>>$(bindir)/shsub echo '#!/bin/sh'
	>>$(bindir)/shsub echo export libexecdir=$(libexecdir)
	>>$(bindir)/shsub echo export datadir=$(datadir)
	>>$(bindir)/shsub echo exec $(libexecdir)/shsub/cli '"$$@"'
	chmod 755 $(bindir)/shsub
	$(INSTALL) -m644 usage version LICENSE $(datadir)/shsub/
	$(INSTALL) -m644 shsub.1 $(mandir)/man1/

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

cli: cli.sh
	cp $< $@
	chmod +x $@

tc: tc.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf cli tc testenv *.dSYM
