#!/bin/bash -posix

# the user interface of shsub <https:/github.com/dongyx/shsub>
# piping the output of tc to shell

set -e
dir=`dirname -- "$0"`
usage="$dir/usage"
version="$dir/version"
license="$dir/LICENSE"
tc="$dir/tc"
if [ -n "$datadir" ]; then
	usage="$datadir/shsub/usage"
	version="$datadir/shsub/version"
	license="$datadir/shsub/license"
fi
if [ -n "$libexecdir" ]; then
	tc="$libexecdir/shsub/tc"
fi

# shell escape
shesc() {
	printf %s "$1" | sed "s/'/'\\\\''/g; 1 s/^/'/; $ s/$/'/"
}

preproc() {
	awk 'NR == 1 && /^#!/ { next } 1'	# ignore shebang
}

killchd() {
	ps -Aopid,ppid |
	awk 'NR > 1 && $2 == "'$$'" { print $1 }' |
	xargs kill 2>/dev/null
	wait
}

sh=/bin/sh
while getopts 's:hv' opt; do
	case $opt in
		s)	sh="$OPTARG";;
		h)	cat "$usage"; exit;;
		v)	echo shsub "`cat "$version"`";
			cat "$license";
			exit;;
		?)	echo type \`shsub -h\' for the brief usage
			echo 'read the man page shsub(1) for detail'
			exit 1;;
	esac
done
shift $(($OPTIND - 1))
trap "killchd; exit $((128 + 15))" TERM
trap "killchd; exit $((128 + 2))" INT
trap "killchd; exit $((128 + 1))" HUP
exec 3>&0
if [ $# -gt 0 ]; then
	fifo="$(mktemp -u)"
	mkfifo -m600 "$fifo"
	trap '[ -p "$fifo" ] && rm "$fifo"' EXIT
	{ printf 'set -e\n'
	printf '%s\n' progname="`shesc "$1"`"
	<"$1" preproc | "$tc" >>"$fifo"; } > "$fifo" &
	shift
	0>&3 "$sh" "$fifo" "$@" &
else
	0>&3 preproc | "$tc" | "$sh" &
fi
wait
