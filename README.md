shsub
=====

*shsub* is a template engine of the shell language.

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

USAGE
-----

`shsub` \[*options*\] \[*file*\]

If *file* is omitted, `shsub` reads the template from stdin.
Complete options and the template syntax is documented in
the man page `shsub`(1).
