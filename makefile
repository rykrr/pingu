CC = gcc

.PHONY: compile install uninstall enable disable

compile: pingu pingu_server

pingu: pingu.c
	$(CC) -o $@ $<

pingu_server: pingu_server.c
	$(CC) -o $@ $<

install: pingu pingu_server
	install -m555 pingu /usr/local/bin/pingu
	install -m555 pingu_server /usr/local/bin/pingu_server

enable: install
	install -m555 pingu.service /etc/systemd/system/pingu.service
	systemctl enable pingu
	systemctl start pingu

disable:
	systemctl disable pingu
	systemctl stop pingu
	rm -f /etc/systemd/system/pingu.service

uninstall: disable
	rm -f /usr/local/bin/pingu /usr/local/bin/pingu_server

