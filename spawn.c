
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
	if (response && 0 <= idx && idx < numspawns) {
		common_tp_player(amx, playerid,
			(spawns + idx)->pos, (spawns + idx)->r);
	}
}

void spawn_on_player_spawn(AMX *amx, int playerid)
{
	int randomspawn;
	struct vec3 pos;
	float r;
	if (numspawns > 0) {
		dialog_NC_ShowPlayerDialog(
			amx, playerid, DIALOG_SPAWN_SELECTION,
			DIALOG_STYLE_LIST, "Spawn selection", spawn_list_text,
			"Spawn", "Cancel", -1);
		NC_random_(numspawns, &randomspawn);
		pos = (spawns + randomspawn)->pos;
		r = (spawns + randomspawn)->r;
	} else {
		/*lv spawn*/
		pos.x = 1320.41f;
		pos.y = 1268.25f;
		pos.z = 10.8203f;
		r = 0.0f;
	}
	common_tp_player(amx, playerid, pos, r);
}
