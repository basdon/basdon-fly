
HEADERS=common.h sharedsymbols.h
FLAGS=-Wall -m32 -std=c89 -ansi

build: out/basdonfly.o out/various.o out/panel.o out/amxplugin.o
	$(GCC) $(FLAGS) -shared -o out/basdonfly.$(PLUGINTYPE) out/basdonfly.o out/amxplugin.o out/various.o out/panel.o

out/amxplugin.o: vendor/SDK/plugincommon.h vendor/SDK/amxplugin.c
	$(GCC) $(FLAGS) -x c -c vendor/SDK/amxplugin.c -o out/amxplugin.o

out/basdonfly.o: basdonfly.c $(HEADERS)
	$(GCC) $(FLAGS) -x c -c basdonfly.c -o out/basdonfly.o

out/various.o: various.c $(HEADERS)
	$(GCC) $(FLAGS) -x c -c various.c -o out/various.o

out/panel.o: panel.c $(HEADERS)
	$(GCC) $(FLAGS) -x c -c panel.c -o out/panel.o

clean:
	rm out/*

