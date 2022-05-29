.PHONY: all install test clean

prefix		=	/usr/local
bindir		=	$(prefix)/bin
libdir		=	$(prefix)/lib
datarootdir	=	$(prefix)/share
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
		$(libdir)/shsub \
		$(mandir)/man1
	$(INSTALL) cli tc $(libdir)/shsub/
	$(INSTALL) -m644 version LICENSE $(libdir)/shsub/
	$(INSTALL) -m644 shsub.1 $(mandir)/man1/
	@echo append version information to manpage
	@( \
		printf "\n%s\n\n" ".SH VERSION"; \
		printf "shsub "; \
		cat version; \
		printf "\n"; \
		cat LICENSE; \
	) >>$(mandir)/man1/shsub.1
	@echo generate "`echo $(bindir)/shsub`"
	@>$(bindir)/shsub
	@>>$(bindir)/shsub echo '#!/bin/sh'
	@>>$(bindir)/shsub echo exec `echo $(libdir)/shsub/cli` '"$$@"'
	@chmod 755 $(bindir)/shsub

test: all
	@set -e; \
	rm -rf testenv testinst; \
	make install prefix="`pwd`/testinst"; \
	cp -R test testenv; \
	for t in testenv/*; do \
		echo running test $$t...; \
		( \
			cd $$t; \
			PATH="../../testinst/bin:$$PATH"; \
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
	rm -rf cli tc testenv testinst *.dSYM
