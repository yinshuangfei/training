all:
	gcc -static sleep.c -o sleep.elf


show_section:
	readelf -S sleep.elf


show_segment:
	readelf -l sleep.elf


proc:
	@if ps -ef | grep "sleep.elf" | grep -v grep 1>/dev/null; then echo 'running ...' ; else ./sleep.elf & fi
	@pid=`ps -ef | grep "sleep.elf" | grep -v grep | awk '{print $$2}'`; cat /proc/$${pid}/maps
