struct INFRACTIONDATA {
	/**
	One of AC_ constants.
	*/
	int eventtype;
	/**
	Amount by which to increment infraction value per infraction.
	*/
	int increment;
	/**
	Maximum allowed infraction value, player will be kicked when exceeding.
	*/
	int maxvalue;
	/**
	Amount by which to decrement infraction value every 5 seconds.
	*/
	int decrement_per_5000;
	/**
	Reason string used when kicking the player.
	*/
	char *kickreason;
};

static struct INFRACTIONDATA infractiondata[] = {
	/*AC_IF_DISALLOWED_VEHICLE*/
	{ AC_UNAUTH_VEHICLE_ACCESS, 3, 15, 1, "unauthorized vehicle access" },
};
static int infractionvalue[INFRACTIONTYPES][MAX_PLAYERS];
/**
Flood value of player.
*/
static int floodcount[MAX_PLAYERS];

/**
Adds infraction value, kicking the player if they exceeded the max value.

@param type infraction type, one of AC_IF_ constants.
*/
static
void anticheat_infraction(int playerid, int type)
{
	struct INFRACTIONDATA d = infractiondata[type];

	if ((infractionvalue[type][playerid] += d.increment) >= d.maxvalue) {
		natives_Kick(playerid, d.kickreason, NULL, -1);
	}
}

void anticheat_decrease_flood()
{
	int playerid, n = playercount;

	while (n--) {
		playerid = players[n];
		if (floodcount[playerid] > 0 &&
			(floodcount[playerid] -= AC_FLOOD_DECLINE_PER_100) < 0)
		{
			floodcount[playerid] = 0;
		}
	}
}

void anticheat_decrease_infractions()
{
	int playerid, j, i = playercount;
	int *iv;

	while (i--) {
		playerid = players[i];
		for (j = 0; j < INFRACTIONTYPES; j++) {
			iv = infractionvalue[j] + playerid;
			if (*iv > 0) {
				*iv -= infractiondata[j].decrement_per_5000;
				if (*iv < 0) {
					*iv = 0;
				}
			}
		}
	}
}

int anticheat_flood(int playerid, int amount)
{
	static char *EXCESS_FLOOD = "excess flood";

	if ((floodcount[playerid] += amount) >= AC_FLOOD_LIMIT) {
		anticheat_log(playerid, AC_FLOOD, EXCESS_FLOOD);
		natives_Kick(playerid, EXCESS_FLOOD, NULL, -1);
		return 1;
	}
	return 0;
}

void anticheat_on_player_connect(int playerid)
{
	int n = INFRACTIONTYPES;

	while (n--) {
		infractionvalue[n][playerid] = 0;
	}
	floodcount[playerid] = 0;
}

void anticheat_log(int playerid, int eventtype, char *info)
{
	char buf[512], *b = info;

	if (kick_update_delay[playerid]) {
		/*player already kicked*/	
		return;
	}

	while (*b != 0) {
		if (*b == '\'') {
			*b = 255;
		}
		b++;
	}

	b = buf;
	b += sprintf(b, "INSERT INTO acl(t,j,u,l,type,e) "
			"VALUES(UNIX_TIMESTAMP(),");
	if (pdata[playerid] == NULL) {
		/*this should not happen, but better to be safe when dealing
		with strange behaviors that trigger anticheat*/
		b += sprintf(b, "'',NULL,-1");
	} else {
		b += sprintf(b,
			"'%s',%d,%d",
			pdata[playerid]->ip,
			userid[playerid],
			loggedstatus[playerid]);
	}
	sprintf(b, ",%d,'%s')", eventtype, info);
	atoc(buf4096, buf, sizeof(buf));
	NC_mysql_tquery_nocb(buf4096a);
}

float anticheat_GetVehicleHealth(int vehicleid)
{
	float hp;
	int playerid;

	NC_PARS(2);
	nc_params[1] = vehicleid;
	nc_params[2] = buf144a;
	NC(n_GetVehicleHealth_);
	hp = *fbuf144;
	if (hp != hp) {
		playerid = common_find_vehicle_driver(vehicleid);
		if (playerid == INVALID_PLAYER_ID) {
			goto resethp;
		}
		anticheat_log(playerid, AC_VEH_HP_NAN, "NaN vehicle hp");
	} else if (hp > 1000.0f) {
		playerid = common_find_vehicle_driver(vehicleid);
		if (playerid == INVALID_PLAYER_ID) {
			goto resethp;
		}
		sprintf(cbuf144, "vehicle hp %.4f", hp);
		anticheat_log(playerid, AC_VEH_HP_HIGH, cbuf144);
	} else if (hp < 0.0f) {
		return 0.0f;
	} else {
		return hp;
	}
	common_crash_player(playerid);
	natives_Kick(playerid, "invalid vehicle hp", NULL, -1);
resethp:
	NC_PARS(2);
	nc_params[1] = vehicleid;
	nc_paramf[2] = 1000.0f;
	NC(n_SetVehicleHealth);
	return 1000.0f;
}

void anticheat_disallowed_vehicle_1s(int playerid)
{
	anticheat_log(playerid, AC_UNAUTH_VEHICLE_ACCESS,
		"unauthorized vehicle access");
	anticheat_infraction(playerid, AC_IF_DISALLOWED_VEHICLE);
}

static const char *DONTSPAM = "Don't spam!";

int anticheat_on_player_command(int playerid)
{

	if (anticheat_flood(playerid, AC_FLOOD_AMOUNT_CMD)) {
		return 0;
	}
	if (floodcount[playerid] > AC_FLOOD_WARN_THRESHOLD) {
		B144((char*) DONTSPAM);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
	}
	return 1;
}

void anticheat_on_player_enter_vehicle(
	int playerid, int vehicleid, int ispassenger)
{
	if (!ispassenger) {
		NC_PARS(2);
		nc_params[1] = vehicleid;
		nc_params[2] = buf32a;
		NC(n_GetVehicleHealth_);
		if (*fbuf32 != *fbuf32 || *fbuf32 < 0.0f || 1000.0f < *fbuf32) {
			*fbuf32 = 1000.0f;
			NC(n_SetVehicleHealth);
		}
	}
}

int anticheat_on_player_text(int playerid)
{
	if (anticheat_flood(playerid, AC_FLOOD_AMOUNT_CHAT)) {
		return 0;
	}
	if (floodcount[playerid] > AC_FLOOD_WARN_THRESHOLD) {
		B144((char*) DONTSPAM);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
	}
	return 1;
}
