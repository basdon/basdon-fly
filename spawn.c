
/* vim: set filetype=c ts=8 noexpandtab: */

#include "a_samp.h"
#include "game_sa.h"
#include "common.h"
#include "airport.h"
#include "class.h"
#include "dialog.h"
#include "spawn.h"
#include <string.h>

/**
Amount of spawns per class index.
*/
static int numspawns[NUMCLASSES];
/**
Array of spawn locations per class index.

Elements may be NULL if associated value in numspawns is zero.
*/
static struct vec4 *spawns[NUMCLASSES];
/**
The text to show in the spawn list dialog when spawning, per class index.

Elements may be NULL is associated value in numspawns is zero.
*/
static char *spawn_list_text[NUMCLASSES];

void spawn_init()
{
	int querycacheid, row, rows, ap, klass = NUMCLASSES;
	int *field = nc_params + 2;
	struct vec4 *sp;
	char buf[144], *txt;

	while (klass--) {
		sprintf(buf,
			"SELECT ap,sx,sy,sz,sr "
			"FROM spw "
			"JOIN apt ON spw.ap=apt.i "
			"WHERE apt.e AND (class&%d)", CLASSMAPPING[klass]);
		amx_SetUString(buf144, buf, 144);
		querycacheid = NC_mysql_query(buf144a);
		rows = NC_cache_get_row_count();
		numspawns[klass] = rows;
		if (!rows) {
			spawns[klass] = NULL;
			spawn_list_text[klass] = NULL;
			goto nospawns;
		}
		sp = malloc(sizeof(struct vec4) * rows);
		txt = malloc(sizeof(char) * rows * (1 + MAX_AIRPORT_NAME));
		spawns[klass] = sp;
		spawn_list_text[klass] = txt;
		row = 0;
		NC_PARS(2);
		while (row != rows) {
			nc_params[1] = row;
			ap = (*field = 0, NC(n_cache_get_field_i));
			sp->coords.x = (*field = 1, NCF(n_cache_get_field_f));
			sp->coords.y = (*field = 2, NCF(n_cache_get_field_f));
			sp->coords.z = (*field = 3, NCF(n_cache_get_field_f));
			sp->r = (*field = 4, NCF(n_cache_get_field_f));
			txt += sprintf(txt, "%s\n", (airports + ap)->name);
			sp++;
			row++;
		}
		*txt = 0;
nospawns:
		NC_cache_delete(querycacheid);
	}
	if (numspawns[0] == 0) {
		logprintf("ERR: no spawns for classid 0, spawn fallback will "
			"be incorrect!");
	}
}

void spawn_dispose()
{
	int i = NUMCLASSES;
	while (i--) {
		if (spawns[i]) {
			free(spawns[i]);
			spawns[i] = NULL;
		}
		if (spawn_list_text[i]) {
			free(spawn_list_text[i]);
			spawn_list_text[i] = NULL;
		}
	}
}

void spawn_on_dialog_response(int playerid, int response, int idx)
{
	int klass = classidx[playerid];

	if (response && 0 <= idx && idx < numspawns[klass]) {
		common_tp_player(playerid, spawns[klass][idx]);
	}
}

void spawn_prespawn(int playerid)
{
	int spawnidx, klass = classidx[playerid];

	switch (numspawns[klass]) {
	/*if no spawns, take first spawn of pilot class*/
	case 0: klass = 0;
	case 1: spawnidx = 0; break;
	default: spawnidx = NC_random(numspawns[klass]); break;
	}

	NC_PARS(13);
	nc_params[1] = playerid;
	nc_params[2] = NO_TEAM;
	nc_params[3] = CLASS_SKINS[klass];
	memcpy(nc_params + 4, spawns[klass] + spawnidx, sizeof(struct vec4));
	nc_params[8] = SPAWN_WEAPON_1;
	nc_params[9] = SPAWN_AMMO_1;
	nc_params[10] = nc_params[12] = SPAWN_WEAPON_2_3;
	nc_params[11] = nc_params[13] = SPAWN_AMMO_2_3;
	NC(n_SetSpawnInfo);
}

void spawn_on_player_spawn(int playerid)
{
	int klass = classidx[playerid];

	/*TODO: spawn preference*/
	if (numspawns[klass] > 1) {
		dialog_NC_ShowPlayerDialog(
			playerid, DIALOG_SPAWN_SELECTION,
			DIALOG_STYLE_LIST, "Spawn selection",
			spawn_list_text[klass],
			"Spawn", "Cancel", -1);
	}
}
