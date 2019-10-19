
/* vim: set filetype=c ts=8 noexpandtab: */

#define _CRT_SECURE_NO_DEPRECATE
#include "common.h"
#include "airport.h"
#include "class.h"
#include "dialog.h"
#include <string.h>

struct SPAWN {
	/**
	Spawn name, this should be the airport name.
	*/
	char *name;
	struct vec4 pos;
};

/**
Amount of spawns per class.
*/
static int numspawns[numclasses];
/**
Array of spawn locations per class

Elements may be NULL if associated value in numspawns is zero.
*/
static struct SPAWN *spawns[numclasses];
/**
The text to show in the spawn list dialog when spawning, per class.

Elements may be NULL is associated value in numspawns is zero.
*/
static char *spawn_list_text[numclasses];

/**
Loads spawn locations from database, creates spawn list texts.

Must run after airports_init
*/
void spawn_init(AMX *amx)
{
	int querycacheid, row, rows, ap, klass = numclasses;
	struct SPAWN *sp;
	char buf[144], *txt;

	while (klass--) {
		sprintf(buf,
			"SELECT ap,sx,sy,sz,sr "
			"FROM spw "
			"JOIN apt ON spw.ap=apt.i "
			"WHERE apt.e AND (class&%d)", CLASSMAPPING[klass]);
		amx_SetUString(buf144, buf, 144);
		NC_mysql_query_(buf144a, &querycacheid);
		NC_cache_get_row_count_(&rows);
		numspawns[klass] = rows;
		if (!rows) {
			spawns[klass] = NULL;
			spawn_list_text[klass] = NULL;
			goto nospawns;
		}
		sp = malloc(sizeof(struct SPAWN) * rows);
		txt = malloc(sizeof(char) * rows * (1 + MAX_AIRPORT_NAME));
		spawns[klass] = sp;
		spawn_list_text[klass] = txt;
		row = 0;
		while (row != rows) {
			NC_cache_get_field_int(row, 0, &ap);
			NC_cache_get_field_flt(row, 1, sp->pos.coords.x);
			NC_cache_get_field_flt(row, 2, sp->pos.coords.y);
			NC_cache_get_field_flt(row, 3, sp->pos.coords.z);
			NC_cache_get_field_flt(row, 4, sp->pos.r);
			sp->name = (airports + ap)->name;
			txt += sprintf(txt, "%s\n", sp->name);
			sp++;
			row++;
		}
		*txt = 0;
nospawns:
		NC_cache_delete(querycacheid);
	}
}

void spawn_dispose()
{
	int i = numclasses;
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

void spawn_on_dialog_response(AMX *amx, int playerid, int response, int idx)
{
	int klass = classidx[playerid];

	if (response && 0 <= idx && idx < numspawns[klass]) {
		common_tp_player(amx, playerid, (spawns[klass] + idx)->pos);
	}
}

void spawn_on_player_spawn(AMX *amx, int playerid)
{
	int klass = classidx[playerid];
	int randomspawn;
	struct vec4 pos;

	if (numspawns[klass] == 1) {
		pos = spawns[klass]->pos;
	} else if (numspawns[klass] > 0) {
		dialog_NC_ShowPlayerDialog(
			amx, playerid, DIALOG_SPAWN_SELECTION,
			DIALOG_STYLE_LIST, "Spawn selection",
			spawn_list_text[klass],
			"Spawn", "Cancel", -1);
		NC_random_(numspawns[klass], &randomspawn);
		pos = (spawns[klass] + randomspawn)->pos;
	} else {
		/*lv spawn*/
		pos.coords.x = 1320.41f;
		pos.coords.y = 1268.25f;
		pos.coords.z = 10.8203f;
		pos.r = 0.0f;
	}
	common_tp_player(amx, playerid, pos);
}
