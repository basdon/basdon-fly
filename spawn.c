
/* vim: set filetype=c ts=8 noexpandtab: */

#define _CRT_SECURE_NO_DEPRECATE
#include "common.h"
#include "dialog.h"
#include "spawn.h"
#include <string.h>

struct SPAWN *spawns = NULL;
int numspawns = 0;

/**
The text to show in the spawn list dialog when spawning.

May be NULL.
*/
char *spawn_list_text = NULL;

void spawn_init()
{
	struct SPAWN *spawn;
	char *b;
	int i;

	spawn = spawns;
	b = malloc(sizeof(char) * numspawns * (2 + MAX_AIRPORT_NAME));
	spawn_list_text = b;
	i = 0;

	while (i < numspawns) {
		b += sprintf(b, "%s\n", spawn->name);
		i++;
		spawn++;
	}
	*b = 0;
}

void spawn_dispose()
{
	if (spawns != NULL) {
		free(spawns);
		spawns = NULL;
		numspawns = 0;
	}
	if (spawn_list_text != NULL) {
		free(spawn_list_text);
		spawn_list_text = NULL;
	}
}

void spawn_on_dialog_response(AMX *amx, int playerid, int response, int idx)
{
	float x, y, z, r;
	if (response && 0 <= idx && idx < numspawns) {
		x = (spawns + idx)->x;
		y = (spawns + idx)->y;
		z = (spawns + idx)->z;
		r = (spawns + idx)->r;
		common_tp_player(amx, playerid, x, y, z, r);
	}
}

void spawn_on_player_spawn(AMX *amx, int playerid)
{
	int randomspawn;
	float x = 1477.4771f, y = 1244.7747f, z = 10.8281f, r = 0.0f;
	if (numspawns > 0) {
		dialog_NC_ShowPlayerDialog(
			amx, playerid, DIALOG_SPAWN_SELECTION,
			DIALOG_STYLE_LIST, "Spawn selection", spawn_list_text,
			"Spawn", "Cancel", -1);
		NC_random_(numspawns, &randomspawn);
		x = (spawns + randomspawn)->x;
		y = (spawns + randomspawn)->y;
		z = (spawns + randomspawn)->z;
		r = (spawns + randomspawn)->r;
	}
	common_tp_player(amx, playerid, x, y, z, r);
}
