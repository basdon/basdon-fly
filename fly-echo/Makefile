.PHONY: clean anna
default: out/mod_bas_echo.jar

SOURCES = src/annamod/Mod.java $(wildcard src/net/basdon/fly/services/echo/*.java)

anna:
	@[ -n "$(ANNA_DIR)" ] || (echo "please set env var \$$ANNA_DIR to anna dir" && exit 1)
	cd "$(ANNA_DIR)" && make anna

out/mod_bas_echo.jar: anna $(SOURCES)
	@mkdir -p out
	@[ ! -f "out/mod_bas_echo.jar" ] || rm out/mod_bas_echo.jar
	javac -g -d out -cp "$(ANNA_DIR)/anna/out" $(SOURCES)
	jar cfM out/mod_bas_echo.jar -C out .
	cp out/mod_bas_echo.jar "$(ANNA_DIR)/out"

clean:
	rm -r out/*
