/**
Amount of spawns per class index.
*/
static int numspawns[SETTING__NUM_CLASSES];
/**
Array of spawn locations per class index.

Elements may be NULL if associated value in numspawns is zero.
*/
static struct vec4 *spawns[SETTING__NUM_CLASSES];
/**
The text to show in the spawn list dialog when spawning, per class index.

Elements may be NULL if associated value in numspawns is zero.
*/
static char *spawn_list_text[SETTING__NUM_CLASSES];

/**
Loads spawn locations from database, creates spawn list texts.

Must run after airports_init
*/
static
void spawn_init()
{
	int querycacheid, row, rows, ap, klass = SETTING__NUM_CLASSES;
	int *field = nc_params + 2;
	struct vec4 *sp;
	char *txt;

	while (klass--) {
		csprintf(buf144,
			"SELECT ap,sx,sy,sz,sr "
			"FROM spw "
			"JOIN apt ON spw.ap=apt.i "
			"WHERE apt.e AND (class&%d)",
			1 << klass);
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
		logprintf("ERR: no spawns for classid 0, spawn fallback will be incorrect!");
	}
}

/**
Frees memory.
*/
static
void spawn_dispose()
{
	int i = SETTING__NUM_CLASSES;
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

static
void spawn_cb_dlg_spawn(int playerid, struct DIALOG_RESPONSE response)
{
	int klass;

	klass = classid[playerid];
	if (response.response && 0 <= response.listitem && response.listitem < numspawns[klass]) {
		common_tp_player(playerid, spawns[klass][response.listitem]);
	}
}

/**
Set player spawn info before a player spawns.

To be called from OnPlayerRequestClass and OnPlayerDeath.

Otherwise players get teleported right after spawn, causing updates like map
streaming and zones twice right after each other.
*/
static
void spawn_prespawn(int playerid)
{
	int spawnidx, klass = classid[playerid];

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

/**
Call on spawn to show a dialog of spawn locations to teleport to.

spawn_prespawn should've been called before, which should've set the player's
spawninfo before spawning, so player should already have spawned at a location
of their choice (as set in preferences) by this time.
*/
static
void spawn_on_player_spawn(int playerid)
{
	struct DIALOG_INFO dialog;
	int klass;

	klass = classid[playerid];
	/*TODO: spawn preference*/
	if (numspawns[klass] > 1) {
		dialog_init_info(&dialog);
		dialog.transactionid = DLG_TID_SPAWN;
		dialog.style = DIALOG_STYLE_LIST;
		dialog.caption = "Spawn selection";
		dialog.info = spawn_list_text[klass];
		dialog.button1 = "Spawn";
		dialog.button2 = "Cancel";
		dialog.handler.callback = spawn_cb_dlg_spawn;
		dialog_show(playerid, &dialog);
	}

	if (NC_GetPlayerScore(playerid) < 20) {
		SendClientMessage(playerid, COL_INFO_LIGHT, INFO"Use /reclass to change to a different class.");
		SendClientMessage(playerid, COL_INFO_LIGHT, INFO"Use /respawn to go to a different spawn place.");
		SendClientMessage(playerid, COL_INFO_LIGHT, INFO"Use /help to learn more about the features of this server.");
	}
}
