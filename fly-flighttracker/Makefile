JAVAC=javac
JFLAGS=-g# -Xlint:all

default: .flighttracker

outdirs:
	mkdir -p out

.flighttracker: outdirs $(wildcard src/annamod/*.java) $(wildcard src/net/basdon/fly/services/flighttracker/*.java)
	[ -n "$(ANNA_DIR)" ] || (echo please set env var ANNA_DIR to anna dir && exit 1)
	[ -f "$(ANNA_DIR)/out/anna.jar" ] || (cd "$(ANNA_DIR)" && make .anna)
	$(JAVAC) $(JFLAGS) -d out -cp "$(ANNA_DIR)/anna/out" $(filter-out $<,$^)
	jar cfM out/mod_bas_ft.jar -C out .
	cp out/mod_bas_ft.jar "$(ANNA_DIR)/out"

clean:
	rm -r out/*

