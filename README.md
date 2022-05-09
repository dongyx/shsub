shsub
=====

The shsub utility is a template engine using the shell language.

The following file `lsnotes.html.st` is a simple shell template:

	<ul>
		<%for i in notes/*.md; do%>
			<%title=`grep '^# ' "$i" | head -n1`%>
			<%if [ -n "$title" ]; then%>
				<li>
					<%=$title%>
				</li>
			<%fi%>
		<%done%>
	</ul>

Running `shsub lsnotes.html.st` prints a HTML list reporting titles of
your Markdown notes.

INSTALLATION
------------

Execute the following commands in the source tree:

	$ make
	$ sudo make install

It installs `shsub` to `/usr/local` by default.

USAGE
-----

`shsub` \[*options*\] \[*file*\]

If *file* is omitted, read from stdin.

Type `man shsub` for detail.
