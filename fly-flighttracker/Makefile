.PHONY: clean anna
default: out/mod_bas_ft.jar

SOURCES = src/annamod/Mod.java $(wildcard src/net/basdon/fly/services/flighttracker/*.java)

anna:
	@[ -n "$(ANNA_DIR)" ] || (echo "please set env var \$$ANNA_DIR to anna dir" && exit 1)
	cd "$(ANNA_DIR)" && make anna

out/mod_bas_ft.jar: anna $(SOURCES)
	@mkdir -p out
	javac -g -d out -cp "$(ANNA_DIR)/anna/out" $(SOURCES)
	jar cfM out/mod_bas_ft.jar -C out .
	cp out/mod_bas_ft.jar "$(ANNA_DIR)/out"

clean:
	rm -r out/*
