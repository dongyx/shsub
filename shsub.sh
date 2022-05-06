#!/bin/sh
#
# the user interface of shsub <https:/github.com/dongyx/shsub>
# piping the output of tc to shell
#
# tokens surrounded by `__`, like `__version__`
# is intended to be substituted at building time
# using a macro processor like m4 .

set -e
tc="$(dirname "$0")/tc"
sh=/bin/sh

while getopts 's:hv' opt; do
	case $opt in
		s) sh="$OPTARG";;
		h)
			cat <<-'.'
			__usage__
			.
			exit;;
		v)
			cat <<-'.'
			shsub __version__

			__license__
			.
			exit;;
	esac
done
"$tc" | "$sh"
