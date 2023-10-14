center()
{
	local w
	local p
	local l

	w=$1
	while read l; do
		p=$(((w - ${#l})/2))
		while [ $p -gt 0 ]; do
			printf ' '
			p=$((p-1))
		done
		printf %s\\n "$l"
	done
}
