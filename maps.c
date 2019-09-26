
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"

struct OBJECT {
	int model;
	float x, y, z, rx, ry, rz;
};

struct MAP {
	int id;
	float radius_in, radius_out;
	char name[64];
	int numobjects;
	struct OBJECT *objects;
};

int nummaps;
struct MAP *maps;

void maps_load_from_db(AMX *amx)
{
	char q[] =
		"SELECT id,filename,radi,rado FROM map "
		"LEFT JOIN apt ON map.ap=apt.i "
		"WHERE apt.e=1 OR apt.e IS NULL";
	cell cacheid, rowcount;
	cell f1, f2;
	struct MAP *map;

	amx_SetUString(buf4096, q, sizeof(q));
	NC_mysql_query_(buf4096a, &cacheid);
	NC_cache_get_row_count_(&rowcount);
	nummaps = rowcount;
	if (rowcount) {
		maps = malloc(nummaps * sizeof(struct MAP));
		while (rowcount--) {
			map = maps + rowcount;
			NC_cache_get_row_int(rowcount, 0, (cell*) &map->id);
			NC_cache_get_row_str(rowcount, 1, buf32a);
			NC_cache_get_row_flt(rowcount, 2, &f1);
			NC_cache_get_row_flt(rowcount, 3, &f2);
			map->radius_in = amx_ctof(f1);
			map->radius_out = amx_ctof(f2);
			amx_GetUString(map->name, buf32, sizeof(map->name));
		}
	}
	NC_cache_delete(cacheid);
}

void maps_stream_for_player(int playerid)
{

}

/*
Go over players to see if maps need to be streamed for them
This function should be called from loop100
It checks 1+playercount/10 amount of players per call, so:

 playercount | checked players | ms between ticks
     1-9              1              100-900
    10-19             2              500-950
    20-29             3              700-966
   100-109           11	            1000-1000
 (not sure if this is correct; I'm too tired)
*/
void maps_process_tick()
{
	static int currentplayeridx = 0;
	int increment = 1 + playercount / 10;

	if (playercount > 0) {
		while (increment--) {
			if (++currentplayeridx >= playercount) {
				currentplayeridx = 0;
			}
			maps_stream_for_player(players[currentplayeridx]);
		}
	}
}
