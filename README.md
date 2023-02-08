Shsub
=====

*Shsub* is a template engine of the shell language,
implemented in C and Shell.

The following file `lsnotes.html.st` is a simple shell template:

	<ul>
	<% for i in notes/*.md; do -%>
	<% 	title=`grep '^# ' "$i" | head -n1` -%>
	<% 	if [ -n "$title" ]; then -%>
		<li>
			<%=$title %>
		</li>
	<%	fi -%>
	<% done -%>
	</ul>

Running `shsub lsnotes.html.st` prints a HTML list reporting titles of
your Markdown notes.

Template Syntax
---------------

The `shsub` program compiles the template to a shell script and execute it.

- if the first line of the shell template begins with `#!` (*shebang*),
it will be ignored

- `<%`*cmd*`%>` is compiled to *cmd*

- `<%=`*expr*`%>` is compiled to `printf %s "`*expr*`"`

	Leading and trailing spaces, tabs, and newlines of *expr*
	are removed.
	Double quotes in *expr* are automatically escaped.

- `-%>` can be used instead of `%>` to ignore the following newline

- `<%%` and `%%>` are escaping tokens representing literal `<%` and `%>`

- ordinary text is compiled to the command that prints the text

Installation
------------

Download the source tarball of the latest version from
the [release list](https://github.com/dongyx/shsub/releases).
Unpack the tarball and
execute the following commands in the source tree:

	make test
	sudo make install

Shsub is installed to `/usr/local` by default.

Building Shsub requires:

- `cc`(1): any C compiler which is compatible with `clang` or `gcc`

Usage
-----

`shsub` \[*options*\] \[*file*\] \[*argument* ...\]

If *file* is `-` or omitted, `shsub` reads the template from stdin.

options:

- `-s` *sh*	specify the shell to execute the compiled script,
	`/bin/sh` by default
- `-h`	print the brief usage
- `-v`	print the version information

The detailed information is documented in the man page `shsub`(1)
which is included in the installation.
You could read it anytime by typing `man shsub`.

Uninstallation
--------------

For default installation in `/usr/local`:

	sudo rm /usr/local/bin/shsub
	sudo rm -r /usr/local/lib/shsub/
	sudo rm /usr/local/share/man/man1/shsub.1
