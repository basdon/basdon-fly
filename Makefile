
FLAGS=-Wall -m32 -std=c89 -ansi

build: out/basdonfly.o out/amxplugin.o
	$(GCC) $(FLAGS) -shared -o out/basdonfly.$(PLUGINTYPE) out/basdonfly.o out/amxplugin.o

out/amxplugin.o: vendor/SDK/plugincommon.h vendor/SDK/amxplugin.c
	$(GCC) $(FLAGS) -x c -c vendor/SDK/amxplugin.c -o out/amxplugin.o

out/basdonfly.o: basdonfly.c
	$(GCC) $(FLAGS) -x c -c basdonfly.c -o out/basdonfly.o

clean:
	rm out/*

