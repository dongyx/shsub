.TH <%="$name"%> 1

.SH NAME

<%="$name"%> - compile the template to a shell script and execute it

.SH SYNOPSIS

\fB<%="$name"%>\fR [\fIoptions\fR] [\fIfile\fR\] [\fIarguments\fR]

\fB<%="$name"%>\fR \fB--help\fR

\fB<%="$name"%>\fR \fB--version\fR

.SH DESCRIPTION

\fB<%="$name"%>\fR reads the template from \fIfile\fR,
or, by default, the standard input.

A template is text
with embedded shell commands and expressions.
Shell commands are surrounded by \fB<%%\fR and \fB%%>\fR.
Shell expressions are surrounded by \fB<%%=\fR and \fB%%>\fR.
Shell commands are compiled to the commands themselves.
Each \fB<%%=\fIexpr\fB%%>\fR is compiled to \fBprintf %s \fIexpr\fR.
Ordinary text is compiled to the command printing that text.
Contents within \fB<%%!\fR and \fB%%>\fR are regarded as comments
and are ignored.
A template can include other templates by \fB<%%+\fIfilename\fB%%>\fR.

If \fB-%%>\fR is used instead of \fB%%>\fR
to end a command, expression, including, or comment,
the following newline character will be ignored.
\fB<%%%\fR and \fB%%%>\fR are compiled to literal \fB<%%\fR and \fB%%>\fR.

\fIArguments\fR are passed to the compiled script while execution.

.SH OPTIONS

.TP
\fB\-s\fR \fIpath\fR
Set which shell is used to execute the script
(default: \fB/bin/sh\fR)

.TP
.B \-c
Compile the template without execution

.TP
\fB\-o\fR \fIpath\fR
Set the path of the output script if \fB-c\fR is enabled

.SH AUTHORS

Shsub was created by
DONG Yuxuan <https://www.dyx.name> in 2022.

.SH FURTHER DOCUMENTATION

See <https://github.com/dongyx/shsub>.
