SERVICE_PREFIX=ysf_boot@
SERVICE_SUFIX=.service


all: install
	@systemctl restart $(SERVICE_PREFIX)feifei
	@if [ -e ./out ]; then cat ./out; fi

.PHONY:install
install:
	install -D -m 0644 $(SERVICE_PREFIX)$(SERVICE_SUFIX) /lib/systemd/system/ 
	@systemctl daemon-reload

.PHONY:uninstall
uninstall:
	rm /lib/systemd/system/$(SERVICE_PREFIX)$(SERVICE_SUFIX)
	@systemctl daemon-reload
