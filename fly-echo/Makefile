.PHONY: clean
default: out/mod_bas_echo.jar

out/mod_bas_echo.jar: $(wildcard src/annamod/*.java) $(wildcard src/net/basdon/fly/services/echo/*.java)
	@[ -n "$(ANNA_DIR)" ] || (echo "please set env var \$$ANNA_DIR to anna dir" && exit 1)
	@[ -f "$(ANNA_DIR)/out/anna.jar" ] || (echo "building anna" && cd "$(ANNA_DIR)" && make .anna)
	@mkdir -p out
	javac -g -d out -cp "$(ANNA_DIR)/anna/out" $(filter-out $<,$^)
	jar cfM out/mod_bas_echo.jar -C out .
	cp out/mod_bas_echo.jar "$(ANNA_DIR)/out"

clean:
	rm -r out/*
