#!/bin/sh

# the user interface of shsub <https:/github.com/dongyx/shsub>
# piping the output of tc to shell

set -e
dir=`dirname -- "$0"`
usage="$dir/usage"
version="$dir/version"
license="$dir/LICENSE"
tc="$dir/tc"

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
		h)	cat "$usage";
			exit;;
		v)	echo shsub "`cat "$version"`";
			cat "$license";
			exit;;
		?)	echo type \`shsub -h\' for the brief usage
			echo 'read the man page shsub(1) for detail'
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
