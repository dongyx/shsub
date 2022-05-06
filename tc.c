/* tc - compile the shell template to a shell script
 *
 * the shell template is read from stdin
 * the shell script is printed to stdout
 *
 * tc is a component of the shsub project
 * <https://github.com/dongyx/shsub>
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define TOKSIZ 8

enum error { ESYS = 1, ESYNTAX };
enum token {
	OPENCMD,	/* <%	*/
	OPENEXP,	/* <%=	*/
	CLOSE,		/* %>	*/
	TRIMCLS,	/* -%>	*/
	ESCOPEN,	/* <%%	*/
	ESCCLS,		/* %%>	*/
	TXT,		/* text	*/
	END		/* EOF	*/
};
enum state { SERR, SINIT, STXT, SCMD, SEXP, STERM };
enum state sttab[][8] = {
/* in/st	  ERR	INIT	TXT	CMD	EXP	TERM	*/

/* OPENCMD */	{ SERR,	SCMD,	SCMD,	SERR,	SERR,	SERR	},
/* OPENEXP */	{ SERR,	SEXP,	SEXP,	SERR,	SERR,	SERR	},
/* CLOSE   */	{ SERR,	SERR,	SERR,	SINIT,	SINIT,	SERR	},
/* TRIMCLS */	{ SERR,	SERR,	SERR,	SINIT,	SINIT,	SERR	},
/* ESCOPEN */	{ SERR,	STXT,	STXT,	SCMD,	SEXP,	SERR	},
/* ESCCLS  */	{ SERR,	STXT,	STXT,	SCMD,	SEXP,	SERR	},
/* TXT     */	{ SERR,	STXT,	STXT,	SCMD,	SEXP,	SERR	},
/* END     */	{ SERR,	STERM,	STERM,	SERR,	SERR,	SERR	}
};
void entrtxt(enum state from, enum token in);
void entrcmd(enum state from, enum token in);
void entrexp(enum state from, enum token in);
void entrerr(enum state from, enum token in);
void exittxt(enum state to, enum token in);
void exitcmd(enum state to, enum token in);
void exitexp(enum state to, enum token in);
void trim(void);
void (*entrtab[])(enum state, enum token) = {
	&entrerr, NULL, &entrtxt, &entrcmd, &entrexp, NULL
};
void (*exittab[])(enum state, enum token) = {
	NULL, NULL, &exittxt, &exitcmd, &exitexp, NULL
};

int cstack[TOKSIZ], cssize;
int popc(void);
int pushc(int c);

char token[TOKSIZ];
enum token gettok(void);
int dryread(char *s);

main()
{
	enum state st, next;
	enum token in;

	puts("set -e");
	for (st = SINIT; st != STERM && st != SERR; st = next) {
		in = gettok();
		if (in == ESCOPEN)
			strcpy(token, "<%");
		if (in == ESCCLS)
			strcpy(token, "%>");
		next = sttab[in][st];
		if (exittab[st])
			exittab[st](next, in);
		if (entrtab[next])
			entrtab[next](st, in);
	}
	return 0;
}

void entrtxt(enum state from, enum token in)
{
	char *s;

	if (from != STXT)
		fputs("printf %s '", stdout);
	for (s = token; *s; s++)
		if (*s == '\'')
			fputs("'\\''", stdout);
		else
			putchar(*s);
}

void entrcmd(enum state from, enum token in)
{
	if (from == SCMD)
		fputs(token, stdout);
}

void entrexp(enum state from, enum token in)
{
	char *s;

	if (from != SEXP)
		fputs("printf %s \"", stdout);
	if (in == OPENEXP)
		return;
	for (s = token; *s; s++)
		if (*s == '"')
			fputs("\\\"", stdout);
		else
			putchar(*s);
}

void entrerr(enum state from, enum token in)
{
	fprintf(stderr, "illegal token: %s", token);
	exit(ESYNTAX);
}

void exittxt(enum state to, enum token in)
{
	if (to != STXT)
		puts("'");
}

void exitcmd(enum state to, enum token in)
{
	if (to == SCMD)
		return;
	if (in == TRIMCLS)
		trim();
	putchar('\n');
}

void exitexp(enum state to, enum token in)
{
	if (to == SEXP)
		return;
	putchar('"');
	if (in == TRIMCLS)
		trim();
	putchar('\n');
}

void trim(void)
{
	fputs("|awk 'NR>1{print p}{p=$0}END{printf(\"%s\",p)}'",
		stdout);
}

enum token gettok(void)
{
	static char *kwords[] = {
		"<%", "<%=", "%>", "-%>", "<%%", "%%>"
	};
	static int nkwords = sizeof kwords / sizeof kwords[0];
	int i, matched, len, maxlen;

	maxlen = 0;
	matched = -1;
	for (i = 0; i < nkwords; i++) {
		len = strlen(kwords[i]);
		if (len > maxlen && dryread(kwords[i])) {
			matched = i;
			maxlen = len;
		}
	}
	while (maxlen--)
		popc();
	if (matched >= 0) {
		strcpy(token, kwords[matched]);
		return matched;
	}
	token[0] = popc();
	token[1] = '\0';
	return token[0] == EOF ? END : TXT;
}

int pushc(int c)
{
	return cstack[cssize++] = c;
}

int popc(void)
{
	return cssize > 0 ? cstack[--cssize] : getchar();
}

int dryread(char *s)
{
	char *p, c;
	int ret;

	ret = 1;
	p = s;
	while (*p && (c = popc()) == *p)
		p++;
	if (*p) {
		ret = 0;
		pushc(c);
	}
	while (p != s)
		pushc(*--p);
	return ret;
}
