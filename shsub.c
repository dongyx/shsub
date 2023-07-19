#include <sys/stat.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#define MAXLITR 4096
#define MAXINCL 64

enum token {
	INCL,
	CMDOPEN,
	EXPOPEN,
	CLOSE,
	ESCOPEN,
	ESCCLOSE,
	LITERAL,
	END
};

struct iframe {
	FILE *in;
	int lookahead, cstack[3], *csp, lineno;
	char *tmplname;
} istack[MAXINCL], *isp = istack;

char *progname, *tmplname, script[] = "/tmp/shsub.XXXXXX";
enum token lookahead;
int cstack[3], *csp = cstack, lineno = 1;
char literal[MAXLITR];

enum token gettoken(FILE *fp);
int cpush(int c);
int cpop(FILE *fp);
void tmpl(FILE *in, FILE *ou);
void text(int esc, FILE *in, FILE *ou);
void match(enum token tok, FILE *fp);
void ipush(FILE *in);
FILE *ipop(void);
void rmscr(void);
void version(void);
void help(void);
void err(char *fmt, ...);
void parserr(char *fmt, ...);
void syserr(void);
void verr(char *fmt, va_list ap);

int main(int argc, char **argv)
{
	FILE *in, *out;
	char *sh = "/bin/sh", *target = NULL;
	int fd, op, noexe = 0;

	progname = argv[0];
	if (argc > 1) {
		if (!strcmp(argv[1], "--version"))
			version();
		if (!strcmp(argv[1], "--help"))
			help();
	}
	while ((op = getopt(argc, argv, "s:co:")) != -1)
		switch (op) {
		case 's': sh = optarg; break;
		case 'c': noexe = 1; break;
		case 'o': target = optarg; break;
		case '?': err("Call with `--help` for usage");
		}
	argc -= optind;
	argv += optind;
	in = stdin;
	if (argc > 0) {
		tmplname = argv[0];
		if (!(in = fopen(tmplname = argv[0], "r")))
			err("%s: %s", tmplname, strerror(errno));
	}
	lookahead = gettoken(in);
	if (noexe) {
		if (target) {
			if ((fd = open(
				target,
				O_CREAT|O_WRONLY|O_TRUNC,
				0755
			)) == -1)
				syserr();
			if (!(out = fdopen(fd, "w")))
				syserr();
		} else
			out = stdout;
		fprintf(out, "#!%s\n", sh);
		tmpl(in, out);
		if (fclose(out) == EOF)
			syserr();
		return 0;
	}
	if ((fd = mkstemp(script)) == -1 || atexit(rmscr))
		syserr();
	if (!(out = fdopen(fd, "w")))
		syserr();
	fprintf(out, "#!%s\nrm %s\n", sh, script);
	tmpl(in, out);
	if (fchmod(fd, S_IRWXU) == -1)
		syserr();
	if (fclose(out))
		syserr();
	if (argc > 0)
		execv(script, argv);
	else
		execl(script, "-", NULL);
	syserr();
	return 0;
}

enum token gettoken(FILE *fp)
{
	char *p;
	int c, h, r, trim = 0;
	enum token kw = END;

	p = literal;
	while (kw == END) {
		while (p - literal < MAXLITR - 1) {
			if ((c = cpop(fp)) == EOF || strchr("<%-", c ))
				break;
			*p++ = c;
		}
		*p = '\0';
		if (c == EOF)
			return p > literal ? LITERAL : END;
		if (p == literal + MAXLITR - 1)
			return LITERAL;
		h = cpop(fp);
		r = cpop(fp);
		if (c == '<' && h == '%') {
			kw = CMDOPEN;
			if (r == '=')
				kw = EXPOPEN;
			else if (r == '+')
				kw = INCL;
			else if (r == '%')
				kw = ESCOPEN;
		} else if (c == '%' && h == '>')
			kw = CLOSE;
		else if (c == '%' && h == '%' && r == '>')
			kw = ESCCLOSE;
		else if (c == '-' && h == '%' && r == '>') {
			trim = 1;
			kw = CLOSE;
		} else {
			cpush(r);
			cpush(h);
			*p++ = c;
			*p = '\0';
		}
	}
	if (p > literal) {
		cpush(r);
		cpush(h);
		cpush(c);
		return LITERAL;
	}
	if (kw == CMDOPEN || kw == CLOSE && !trim)
		cpush(r);
	if (trim && (c = cpop(fp)) != '\n')
		cpush(c);
	return kw;
}

int cpush(int c)
{
	if (c == '\n')
		--lineno;
	return *csp++ = c;
}

