/*SOUND_ROULETTE_ADD_CASH*/
#define SOUND_NAV_SET 1083
/*SOUND_ROULETTE_REMOVE_CASH*/
#define SOUND_NAV_DEL 1084

/*TODO remove this*/
static void panel_nav_updated(int vehicleid);

static struct NAVDATA {
	struct vec3 *beacon;
	struct RUNWAY *vor;
	int ils;
	int dist;
	int alt;
	int crs;
	int vorvalue;
	/**
	ILS x value, [-ILS_SIZE,ILS_SIZE*2] where [0,ILS_SIZE] is 'on target'.
	INVALID_ILS_VALUE when ILS out of range.
	*/
	signed char ilsx;
	/**
	ILS z value, [-ILS_SIZE,ILS_SIZE*2] where [0,ILS_SIZE] is 'on target'
	*/
	signed char ilsz;
} *nav[MAX_VEHICLES];

/*value for VOR when it's out of range*/
#define INVALID_VOR_VALUE 2000000000
/*value for ILS when it's out of range*/
#define INVALID_ILS_VALUE 100
/*maximum distance from where to start showing ILS*/
#define ILS_MAX_DIST (3000.0f)
/*extra offset where ILS values are out of range, but ILS is still shown*/
#define ILS_GREYZONE (80.0f)
/*amount of horizontal/vertical X tokens*/
#define ILS_SIZE 9

static
void nav_init()
{
	int i = 0;
	while (i < MAX_VEHICLES) {
		nav[i++] = NULL;
	}
}

static
void nav_disable(int vehicleid)
{
	free(nav[vehicleid]);
	nav[vehicleid] = NULL;
	panel_nav_updated(vehicleid);
}

/**
Calculates ILS values.

@param ilsx output
@param ilsz output
@param dist horizontal distance of player to beacon
@param z player z
@param targetz beacon z position
@param dx how far player is off of beacon target.
*/
static
void nav_calc_ils_values(
	signed char *ilsx, signed char *ilsz, const float dist,
	const float z, const float targetz, const float dx)
{
	float xdev = 5.0f + dist * (90.0f - 5.0f) / 1500.0f;
	float zdev = 2.0f + dist * (100.0f - 2.0f) / 1250.0f;
	float ztarget = targetz + dist * 0.2f;
	float dz = z - ztarget;
	int tmp;

	if (dz < -zdev - ILS_GREYZONE || dz > zdev + ILS_GREYZONE ||
		dx < -xdev - ILS_GREYZONE || dx > xdev + ILS_GREYZONE)
	{
		*ilsx = *ilsz = INVALID_ILS_VALUE;
		return;
	}
	tmp = (int) (ILS_SIZE * dz / (zdev * 2.0f)) + ILS_SIZE / 2;
	*ilsz = CLAMP(tmp, -ILS_SIZE, ILS_SIZE * 2 - 1);
	tmp = (int) (-ILS_SIZE * dx / (xdev * 2.0f)) + ILS_SIZE / 2;
	*ilsx = CLAMP(tmp, -ILS_SIZE, ILS_SIZE * 2 - 1);
}

static
void nav_update(int vehicleid, struct vec4 *pos)
{
	struct NAVDATA *n = nav[vehicleid];
	float dx, dy;
	float dist, crs, vorangle, horizontaldeviation;
	float heading;
	struct vec3 *beacon;

	if (n == NULL) {
		return;
	}

	if (n->beacon != NULL) {
		beacon = n->beacon;
	} else if (n->vor != NULL) {
		beacon = &n->vor->pos;
	} else {
		return;
	}

	heading = pos->r;
	dx = beacon->x - pos->coords.x;
	dy = beacon->y - pos->coords.y;
	dist = sqrt(dx * dx + dy * dy);
	n->dist = (int) dist;
	if (n->dist > 1000) {
		n->dist = (n->dist / 100) * 100;
	}
	n->alt = (int) (pos->coords.z - beacon->z);
	if (n->vor != NULL) {
		crs = (360.0f - heading) - n->vor->heading;
		vorangle = (float) atan2(dy, dx) - n->vor->headingr;
		horizontaldeviation = dist * (float) sin(vorangle);
		n->vorvalue = (int) horizontaldeviation;
	} else {
		crs = (float) atan2(dy, dx) * 180.0f / M_PI;
		heading += 90.0f;
		if (heading > 360.0f) {
			heading -= 360.0f;
		}
		crs = heading - crs;
		n->vorvalue = INVALID_VOR_VALUE;
	}
	if ((n->vor == NULL || !n->ils) || (dist > ILS_MAX_DIST || (dist *= (float) cos(vorangle)) <= 0.0f)) {
		n->ilsx = n->ilsz = INVALID_ILS_VALUE;
	} else {
		nav_calc_ils_values(&n->ilsx, &n->ilsz, dist, pos->coords.z, beacon->z, horizontaldeviation);
	}
	n->crs = (int) (crs - floor((crs + 180.0f) / 360.0f) * 360.0f);
}

