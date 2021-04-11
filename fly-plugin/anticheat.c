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

/**
Decrease flood count for all players.

To be called every 100ms
*/
static
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

/**
Decrease all infractions for all players based on each infraction's decrement.

To be called in timer5000.
*/
static
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

static
void anticheat_on_player_connect(int playerid)
{
	int n = INFRACTIONTYPES;

	while (n--) {
		infractionvalue[n][playerid] = 0;
	}
	floodcount[playerid] = 0;
}

/**
Log some anticheat related thing. Uses buf4096 after info is copied.

@param eventtype one of AC_ definitions
@param info primitively escaped into db query
*/
static
void anticheat_log(int playerid, int eventtype, char *info)
{
	char buf[2200]; /*Entry has maxlen 2048 in the db.*/
	char *b;

	if (kick_update_delay[playerid]) {
		/*player already kicked*/	
		return;
	}

#ifdef DEV
	printf("anticheat_log: type %d msg %s\n", eventtype, info);
#endif

	b = buf + sprintf(buf, "INSERT INTO acl(t,j,u,l,type,e) VALUES(UNIX_TIMESTAMP(),");
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
	b += sprintf(b, ",%d,'", eventtype);
	while (*info) {
		if (*info == '\'') {
			*(b++) = '\\';
		}
		*(b++) = *info;
		info++;
	}
	*(b++) = '\'';
	*(b++) = ')';
	*b = 0;
	atoc(buf4096, buf, sizeof(buf));
	NC_mysql_tquery_nocb(buf4096a);
}

/**
Add given flood amount to given player's flood value.

Player will be kicked on excess flood.

@returns non-zero if player is kicked as result
*/
static
int anticheat_flood(int playerid, int amount)
{
	if ((floodcount[playerid] += amount) >= AC_FLOOD_LIMIT) {
		anticheat_log(playerid, AC_FLOOD, "excess flood");
		natives_Kick(playerid, "excess flood", NULL, -1);
		return 1;
	}
	return 0;
}

/**
Gets vehicle hp, after checking for unnacceptable values and handling offenders.

@return 0.0f if the vehicle is not valid.
*/
static
float GetVehicleHealth(int vehicleid)
{
	struct SampVehicle *vehicle;
	float hp;
	int playerid;

	vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
	if (!vehicle) {
		return 0.0f;
	}
	hp = vehicle->health;
	if (hp != hp) {
		playerid = GetVehicleDriver(vehicleid);
		if (playerid == INVALID_PLAYER_ID) {
			goto resethp;
		}
		anticheat_log(playerid, AC_VEH_HP_NAN, "NaN vehicle hp");
	} else if (hp > 1000.0f) {
		playerid = GetVehicleDriver(vehicleid);
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
	CrashPlayer(playerid);
	natives_Kick(playerid, "invalid vehicle hp", NULL, -1);
resethp:
	SAMP_SetVehicleHealth(vehicle, 1000.0f);
	return 1000.0f;
}

/**
Call when a player is in a vehicle it can not be in.

Should be called at most once per second, player will be kicked when it happens
too often.
*/
static
void anticheat_disallowed_vehicle_1s(int playerid)
{
	anticheat_log(playerid, AC_UNAUTH_VEHICLE_ACCESS,
		"unauthorized vehicle access");
	anticheat_infraction(playerid, AC_IF_DISALLOWED_VEHICLE);
}

/**
@return non-zero when player is kicked as result
*/
static
int anticheat_on_player_command(int playerid)
{
	if (anticheat_flood(playerid, AC_FLOOD_AMOUNT_CMD)) {
		return 1;
	}
	if (floodcount[playerid] > AC_FLOOD_WARN_THRESHOLD) {
		SendClientMessage(playerid, COL_WARN, WARN"Don't spam!");
	}
	return 0;
}

/**
Ensures the vehicle's health is valid
*/
static
void anticheat_on_player_enter_vehicle(int playerid, int vehicleid, int ispassenger)
{
	register struct SampVehicle *vehicle;
	register float hp;

	if (!ispassenger) {
		vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
		if (vehicle) {
			hp = vehicle->health;
			if (hp != hp || hp < 0.0f || 1000.0f < hp) {
				SAMP_SetVehicleHealth(vehicle, 1000.0f);
			}
		}
	}
}

static
int anticheat_on_player_text(int playerid)
{
	if (anticheat_flood(playerid, AC_FLOOD_AMOUNT_CHAT)) {
		return 0;
	}
	if (floodcount[playerid] > AC_FLOOD_WARN_THRESHOLD) {
		SendClientMessage(playerid, COL_WARN, WARN"Don't spam!");
	}
	return 1;
}
