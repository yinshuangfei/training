#!/bin/bash



show(){
	echo "[info] data file content:"
	od -A x -t x1z -v data

	echo "[info] data file content:"
	od -A x -t x1z -N 120 /dev/sdd
	
}

write(){
	dd if=data of=/dev/sdd
}

$@
