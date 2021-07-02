sleep 1
t=1
n=1
while true; do
    if [ $t -eq 1 ];then
        color="#ff0000"
    else
        color="#0000ff"
    fi
    echo '{"color":"'$color'"}'
    n=$((n+1))
    ((t^=1))
    sleep 1
done
