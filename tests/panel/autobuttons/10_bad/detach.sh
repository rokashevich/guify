#!/bin/bash
socket_file=/tmp/socket_channel
current_status=`echo "STATUS"|nc -U $socket_file -q 1|grep STATUS=|tail -1`
if [ "$current_status" == "STATUS=1" ]
then
    echo "STATUS=0"|nc -U $socket_file
else
    echo "STATUS=1"|nc -U $socket_file
fi
