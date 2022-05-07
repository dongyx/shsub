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

To install `shsub`, execute the following command in the source tree:

	$ sudo make install

It installs `shsub` to `/usr/local` which is the most common location
for local programs in Unix variants.

Installing `shsub` to the home directory is more complicated,
because the structure of the home directory is quite a personal
taste. Supposing your personal executable files are placed in
`~/bin` and your personal man pages are placed in `~/man`,
the following setting would be a common way to install `shsub` to
the home directory.

	$ make install prefix=~/.shsub bindir=~/bin mandir=~/man

It installs the executable files in `~/bin`, man pages in `~/man`,
and any other supporting files of `shsub` would be installed to
`~/.shsub`.

USAGE
-----

`shsub` \[*options*\] \[*file*\]

If *file* is omitted, read from stdin.

Type `man shsub` for detail.
