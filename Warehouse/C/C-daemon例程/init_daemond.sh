#!/bin/bash

### BEGIN INIT INFO 
#this is a daemon function of RAID for LED display.
### END INIT INFO

if [ -f /etc/init.d/functions ]
then 
	. /etc/init.d/functions
else
	. /lib/lsb/init-functions
fi

NAME=LED_Daemon
DAEMON=/usr/sbin/init_daemond
LOCKFILE=/var/lock/subsys/$DAEMON
PIDFILE=/var/run/$NAME.pid

start(){
	echo -n "开始守护进程:" $NAME
	start-stop-daemon --start --quiet --pidfile $PIDFILE --exec $DAEMON
	echo "."
}

stop(){
	echo -n "终止守护进程: " $NAME
	if	pidof $DAEMON > /dev/null;	then	
		killall -9 $DAEMON > /dev/null
		rm -rf $PIDFILE
		echo "服务已停止!"
	fi
}

status(){
	if	pidof -o %PPID $DAEMON > /dev/null; 	then
		echo $NAME "running"
		exit 0
	else
		echo $NAME "not running"
		exit 1
	fi
}

case "$1" in 
start)
	start
	;;
stop)
	stop
	;;
status)
	status
	;;
*)
	echo $"Usage: $0 {start|stop|status}"
	exit 1
esac	
	
	
	
	
	

