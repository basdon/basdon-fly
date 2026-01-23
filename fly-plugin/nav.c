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
	/**Only used when ILS is active.
	positive means that the touchdown point will be later than desired,
	negative means the touchdown point will be sooner than desired.*/
	char glideslope_number;
} *nav[MAX_VEHICLES];

/*value for VOR when it's out of range*/
#define INVALID_VOR_VALUE 2000000000
/*value for ILS when it's out of range*/
#define INVALID_ILS_VALUE 100
/*maximum distance from where to start showing ILS*/
#define ILS_MAX_DIST (15000.0f)
/*extra offset where ILS values are out of range, but ILS is still shown*/
#define ILS_GREYZONE (500.0f)
/*amount of horizontal/vertical X tokens*/
#define ILS_SIZE 9

static
void nav_init()
{
	TRACE;
	int i = 0;
	while (i < MAX_VEHICLES) {
		nav[i++] = NULL;
	}
}

static
void nav_disable(int vehicleid)
{
	TRACE;
	if (nav[vehicleid]) {
		free(nav[vehicleid]);
		nav[vehicleid] = NULL;
		panel_nav_updated(vehicleid);
	}
}

/**
 * Same as nav_disable, but doesn't try to update the panel immediately.
 */
static
void nav_disable_for_respawned_or_destroyed_vehicle(int vehicleid)
{
	TRACE;
	if (nav[vehicleid]) {
		free(nav[vehicleid]);
		nav[vehicleid] = NULL;
	}
}

/**
Calculates ILS values.

@param dist distance to the runway as if player is on the line of the runway
@param horiz_dev horizontal deviation to runway (basically the VORbar value)
*/
static
void nav_calc_ils_values(struct NAVDATA *n, struct vec3 *player_pos, float dist, const float horiz_dev)
{
	TRACE;
	float xdev;
	float zdev;
	float ztarget;
	float dz;
	int tmp;

	xdev = 10.0f + dist * 100.0f / 5000.0f;
	zdev = 10.0f + dist * 100.0f / 5000.0f;
	ztarget = n->vor->pos.z + dist * 0.15f;
	dz = player_pos->z - ztarget;

	if (dz < -zdev - ILS_GREYZONE || dz > zdev + ILS_GREYZONE ||
		horiz_dev < -xdev - ILS_GREYZONE || horiz_dev > xdev + ILS_GREYZONE)
	{
		n->ilsx = n->ilsz = INVALID_ILS_VALUE;
	} else {
		tmp = (int) (ILS_SIZE * dz / (zdev * 2.0f)) + ILS_SIZE / 2;
		n->ilsz = CLAMP(tmp, -ILS_SIZE, ILS_SIZE * 2 - 1);
		tmp = (int) (-ILS_SIZE * horiz_dev / (xdev * 2.0f)) + ILS_SIZE / 2;
		n->ilsx = CLAMP(tmp, -ILS_SIZE, ILS_SIZE * 2 - 1);
	}
}

static
void nav_calc_ils_glideslope_number(struct NAVDATA *n, float crs, float dist, struct vec3 *velocity)
{
	TRACE;
	float horiz_speed, glideslope_angle;
	int num;

	horiz_speed = (float) cos(crs);
	if (horiz_speed < 0) {
		n->glideslope_number = 127;
	} else {
		horiz_speed *= (float) sqrt(velocity->x * velocity->x + velocity->y * velocity->y);
		glideslope_angle = (float) atan2(-velocity->z, horiz_speed);
		if (glideslope_angle <= 0.0174f) { /*1deg*/
			n->glideslope_number = 127;
		} else if (glideslope_angle >= 1.5533f) { /*89deg*/
			n->glideslope_number = -128;
		} else {
			num = (int) (n->alt / (float) tan(glideslope_angle) - dist) / 50;
			if (num > 127) {
				n->glideslope_number = 127;
			} else if (num < -128) {
				n->glideslope_number = -128;
			} else {
				n->glideslope_number = (char) num;
			}
		}
	}
}

static
void nav_update(int vehicleid, struct vec4 *pos, struct vec3 *velocity)
{
	TRACE;
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
		beacon = &n->vor->touchdown_pos;
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
	n->ilsx = n->ilsz = INVALID_ILS_VALUE;
	if (n->vor != NULL) {
		crs = (360.0f - heading) - n->vor->heading;
		vorangle = (float) atan2(dy, dx) - n->vor->headingr;
		horizontaldeviation = dist * (float) sin(vorangle);
		n->vorvalue = (int) horizontaldeviation;
		if (n->ils && dist < ILS_MAX_DIST && (dist *= (float) cos(vorangle)) >= 0.0f) {
			nav_calc_ils_values(n, &pos->coords, dist, horizontaldeviation);
			nav_calc_ils_glideslope_number(n, crs * M_PI / 180.0f, dist, velocity);
		}
	} else {
		crs = (float) atan2(dy, dx) * 180.0f / M_PI;
		heading += 90.0f;
		if (heading > 360.0f) {
			heading -= 360.0f;
		}
		crs = heading - crs;
		n->vorvalue = INVALID_VOR_VALUE;
	}
	n->crs = (int) (crs - floor((crs + 180.0f) / 360.0f) * 360.0f);
}

static
void nav_enable(int vehicleid, struct vec3 *adf, struct RUNWAY *vor)
{
	TRACE;
	struct vec3 vvel;
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
	GetVehicleVelocityUnsafe(vehicleid, &vvel);
	nav_update(vehicleid, &vpos, &vvel);
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
	TRACE;
	int vehiclemodel;

	*vehicleid = GetPlayerVehicleID(playerid);
	vehiclemodel = GetVehicleModel(*vehicleid);
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
	if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		SendClientMessage(playerid, COL_WARN, WARN"Only the pilot can change navigation settings");
		return 0;
	}
	return 1;
}

void nav_navigate_to_airport(int playerid, int vehicleid, int vehiclemodel, struct AIRPORT *ap)
{
	TRACE;
	struct vec3 vpos;
	struct RUNWAY *rw, *closestrw;
	float dx, dy, dz, dist, mindist;

	rw = ap->runways;
	mindist = float_pinf;
	closestrw = NULL;
	GetVehiclePosUnsafe(vehicleid, &vpos);

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
