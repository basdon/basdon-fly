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

static
void spawn_get_random_spawn(int playerid, struct SpawnInfo *outSpawnInfo)
{
	int spawnidx = 0;
	int klass = classid[playerid];

	switch (numspawns[klass]) {
	/*if no spawns, take first spawn of pilot class*/
	case 0: klass = 0;
	case 1: spawnidx = 0; break;
	default: spawnidx = NC_random(numspawns[klass]); break;
	}

	outSpawnInfo->team = 11; /*using anything but NO_TEAM should make players not able to damage other players, except by slitting throat when having a knife*/
	outSpawnInfo->skin = CLASS_SKINS[klass];
	outSpawnInfo->_pad5 = 69;
	memcpy(&outSpawnInfo->pos, spawns[klass] + spawnidx, sizeof(struct vec4));
	outSpawnInfo->weapon[0] = WEAPON_CAMERA;
	outSpawnInfo->weapon[1] = 0;
	outSpawnInfo->weapon[2] = 0;
	outSpawnInfo->ammo[0] = 3036; /*one clip is 36, that leaves an extra clean 3000*/
	outSpawnInfo->ammo[1] = 0;
	outSpawnInfo->ammo[2] = 0;
}

/**
Call on spawn to show a dialog of spawn locations to teleport to.

Player should already be spawned in their preferred location, because we should
set spawn info before they spawn so we don't need to teleport players after they
spawn because that would otherwise cause zone/object streaming headaches. But we
still show the spawn dialog after spawning in case the player wants to override
the spawn that they currently got.
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

	if (GetPlayerScore(playerid) < 20) {
		SendClientMessage(playerid, COL_INFO_LIGHT, INFO"Use /reclass to change to a different class.");
		SendClientMessage(playerid, COL_INFO_LIGHT, INFO"Use /respawn to go to a different spawn place.");
		SendClientMessage(playerid, COL_INFO_LIGHT, INFO"Use /help to learn more about the features of this server.");
	}
}
