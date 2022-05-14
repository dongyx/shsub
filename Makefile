.PHONY: all install test clean

prefix		= /usr/local
exec_prefix	= $(prefix)
bindir		= $(exec_prefix)/bin
libexecdir	= $(exec_prefix)/libexec
datarootdir	= $(prefix)/share
mandir		= $(datarootdir)/man

CC		= cc
CFLAGS		= 	-ansi \
			-D_POSIX_C_SOURCE=200809L \
			-pedantic-errors \
			-Wno-error=all
INSTALL		= install

all: cli tc

install: all
	m4 -D__libexecdir__=`echo $(libexecdir)` shsub.sh > shsub
	$(INSTALL) -d $(bindir) $(libexecdir)/shsub $(mandir)/man1
	$(INSTALL) cli tc $(libexecdir)/shsub/
	$(INSTALL) -m644 shsub.1 $(mandir)/man1/
	$(INSTALL) shsub $(bindir)/

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

cli: cli.sh usage LICENSE version
	m4	-D__version__="`cat version`" \
		-D__license__="`cat LICENSE`" \
		-D__usage__="`cat usage`" \
		cli.sh > cli
	chmod +x cli

tc: tc.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf cli shsub tc testenv *.dSYM