int cpop(FILE *fp)
{
	int c;

	if (csp == cstack)
		c = fgetc(fp);
	else
		c = *--csp;
	if (c == '\n') {
		if (lineno == INT_MAX)
			err("Too many lines");
		++lineno;
	}
	return c;
}

void tmpl(FILE *in, FILE *ou)
{
	while (lookahead != END)
		switch (lookahead) {
		case INCL:
			match(INCL, in);
			if (lookahead != LITERAL)
				parserr("Expect included filename");
			if (!(tmplname = strdup(literal)))
				syserr();
			match(LITERAL, in);
			match(CLOSE, in);
			ipush(in);
			if (!(in = fopen(tmplname, "r")))
				err("%s: %s",
					tmplname, strerror(errno));
			lineno = 1;
			csp = cstack;
			lookahead = gettoken(in);
			tmpl(in, ou);
			fclose(in);
			free(tmplname);
			in = ipop();
			break;
		case CMDOPEN:
			match(CMDOPEN, in);
			text(0, in, ou);
			fputc('\n', ou);
			match(CLOSE, in);
			break;
		case EXPOPEN:
			fputs("printf %s ", ou);
			match(EXPOPEN, in);
			text(0, in, ou);
			fputc('\n', ou);
			match(CLOSE, in);
			break;
		case LITERAL: case ESCOPEN: case ESCCLOSE:
			fputs("printf %s '", ou);
			text(1, in, ou);
			fputs("'\n", ou);
			break;
		default:
			parserr("Unexpected token");
		}
}

void text(int esc, FILE *in, FILE *ou)
{
	char *s;

	for (;;)
		switch(lookahead) {
		case LITERAL:
			if (!esc)
				fputs(literal, ou);
			else
				for (s = literal; *s; ++s)
					if (*s == '\'')
						fputs("'\\''", ou);
					else
						fputc(*s, ou);
			match(LITERAL, in);
			break;
		case ESCOPEN:
			fputs("<%", ou);
			match(ESCOPEN, in);
			break;
		case ESCCLOSE:
			fputs("%>", ou);
			match(ESCCLOSE, in);
			break;
		default:
			return;
		}
}

void match(enum token tok, FILE *fp)
{
	if (lookahead != tok)
		parserr("Lack of expected token");
	lookahead = gettoken(fp);
}

void ipush(FILE *in)
{
	if (isp == istack + MAXINCL)
		err("Including too deep");
	isp->lookahead = lookahead;
	memcpy(isp->cstack, cstack, sizeof cstack);
	isp->csp = csp;
	isp->in = in;
	isp->tmplname = tmplname;
	isp->lineno = lineno;
	++isp;
}

FILE *ipop(void)
{
	--isp;
	lookahead = isp->lookahead;
	memcpy(cstack, isp->cstack, sizeof cstack);
	csp = isp->csp;
	tmplname = isp->tmplname;
	lineno = isp->lineno;
	return isp->in;
}

void rmscr(void)
{
	unlink(script);
}

void version(void)
{
	fputs(
	"Shsub 2.0.0\n"
	"Project Homepage: <https://github.com/dongyx/shsub>\n"
	"Copyright (c) 2022 DONG Yuxuan <https://www.dyx.name>\n"
	, stdout);
	exit(0);
}

void help(void)
{
	printf(
	"USAGE\n"
	"	%s [OPTIONS] [FILE] [ARGUMENTS]\n"
	"	%s --help\n"
	"	%s --version\n"
	"OPTIONS\n"
	"	-s PATH	Specify the shell\n"
	"		(default: /bin/sh)\n"
	"	-c	Compile without execution\n"
	"	-o PATH	Set the path of the output script\n"
	"		(default: the standard output)\n"
	"FURTHER DOCUMENTATION\n"
	"	See the man page\n"
	"	See <https://github.com/dongyx/shsub>\n"
	, progname, progname, progname);
	exit(0);
}

void err(char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "%s: ", progname);
	va_start(ap, fmt);
	verr(fmt, ap);
}

void parserr(char *fmt, ...)
{
	va_list ap;

	if (tmplname)
		fprintf(stderr,"%s: %s: line %d: ",
			progname, tmplname, lineno);
	else
		fprintf(stderr, "%s: line %d: ", progname, lineno);
	va_start(ap, fmt);
	verr(fmt, ap);
}

void syserr(void)
{
	perror(progname);
	exit(-1);
}

void verr(char *fmt, va_list ap)
{
	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);
	exit(-1);
}
