shsub
=====

*shsub* is a template engine of the shell language,
but mainly implemented in C.

The following file `lsnotes.html.st` is a simple shell template:

	<ul>
		<% for i in notes/*.md; do -%>
			<% title=`grep '^# ' "$i" | head -n1` -%>
			<% if [ -n "$title" ]; then -%>
				<li>
					<%= $title %>
				</li>
			<% fi -%>
		<% done -%>
	</ul>

Running `shsub lsnotes.html.st` prints a HTML list reporting titles of
your Markdown notes.

TEMPLATE SYNTAX
---------------

- if the first line of the shell template begins with `#!` (*shebang*),
it will be ignored

- `<%`*cmd*`%>` is substituted with the output of *cmd*

- `<%=`*expr*`%>` is substituted with the output of the command
`printf %s "`*expr*`"`. 

	Leading and trailing spaces, tabs, and newlines of *expr*
	are removed.
	Double quotes in *expr* are automatically escaped.

- `-%>` can be used instead of `%>` to trim the following newline

- `<%%` and `%%>` are escape tokens representing literal `<%` and `%>`

INSTALLATION
------------

Download the source tarball of the latest version from
the [release list](https://github.com/dongyx/shsub/releases).
Unpack the tarball and
execute the following commands in the source tree:

	make test
	sudo make install

*shsub* is installed to `/usr/local` by default.

Building *shsub* requires:

- `cc`(1): any C compiler which is compatible with `clang` or `gcc`

USAGE
-----

`shsub` \[*options*\] \[*file*\]

If *file* is omitted, `shsub` reads the template from stdin.

options:

- `-s` *sh*	specify the shell, `/bin/sh` by default
- `-h`	print the brief usage
- `-v`	print the version information

The detailed information is documented in the man page `shsub`(1)
which is included in the installation.
You could read it anytime by typing `man shsub`.

UNINSTALLATION
--------------

For default installation in `/usr/local`:

	sudo rm /usr/local/bin/shsub
	sudo rm -r /usr/local/libexec/shsub/
	sudo rm -r /usr/local/share/shsub/
	sudo rm /usr/local/share/man/man1/shsub.1
