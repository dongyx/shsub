#include <sys/stat.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <getopt.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#define MAXTOKEN 4096
#define MAXINCL 64

enum token {
	INCL,
	CMDOPEN,
	EXPOPEN,
	COMOPEN,
	CLOSE,
	ESCOPEN,
	ESCCLOSE,
	LITERAL,
	END
};

struct iframe {
	FILE *in;
	char *tmplname;
	int cstack[3], *csp, lineno;
	enum token lookahead;
} istack[MAXINCL], *isp = istack;

char *progname, *tmplname, *script;
enum token lookahead;
int cstack[3], *csp = cstack, lineno = 1;
char token[MAXTOKEN];

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

int main(int argc, char **argv)
{
	char *sh = "/bin/sh", tmp[] = "/tmp/shsub.XXXXXX";
	int fd, op, exe = 1;
	FILE *in, *out;
	struct stat st;

	progname = argv[0];
	if (argc > 1) {
		if (!strcmp(argv[1], "--version"))
			version();
		if (!strcmp(argv[1], "--help"))
			help();
	}
	while ((op = getopt(argc, argv, "s:co:")) != -1)
		switch (op) {
		case 's':
			sh = optarg;
			break;
		case 'c':
			exe = 0;
			break;
		case 'o':
			script = optarg;
			break;
		default:
			err("Call with `--help` for usage");
		}
	argc -= optind;
	argv += optind;
	if (argc > 0) {
		tmplname = argv[0];
		if (!(in = fopen(tmplname = argv[0], "r")))
			err("%s: %s", tmplname, strerror(errno));
	} else
		in = stdin;
	if (exe) {
		if ((fd = mkstemp(script = tmp)) == -1)
			syserr();
		if (atexit(rmscr) == -1)
			syserr();
	} else if (script) {
		if ((fd = open(script, O_CREAT|O_WRONLY|O_TRUNC, 0744))
			== -1
		)
			syserr();
	} else
		fd = 1;
	if (fd != 1) {
		if (fstat(fd, &st) == -1)
			syserr();
		if (fchmod(fd, st.st_mode|0111) == -1)
			syserr();
		if (!(out = fdopen(fd, "w")))
			syserr();
	} else
		out = stdout;
	fprintf(out, "#!%s\n", sh);
	if (exe)
		fprintf(out, "rm %s\n", script);
	lookahead = gettoken(in);
	tmpl(in, out);
	if (fclose(out))
		syserr();
	if (exe) {
		if (argc > 0)
			execv(script, argv);
		else
			execl(script, "-", NULL);
		syserr();
	}
	return 0;
}

/* This routine also sets the `token` global variable.
 * If the token is `LITERAL`, `token` is set to the content.
 * Otherwise, `token` is set to the NULL-terminated empty string.
 */
enum token gettoken(FILE *fp)
{
	char *p;
	int c, h, r, trim = 0;
	enum token kw = END;

	p = token;
	while (kw == END) {
		while (p != token + MAXTOKEN - 1) {
			if ((c = cpop(fp)) == EOF || strchr("<%-", c ))
				break;
			*p++ = c;
		}
		*p = '\0';
		if (c == EOF)
			return p > token ? LITERAL : END;
		if (p == token + MAXTOKEN - 1)
			return LITERAL;
		h = cpop(fp);
		r = cpop(fp);
		if (c == '<' && h == '%') {
			if (r == '=')
				kw = EXPOPEN;
			else if (r == '+')
				kw = INCL;
			else if (r == '!')
				kw = COMOPEN;
			else if (r == '%')
				kw = ESCOPEN;
			else
				kw = CMDOPEN;
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
	if (p > token) {
		cpush(r);
		cpush(h);
		cpush(c);
		return LITERAL;
	}
	*p++ = c;
	*p++ = h;
	*p++ = r;
	/* push back a char if the token is a 2-char keyword */
	if (kw == CMDOPEN || (kw == CLOSE && !trim))
		cpush(*--p);
	*p = '\0';
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
	char incl[MAXTOKEN];

	while (lookahead != END)
		switch (lookahead) {
		case INCL:
			match(INCL, in);
			strcpy(incl, token);
			match(LITERAL, in);
			match(CLOSE, in);
			ipush(in);
			tmplname = incl;
			if (!(in = fopen(tmplname, "r")))
				err(
					"%s: %s",
					tmplname,
					strerror(errno)
				);
			lineno = 1;
			csp = cstack;
			lookahead = gettoken(in);
			tmpl(in, ou);
			fclose(in);
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
		case COMOPEN:
			match(COMOPEN, in);
			text(-1, in, ou);
			match(CLOSE, in);
			break;
		case LITERAL:
		case ESCOPEN:
		case ESCCLOSE:
			fputs("printf %s '", ou);
			text(1, in, ou);
			fputs("'\n", ou);
			break;
		default:
			parserr("Unexpected token: %s", token);
		}
}

void text(int esc, FILE *in, FILE *ou)
{
	char *s;

	for (;;)
		switch(lookahead) {
		case LITERAL:
			if (esc == 0)
				fputs(token, ou);
			else if (esc == 1)
				for (s = token; *s; ++s) {
					if (*s == '\'')
						fputs("'\\''", ou);
					else
						fputc(*s, ou);
				}
			match(LITERAL, in);
			break;
		case ESCOPEN:
			if (esc >= 0)
				fputs("<%", ou);
			match(ESCOPEN, in);
			break;
		case ESCCLOSE:
			if (esc >= 0)
				fputs("%>", ou);
			match(ESCCLOSE, in);
			break;
		default:
			return;
		}
}

void match(enum token tok, FILE *fp)
{
	if (lookahead != tok) {
		if (lookahead == END)
			parserr("Unexpected end");
		else
			parserr("Unexpected token: %s", token);
	}
	lookahead = gettoken(fp);
}

void ipush(FILE *in)
{
	if (isp == istack + MAXINCL)
		err("Too deep including");
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
	"Shsub 2.1.2 <https://github.com/dongyx/shsub>\n"
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
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
	exit(1);
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
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
	exit(1);
}

void syserr(void)
{
	perror(progname);
	exit(1);
}
