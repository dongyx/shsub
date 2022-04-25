#!/bin/sh

set -e
tc="m4_libexecdir/shsub/tc"
sh=sh

while getopts 's:hv' opt; do
	case $opt in
		s) sh="$OPTARG";;
		h)
			cat <<-'.'
			m4_usage
			.
			exit;;
		v)
			cat <<-'.'
			shsub m4_version

			m4_license
			.
			exit;;
	esac
done
"$tc" | "$sh"