static
void nav_enable(int vehicleid, struct vec3 *adf, struct RUNWAY *vor)
{
	struct vec4 vpos;
	struct NAVDATA *navdata;

	navdata = nav[vehicleid];
	if (navdata == NULL) {
		nav[vehicleid] = navdata = malloc(sizeof(struct NAVDATA));
		navdata->alt = navdata->crs = navdata->dist = 0;
	}
	navdata->beacon = adf;
	navdata->vor = vor;
	navdata->ils = 0;

	GetVehiclePosRotUnsafe(vehicleid, &vpos);
	nav_update(vehicleid, &vpos);
	panel_nav_updated(vehicleid);
}

/**
Checks if a player can do a nav cmd by checking if the vehicle can do the nav.

@param navtype NAV_ADF or NAV_VOR or NAV_ILS
@param vehicleid out param to contain the vehicleid of the player
@return 0 on failure, a message will have been sent to the player
*/
static
int nav_check_can_do_cmd(int playerid, int navtype, int *vehicleid)
{
	int vehiclemodel;

	*vehicleid = NC_GetPlayerVehicleID(playerid);
	vehiclemodel = NC_GetVehicleModel(*vehicleid);
	if (navtype == NAV_ADF) {
		if (!game_is_air_vehicle(vehiclemodel)) {
			SendClientMessage(playerid, COL_WARN, WARN"You're not in an ADF capable vehicle");
			return 0;
		}
	} else if (!game_is_plane(vehiclemodel)) {
		if (navtype == NAV_VOR) {
			SendClientMessage(playerid, COL_WARN, WARN"You're not in a VOR capable vehicle");
		} else {
			SendClientMessage(playerid, COL_WARN, WARN"You're not in an ILS capable vehicle");
		}
		return 0;
	}
	if (NC_GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		SendClientMessage(playerid, COL_WARN, WARN"Only the pilot can change navigation settings");
		return 0;
	}
	return 1;
}

/**
The /adf cmd.

Syntax: /adf [beacon]
Example: /adf ls
When no beacon, disable nav
*/
static
int nav_cmd_adf(CMDPARAMS)
{
	struct AIRPORT *ap;
	int vehicleid, len;
	char beacon[144], *bp;

	if (!nav_check_can_do_cmd(playerid, NAV_ADF, &vehicleid)) {
		return 1;
	}

	if (!cmd_get_str_param(cmdtext, &parseidx, beacon)) {
		if (nav[vehicleid] != NULL) {
			nav_disable(vehicleid);
			NC_PlayerPlaySound0(playerid, SOUND_NAV_DEL);
			return 1;
		}
		SendClientMessage(playerid, COL_WARN, WARN"Syntax: /adf [beacon] - see /beacons or /nearest");
		return 1;
	}

	bp = beacon;
	len = 0;
	while (1) {
		if (*bp == 0) {
			break;
		}
		*bp &= ~0x20;
		if (++len >= 5 || *bp < 'A' || 'Z' < *bp) {
			goto unkbeacon;
		}
		bp++;
	}

	ap = airports;
	len = numairports;
	while (len--) {
		if (strcmp(beacon, ap->code) == 0) {
			nav_enable(vehicleid, &ap->pos, NULL);
			NC_PlayerPlaySound0(playerid, SOUND_NAV_SET);
			return 1;
		}
		ap++;
	}
unkbeacon:
	SendClientMessage(playerid, COL_WARN, WARN"Unknown beacon - see /beacons or /nearest");
	return 1;
}

/**
The /vor cmd.

Syntax: /vor [beacon][runway]
Example: /vor ls09l
Optional whitespace between beacon and runway
When no beacon, disable nav
When no or invalid runway, print a list of valid runways
*/
static
int nav_cmd_vor(CMDPARAMS)
{
	struct AIRPORT *ap;
	struct RUNWAY *rw;
	int vehicleid;
	char beaconpar[144], runwaypar[144], beacon[5], *b, *bp;
	int combinedparameter;
	int len;

	if (!nav_check_can_do_cmd(playerid, NAV_VOR, &vehicleid)) {
		return 1;
	}

	if (!cmd_get_str_param(cmdtext, &parseidx, beaconpar)) {
		if (nav[vehicleid] != NULL) {
			nav_disable(vehicleid);
			NC_PlayerPlaySound0(playerid, SOUND_NAV_DEL);
			return 1;
		}
		SendClientMessage(playerid, COL_WARN, WARN"Syntax: /vor [beacon][runway] - see /beacons or /nearest");
		return 1;
	}
	combinedparameter = !cmd_get_str_param(cmdtext, &parseidx, runwaypar);

	b = beaconpar;
	bp = beacon;
	len = 0;
	while (1) {
		if ((*bp = (*b & ~0x20)) == 0) {
			if (!combinedparameter) {
				b = runwaypar;
			}
			goto havebeacon;
		}
		if (++len >= 5) {
			if (combinedparameter) {
				*bp = 0;
				goto havebeacon;
			}
			goto unkbeacon;
		}
		if (*bp < 'A' || 'Z' < *bp) {
			if (combinedparameter) {
				*bp = 0;
				goto havebeacon;
			}
			goto unkbeacon;
		}
		bp++;
		b++;
	}
havebeacon:
	ap = airports;
	len = numairports;
	while (len--) {
		if (strcmp(beacon, ap->code) == 0) {
			goto haveairport;
		}
		ap++;
	}
unkbeacon:
	SendClientMessage(playerid, COL_WARN, WARN"Unknown beacon - see /beacons or /nearest");
	return 1;
haveairport:
	if (b[0] < '0' || '9' < b[0] ||
		b[1] < '0' || '9' < b[1] ||
		(b[2] != 0 && (b[2] = b[2] & ~0x20) != 'L' &&
			b[2] != 'R' && b[2] != 'C' && b[3] != 0))
	{
		goto tellrws;
	}

	rw = ap->runways;
	while (rw != ap->runwaysend) {
		if (rw->type == RUNWAY_TYPE_RUNWAY && strcmp(rw->id, b) == 0) {
			nav_enable(vehicleid, NULL, rw);
			NC_PlayerPlaySound0(playerid, SOUND_NAV_SET);
			return 1;
		}
		rw++;
	}

tellrws:
	len = 0;
	b = beaconpar; /*reuse a buffer*/
	b += sprintf(b, WARN"Unknown runway, try one of:");
	rw = ap->runways;
	while (rw != ap->runwaysend) {
		if (rw->nav & NAV_VOR) {
			b += sprintf(b, " %s", rw->id);
			len++;
		}
		rw++;
	}
	if (len) {
		SendClientMessage(playerid, COL_WARN, beaconpar);
		return 1;
	}

	SendClientMessage(playerid, COL_WARN, WARN"There are no VOR capable runways at this beacon");
	return 1;
}

