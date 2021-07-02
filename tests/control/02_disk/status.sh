sleep 1
t=7
n=1
while true; do
	echo '{"active":'$t', "label":"'$n'"}'
	n=$((n+1))
	((t^=1))
	sleep 1
done
