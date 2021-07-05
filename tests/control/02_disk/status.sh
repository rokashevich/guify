sleep 1
t=1
n=1
while true; do
	((t^=1))
	if [ $t -eq 1 ]
	then
		label=111111111111
	else
		label=0
	fi
	n=$((n+1))
	echo '{"active":'$t', "label":"'$n$label'"}'
	sleep 1
done