/**
The /ils command

Syntax: /ils
Only toggles ils when VOR is already active and the runway has ILS capabilities.
*/
static
int nav_cmd_ils(CMDPARAMS)
{
	struct vec4 vpos;
	struct NAVDATA *np;
	int vehicleid;

	if (!nav_check_can_do_cmd(playerid, NAV_ILS, &vehicleid)) {
		return 1;
	}
	if ((np = nav[vehicleid]) == NULL || np->vor == NULL) {
		SendClientMessage(playerid, COL_WARN, WARN"ILS can only be activated when VOR is already active");
		return 1;
	}
	if ((np->vor->nav & NAV_ILS) != NAV_ILS) {
		SendClientMessage(playerid, COL_WARN, WARN"The selected runway does not have ILS capabilities");
		return 1;
	}
	np->ilsx = np->ilsz = INVALID_ILS_VALUE;
#if SOUND_NAV_SET != 1083 || SOUND_NAV_DEL != 1084
#error "edit the soundid in line below"
#endif
	NC_PlayerPlaySound0(playerid, SOUND_NAV_DEL - (np->ils ^= 1));

	GetVehiclePosRotUnsafe(vehicleid, &vpos);
	nav_update(vehicleid, &vpos);
	panel_nav_updated(vehicleid);
	return 1;
}

void nav_navigate_to_airport(int playerid, int vehicleid, int vehiclemodel, struct AIRPORT *ap)
{
	struct vec3 vpos;
	struct RUNWAY *rw, *closestrw;
	float dx, dy, dz, dist, mindist;

	rw = ap->runways;
	mindist = 0x7F800000;
	closestrw = NULL;
	GetVehiclePos(vehicleid, &vpos);

	/* if plane, try VOR if available, prioritizing ILS runways */
	if (game_is_plane(vehiclemodel)) {
		while (rw != ap->runwaysend) {
			if (rw->type == RUNWAY_TYPE_RUNWAY) {
				dx = rw->pos.x - vpos.x;
				dy = rw->pos.y - vpos.y;
				dz = rw->pos.z - vpos.z;
				dist = dx * dx + dy * dy + dz * dz;
				if (rw->nav & NAV_VOR && dist < mindist) {
					mindist = dist;
					closestrw = rw;
				}
			}
			rw++;
		}
		if (closestrw != NULL) {
			nav_enable(vehicleid, NULL, closestrw);
			sprintf(cbuf144, "Navigation set to %s runway %s", ap->code, closestrw->id);
			SendClientMessage(playerid, COL_SAMP_GREY, cbuf144);
			return;
		}
	} else if (game_is_heli(vehiclemodel)) {
		while (rw != ap->runwaysend) {
			if (rw->type == RUNWAY_TYPE_HELIPAD) {
				dx = rw->pos.x - vpos.x;
				dy = rw->pos.y - vpos.y;
				dz = rw->pos.z - vpos.z;
				dist = dx * dx + dy * dy + dz * dz;
				if (dist < mindist) {
					mindist = dist;
					closestrw = rw;
				}
			}
			rw++;
		}
		if (closestrw != NULL) {
			nav_enable(vehicleid, &closestrw->pos, NULL);
			return;
		}
	}

	nav_enable(vehicleid, &ap->pos, NULL);
}

static
void nav_reset_for_vehicle(int vehicleid)
{
	if (nav[vehicleid] != NULL) {
		nav_disable(vehicleid);
	}
}
