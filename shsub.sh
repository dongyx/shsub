#!/bin/sh

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
