SHELL=bash
.PHONY: echo flighttracker plugin web clean
default: echo flighttracker plugin web

echo:
	@([ -z "$(ANNA_DIR)" ] && echo "WARN: not building fly-echo because \$$ANNA_DIR is not set") ||\
	([ -z "$$(which javac)" ] && echo "WARN: not building fly-echo because javac is not found") ||\
	(cd fly-echo && make)

flighttracker:
	@([ -z "$(ANNA_DIR)" ] && echo "WARN: not building fly-flighttracker because \$$ANNA_DIR is not set") ||\
	([ -z "$$(which javac)" ] && echo "WARN: not building fly-flighttracker because javac is not found") ||\
	(cd fly-flighttracker && make)

plugin:
	@(cd fly-plugin && . build)

web:
	@(cd fly-web && make)

clean:
	cd fly-echo && make clean
	cd fly-flighttracker && make clean
	rm -f fly-plugin/out/*
	cd fly-web && make clean
	rm -f server/server.cfg
