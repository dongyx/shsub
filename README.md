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

INSTALLATION
------------

Download the source tarball of the preferred version from
the [release list](https://github.com/dongyx/shsub/releases).
Unpack the tarball and
execute the following commands in the source tree:

	$ make
	$ sudo make install

It installs `shsub` to `/usr/local` by default.

Building *shsub* requires:

- `cc`(1): any C compiler which is compatible with *clang* or *gcc*

- `m4`(1): the m4 macro processor

They are all shipped with most Unix variants
or can be installed from the package manager.

USAGE
-----

`shsub` \[*options*\] \[*file*\]

If *file* is omitted, `shsub` reads the template from stdin.

TEMPLATE SYNTAX
---------------

- `<%`*cmd*`%>` is substituted with the output of *cmd*

- `<%=`*expr*`%>` is substituted with the output of the command
`printf %s "`*expr*`"`. 

	Leading and trailing spaces, tabs, and newlines of *expr*
	are removed.
	Double quotes in *expr* are automatically escaped.

- `-%>` can be used instead of `%>` to trim the following newline

- `<%%` and `%%>` are escape tokens representing literal `<%` and `%>`

- if the first line of the shell template begins with `#!` (*shebang*),
it will be ignored
