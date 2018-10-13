
HEADERS=common.h sharedsymbols.h
FLAGS=-Wall -m32 -std=c89 -ansi

build: out/basdonfly.o out/various.o out/panel.o out/amxplugin.o out/game_sa.o out/game_sa_data.o out/login.o out/commands.o out/dialog.o out/airport.o out/playerdata.o out/zones.o out/zones_data.o
	$(GCC) $(FLAGS) -shared -o out/basdonfly.$(PLUGINTYPE) out/basdonfly.o out/amxplugin.o out/various.o out/panel.o out/game_sa.o out/game_sa_data.o out/login.o out/commands.o out/dialog.o out/airport.o out/playerdata.o out/zones.o out/zones_data.o

out/amxplugin.o: vendor/SDK/plugincommon.h vendor/SDK/amxplugin.c
	$(GCC) $(FLAGS) -x c -c vendor/SDK/amxplugin.c -o out/amxplugin.o

out/basdonfly.o: basdonfly.c $(HEADERS)
	$(GCC) $(FLAGS) -x c -c basdonfly.c -o out/basdonfly.o

out/various.o: various.c $(HEADERS)
	$(GCC) $(FLAGS) -x c -c various.c -o out/various.o

out/panel.o: panel.c $(HEADERS)
	$(GCC) $(FLAGS) -x c -c panel.c -o out/panel.o

out/game_sa.o: game_sa.c $(HEADERS) game_sa.h
	$(GCC) $(FLAGS) -x c -c game_sa.c -o out/game_sa.o

out/game_sa_data.o: game_sa_data.c $(HEADERS) game_sa.h
	$(GCC) $(FLAGS) -x c -c game_sa_data.c -o out/game_sa_data.o

out/login.o: login.c $(HEADERS) playerdata.h
	$(GCC) $(FLAGS) -x c -c login.c -o out/login.o

out/commands.o: commands.c $(HEADERS)
	$(GCC) $(FLAGS) -x c -c commands.c -o out/commands.o

out/dialog.o: dialog.c $(HEADERS)
	$(GCC) $(FLAGS) -x c -c dialog.c -o out/dialog.o

out/airport.o: airport.c $(HEADERS)
	$(GCC) $(FLAGS) -x c -c airport.c -o out/airport.o

out/playerdata.o: playerdata.c $(HEADERS) playerdata.h
	$(GCC) $(FLAGS) -x c -c playerdata.c -o out/playerdata.o

out/zones.o: zones.c $(HEADERS) zones.h game_sa.h
	$(GCC) $(FLAGS) -x c -c zones.c -o out/zones.o

out/zones_data.o: zones_data.c $(HEADERS) zones.h
	$(GCC) $(FLAGS) -x c -c zones_data.c -o out/zones_data.o

clean:
	rm out/*

