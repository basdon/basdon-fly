
SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c, out/%.o, $(SOURCES))
HEADERS := $(wildcard *.h)

FLAGS=-Wall -m32 -std=c89

build: $(OBJECTS) out/amxplugin.o
	$(GCC) $(FLAGS) -shared -o out/basdonfly.$(PLUGINTYPE) out/amxplugin.o $(OBJECTS)

out/amxplugin.o: vendor/SDK/plugincommon.h vendor/SDK/amxplugin.c
	$(GCC) $(FLAGS) -x c -c vendor/SDK/amxplugin.c -o out/amxplugin.o

out/basdonfly.o: basdon.c
out/cmdbase.o: cmdhandlers.c

out/%.o: %.c $(HEADERS)
	$(GCC) $(FLAGS) -x c -c $< -o $@

clean:
	rm out/*

