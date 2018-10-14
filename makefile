CC = gcc

compile: pingu pingu_server

pingu: pingu.c
	$(CC) -o $@ $<

pingu_server: pingu_server.c
	$(CC) -o $@ $<

install: pingu pingu_server
	install -m555 pingu /usr/local/bin/pingu
	install -m555 pingu_server /usr/local/bin/pingu_server
	install -m555 pingu.service /etc/systemd/system/pingu.service
	systemctl enable pingu
	systemctl start pingu

uninstall:
	rm -f /usr/local/bin/pingu /usr/local/bin/pingu_server
	systemctl disable pingu
	systemctl stop pingu
	rm -f /etc/systemd/system/pingu.service
