#!/bin/bash 


case $1 in
    start)
    echo "start"
    for((;;))
    do
        time=`date`
        echo $time >> /root/ysf/date.txt
        sleep 3
    done
    ;;
    stop)
    echo "stop"
    pkill run.sh
    ;;
    *)
    ;;
esac
