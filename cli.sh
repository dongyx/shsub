#!/bin/sh

# the user interface of shsub <https:/github.com/dongyx/shsub>
# piping the output of tc to shell

set -e
dir=`dirname -- "$0"`
version="$dir/version"
license="$dir/LICENSE"
tc="$dir/tc"

help() {
cat <<\.
usage: shsub [<options>] [<file>]
	if <file> is omitted, read from stdin.

options:
	-s <sh>	specify the shell to execute the compiled script,
		`/bin/sh' by default
	-h	print the brief usage
	-v	print the version information
.
}

# shell escape
shesc() {
	printf %s "$1" | sed "s/'/'\\\\''/g; 1 s/^/'/; $ s/$/'/"
}

preproc() {
	awk 'NR == 1 && /^#!/ { next } 1'	# ignore shebang
}

sh=/bin/sh
while getopts 's:hv' opt; do
	case $opt in
		s)	sh="$OPTARG";;
		h)	help
			exit;;
		v)	echo shsub "`cat "$version"`"
			cat "$license"
			exit;;
		?)	help >&2
			exit 1;;
	esac
done
shift $(($OPTIND - 1))

script="$(mktemp)"
trap '[ -f "$script" ] && rm "$script"' EXIT
trap 'exit 1' TERM INT HUP
if [ $# -gt 0 ]; then
	printf '%s\n' progname="`shesc "$1"`" >>"$script"
	<"$1" preproc | "$tc" >>"$script"
	shift
else
	preproc | "$tc" >>"$script"
fi
$sh "$script" "$@"
