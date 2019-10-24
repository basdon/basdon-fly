
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include "a_samp.h"
#include "airport.h"
#include "cmd.h"
#include "game_sa.h"
#include <string.h>
#include <math.h>

/*SOUND_ROULETTE_ADD_CASH*/
#define SOUND_NAV_SET 1083
/*SOUND_ROULETTE_REMOVE_CASH*/
#define SOUND_NAV_DEL 1084

static struct NAVDATA {
	struct AIRPORT *beacon;
	struct RUNWAY *vor;
	int ils;
	int dist;
	int alt;
	int crs;
	int vorvalue;
	unsigned char ilsx, ilsz;
} *nav[MAX_VEHICLES];

static struct playercache {
	int dist;
	int alt;
	int crs;
	int vorvalue;
	int ils;
} pcache[MAX_PLAYERS];

#define INVALID_CACHE 500000
#define INVALID_ILS_VALUE 100
#define ILS_MAX_DIST (1500.0f)

void nav_init()
{
	int i = 0;
	while (i < MAX_VEHICLES) {
		nav[i++] = NULL;
	}
}

void nav_resetcache(int playerid)
{
	pcache[playerid].dist = INVALID_CACHE;
	pcache[playerid].alt = INVALID_CACHE;
	pcache[playerid].crs = INVALID_CACHE;
	pcache[playerid].ils = INVALID_CACHE;
}

/**
Disables navigation for given vehicle.

Ensures the VOR textdraws are hidden.
*/
void nav_disable(int vehicleid)
{
	free(nav[vehicleid]);
	nav[vehicleid] = NULL;
	/*TODO panel_resetNavForPassengers vid*/
}

/**
Enables navigation for given vehicle.

Ensures the VOR textdraws are shown/hidden.

@param adf airport to ADF towards, may be NULL
@param vor runway to VOR towards, may be NULL
*/
void nav_enable(int vehicleid, struct AIRPORT *adf, struct RUNWAY *vor)
{
	struct NAVDATA *np;

	np = nav[vehicleid];
	if (np == NULL) {
		nav[vehicleid] = np = malloc(sizeof(struct NAVDATA));
		np->alt = np->crs = np->dist = 0.0f;
	}
	np->beacon = adf;
	np->vor = vor;
	np->ils = 0;
	if (vor) {
		/* TODO panel_showVorBarForPassengers vid */
	} else {
		/* TODO panel_hideVorBarForPassengers vid */
	}
}

/**
Checks if a player can do a nav cmd by checking if the vehicle can do the nav.

@param navtype NAV_ADF or NAV_VOR or NAV_ILS
@param vehicleid out param to contain the vehicleid of the player
@return 0 on failure, a message will have been sent to the player
*/
int nav_check_can_do_cmd(AMX *amx, int playerid, int navtype, int *vehicleid)
{
	static const char
		*NO_NAV = WARN"You're not in an %s capable vehicle",
		*NO_PILOT = WARN"Only the pilot can change navigation settings";
	char buf[144], *b;

	NC_GetPlayerVehicleID_(playerid, vehicleid);
	NC_GetVehicleModel(*vehicleid);
	if (navtype == NAV_ADF) {
		if (!game_is_air_vehicle(nc_result)) {
			sprintf(buf, NO_NAV, "ADF");
			b = buf;
			goto sendmsgfail;
		}
	} else if (!game_is_plane(nc_result)) {
		sprintf(buf, NO_NAV, navtype == NAV_VOR ? "VOR" : "ILS");
		b = buf;
		goto sendmsgfail;
	}
	NC_GetPlayerState(playerid);
	if (nc_result != PLAYER_STATE_DRIVER) {
		b = (char*) NO_PILOT;
sendmsgfail:	amx_SetUString(buf144, b, 144);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 0;
	}
	return 1;
}

static const char *UNK_BEACON = WARN"Unknown beacon - see /beacons or /nearest";

/**
The /adf cmd.

Syntax: /adf [beacon]
Example: /adf ls
When no beacon, disable nav
*/
int nav_cmd_adf(CMDPARAMS)
{
	static const char
		*SYN = WARN"Syntax: /adf [beacon] - see /beacons or /nearest";

	void panel_hide_vor_bar_for_passengers(AMX*, int);
	void panel_reset_nav_for_passengers(AMX*, int);

	struct AIRPORT *ap;
	struct NAVDATA *np;
	int vehicleid, len;
	char beacon[144], *bp;

	if (!nav_check_can_do_cmd(amx, playerid, NAV_ADF, &vehicleid)) {
		return 1;
	}

	if (!cmd_get_str_param(cmdtext, &parseidx, beacon)) {
		if (nav[vehicleid] != NULL) {
			nav_disable(vehicleid);
			panel_reset_nav_for_passengers(amx, vehicleid);
			NC_PlayerPlaySound0(playerid, SOUND_NAV_DEL);
			return 1;
		}
		amx_SetUString(buf144, SYN, 144);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
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
		if (strcmp(beacon, ap->beacon) == 0) {
			nav_enable(vehicleid, ap, NULL);
			panel_hide_vor_bar_for_passengers(amx, vehicleid);
			NC_PlayerPlaySound0(playerid, SOUND_NAV_SET);
			return 1;
		}
		ap++;
	}
unkbeacon:
	amx_SetUString(buf144, UNK_BEACON, 144);
	NC_SendClientMessage(playerid, COL_WARN, buf144a);
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
int nav_cmd_vor(CMDPARAMS)
{
	static const char
		*SYN = WARN"Syntax: /vor [beacon][runway] - "
			"see /beacons or /nearest",
		*NO_CAP = WARN"There are no VOR capable runways at this beacon";

	void panel_show_vor_bar_for_passengers(AMX*, int);
	void panel_reset_nav_for_passengers(AMX*, int);

	struct NAVDATA *np;
	struct AIRPORT *ap;
	struct RUNWAY *rw;
	int vehicleid;
	char beaconpar[144], runwaypar[144], beacon[5], *b, *bp;
	int combinedparameter;
	int len;

	if (!nav_check_can_do_cmd(amx, playerid, NAV_VOR, &vehicleid)) {
		return 1;
	}

	if (!cmd_get_str_param(cmdtext, &parseidx, beaconpar)) {
		if (nav[vehicleid] != NULL) {
			nav_disable(vehicleid);
			panel_reset_nav_for_passengers(amx, vehicleid);
			NC_PlayerPlaySound0(playerid, SOUND_NAV_DEL);
			return 1;
		}
		amx_SetUString(buf144, SYN, 144);
		goto sendwarnmsg;
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
		if (strcmp(beacon, ap->beacon) == 0) {
			goto haveairport;
		}
		ap++;
	}
unkbeacon:
	amx_SetUString(buf144, UNK_BEACON, 144);
	goto sendwarnmsg;
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
		if (strcmp(rw->id, b) == 0) {
			nav_enable(vehicleid, NULL, rw);
			panel_show_vor_bar_for_passengers(amx, vehicleid);
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
		if (rw ->nav) {
			b += sprintf(b, " %s", rw->id);
			len++;
		}
		rw++;
	}
	if (len) {
		amx_SetUString(buf144, beaconpar, 144);
	} else {
		amx_SetUString(buf144, NO_CAP, 144);
	}
sendwarnmsg:
	NC_SendClientMessage(playerid, COL_WARN, buf144a);
	return 1;
}

/**
The /ils command

Syntax: /ils
Only toggles ils when VOR is already active and the runway has ILS capabilities.
*/
int nav_cmd_ils(CMDPARAMS)
{
	static const char
		*N_VOR = "ILS can only be activated when VOR is already active",
		*N_CAP = "The selected runway does not have ILS capabilities";

	struct NAVDATA *np;
	int vehicleid;

	if (!nav_check_can_do_cmd(amx, playerid, NAV_ILS, &vehicleid)) {
		return 1;
	}
	if ((np = nav[vehicleid]) == NULL || np->vor == NULL) {
		amx_SetUString(buf144, N_VOR, 144);
		goto retmsg;
	}
	if ((np->vor->nav & NAV_ILS) != NAV_ILS) {
		amx_SetUString(buf144, N_CAP, 144);
retmsg:		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 1;
	}
	np->ilsx = np->ilsz = INVALID_ILS_VALUE;
#if SOUND_NAV_SET != 1083 || SOUND_NAV_DEL != 1084
#error "edit the soundid in line below"
#endif
	NC_PlayerPlaySound0(playerid, SOUND_NAV_SET + (np->ils ^= 1));
	return 1;
}

/**
Update nav related textdraws in panel for player if needed.

Does not actually update the nav data (see nav_update for that), just updates
the textdraws.
*/
void nav_update_textdraws(
	AMX *amx, int playerid, int vehicleid, int *ptxt_adf_dis_base,
	int *ptxt_adf_alt_base, int *ptxt_adf_crs_base, int *ptxt_vor_base)
{
	static const char
		*ILS_X = "~w~X~n~~w~X~n~~w~X~n~~w~X~n~~w~X ~w~X ~w~X ~w~X ~w~X"
			" ~w~X ~w~X ~w~X ~w~X~n~~w~X~n~~w~X~n~~w~X~n~~w~X";
	static const unsigned char
		ILS_X_OFFSETS[] = { 1, 8, 15, 22, 49, 76, 83, 90, 97 };

	struct NAVDATA *n = nav[vehicleid];
	char buf[144];

	if (n == NULL) {
		return;
	}

	nc_params[0] = 3;
	nc_params[1] = playerid;
	nc_params[3] = buf144a;

	if (n->dist != pcache[playerid].dist) {
		pcache[playerid].dist = n->dist;
		if (n->dist >= 1000) {
			sprintf(buf, "%.1fK", n->dist / 1000.0f);
		} else {
			sprintf(buf, "%d", n->dist);
		}
		nc_params[2] = ptxt_adf_dis_base[playerid];
		amx_SetUString(buf144, buf, sizeof(buf));
		NC(n_PlayerTextDrawSetString);
	}

	if (n->alt != pcache[playerid].alt) {
		pcache[playerid].alt = n->alt;
		sprintf(buf, "%d", n->alt);
		nc_params[2] = ptxt_adf_alt_base[playerid];
		amx_SetUString(buf144, buf, sizeof(buf));
		NC(n_PlayerTextDrawSetString);
	}

	if (n->crs != pcache[playerid].crs) {
		pcache[playerid].crs = n->crs;
		sprintf(buf, "%d", n->crs);
		nc_params[2] = ptxt_adf_crs_base[playerid];
		amx_SetUString(buf144, buf, sizeof(buf));
		NC(n_PlayerTextDrawSetString);
	}

	if (n->ilsx < 0 || 8 < n->ilsx) {
		amx_SetUString(buf144, "~r~~h~no ILS signal", 144);
		goto doils;
	} else if (0 <= n->ilsz &&  n->ilsz <= 8) {
		amx_SetUString(buf144, ILS_X, 144);
		buf144[29 + 5 * n->ilsx] = buf144[ILS_X_OFFSETS[n->ilsz]] = 'r';
doils:
		NC_GameTextForPlayer(playerid, buf144a, 200, 6);
	}

	if (n->vorvalue < 640) {
		if (ptxt_vor_base[playerid] != -1) {
			nc_params[0] = 2;
			nc_params[1] = playerid;
			nc_params[2] = ptxt_vor_base[playerid];
			NC(n_PlayerTextDrawDestroy);
		}
		buf144[0] = 'i';
		buf144[1] = 0;
		nc_params[0] = 4;
		nc_params[1] = playerid;
		*((float*) (nc_params + 2)) = (float) n->vorvalue;
		*((float*) (nc_params + 3)) = 407.0f;
		nc_params[4] = buf144a;
		NC(n_CreatePlayerTextDraw);

		nc_params[2] = ptxt_vor_base[playerid] = nc_result;
		*((float*) (nc_params + 3)) = 0.4f;
		*((float*) (nc_params + 4)) = 1.6f;
		NC(n_PlayerTextDrawLetterSize);

		nc_params[0] = 3;
		nc_params[3] = 0xFF00FFFF;
		NC(n_PlayerTextDrawColor);
		nc_params[3] = 2;
		NC(n_PlayerTextDrawAlignment);
		NC(n_PlayerTextDrawFont);
		nc_params[3] = 1;
		NC(n_PlayerTextDrawSetProportional);
		nc_params[3] = 0;
		NC(n_PlayerTextDrawSetOutline);
		NC(n_PlayerTextDrawSetShadow);

		nc_params[0] = 2;
		NC(n_PlayerTextDrawShow);
	} else {
		ptxt_vor_base[playerid] = -1;
	}

	if (n->vorvalue != pcache[playerid].vorvalue) {
		pcache[playerid].vorvalue = n->vorvalue;
	}
}

/**
May return NAV_NONE, NAV_ADF, NAV_VOR or NAV_VOR|NAV_ILS
*/
int nav_get_active_type(int vehicleid)
{
	if (nav[vehicleid] == NULL) {
		return NAV_NONE;
	}
	if (nav[vehicleid]->beacon != NULL) {
		return NAV_ADF;
	}
	if (nav[vehicleid]->vor != NULL) {
		return NAV_VOR | (NAV_ILS * nav[vehicleid]->ils);
	}
	return NAV_NONE;
}

/* native Nav_GetActiveNavType(vehicleid) */
cell AMX_NATIVE_CALL Nav_GetActiveNavType(AMX *amx, cell *params)
{
	int vid = params[1];
	if (nav[vid] == NULL) {
		return NAV_NONE;
	}
	if (nav[vid]->beacon != NULL) {
		return NAV_ADF;
	}
	if (nav[vid]->vor != NULL) {
		return NAV_VOR | (NAV_ILS * nav[vid]->ils);
	}
	return NAV_NONE;
}

/* native Nav_NavigateToMission(vehicleid, vehiclemodel, airportidx, Float:x, Float:y, Float:z) */
cell AMX_NATIVE_CALL Nav_NavigateToMission(AMX *amx, cell *params)
{
	void panel_hide_vor_bar_for_passengers(AMX*, int);
	void panel_show_vor_bar_for_passengers(AMX*, int);

	const int vid = params[1], vehiclemodel = params[2] - 400;
	struct AIRPORT *ap = airports + params[3];
	struct RUNWAY *rw, *shortestrw, *shortestilsrw;
	float x, y, z, dx, dy, dz, dist, mindistall, mindistils;

	/* if plane, try VOR if available, prioritizing ILS runways */
	if (0 <= vehiclemodel && vehiclemodel < MODEL_TOTAL && vehicleflags[vehiclemodel] & PLANE) {
		rw = ap->runways;
		mindistall = mindistils = 0x7F800000;
		shortestrw = shortestilsrw = NULL;
		x = amx_ctof(params[4]);
		y = amx_ctof(params[5]);
		z = amx_ctof(params[6]);
		while (rw != ap->runwaysend) {
			dx = rw->pos.x - x;
			dy = rw->pos.y - y;
			dz = rw->pos.z - z;
			dist = dx * dx + dy * dy + dz * dz;
			if (rw->nav & NAV_VOR && dist < mindistall) {
				mindistall = dist;
				shortestrw = rw;
			}
			if (rw->nav & NAV_ILS && dist < mindistils) {
				mindistils = dist;
				shortestilsrw = rw;
			}
			rw++;
		}
		if (shortestrw != NULL) {
			if (shortestilsrw != NULL) {
				shortestrw = shortestilsrw;
			}
			if (nav[vid] == NULL) {
				nav[vid] = malloc(sizeof(struct NAVDATA));
				nav[vid]->alt = nav[vid]->crs = nav[vid]->dist = 0.0f;
			}
			nav[vid]->beacon = NULL;
			nav[vid]->ils = 0;
			nav[vid]->vor = shortestrw;
			panel_show_vor_bar_for_passengers(amx, vid);
			return 1;
		}
	}

	/* if heli of no VOR runways, do ADF */
	if (nav[vid] == NULL) {
		nav[vid] = malloc(sizeof(struct NAVDATA));
		nav[vid]->alt = nav[vid]->crs = nav[vid]->dist = 0.0f;
	}
	nav[vid]->beacon = ap;
	nav[vid]->ils = 0;
	nav[vid]->vor = NULL;
	panel_hide_vor_bar_for_passengers(amx, vid);
	return 1;
}

/**
Resets (disables) navigation for given vehicle.
*/
void nav_reset_for_vehicle(int vehicleid)
{
	if (nav[vehicleid] != NULL) {
		free(nav[vehicleid]);
		nav[vehicleid] = NULL;
	}
}

/*TOREMOVE*/
/* native Nav_Reset(vehicleid) */
cell AMX_NATIVE_CALL Nav_Reset(AMX *amx, cell *params)
{
	void panel_reset_nav_for_passengers(AMX*, int);

	int vid = params[1];
	if (nav[vid] != NULL) {
		free(nav[vid]);
		nav[vid] = NULL;
		panel_reset_nav_for_passengers(amx, vid);
		return 1;
	}
	return 0;
}

/* native Nav_ResetCache(playerid) */
cell AMX_NATIVE_CALL Nav_ResetCache(AMX *amx, cell *params)
{
	nav_resetcache(params[1]);
	return 1;
}

void calc_ils_values(
	unsigned char *ilsx, unsigned char *ilsz, const float ydist,
	const float z, const float dx)
{
	float xdev = 5.0f + ydist * (90.0f - 5.0f) / ILS_MAX_DIST;
	float zdev = 2.0f + ydist * (100.0f - 2.0f) / 1500.0f;
	float ztarget = 4.0f + ydist * (130.0f - 4.0f) / (500.0f);
	int tmp;

#define GREYZONE (50.0f)
	if (z < ztarget - zdev - GREYZONE || z > ztarget + zdev + GREYZONE ||
		dx < -xdev - GREYZONE || dx > xdev + GREYZONE)
	{
		*ilsx = INVALID_ILS_VALUE;
		*ilsz = 0;
		return;
	}
	tmp = (int) (-8.0f * (z - ztarget) / (zdev * 2.0f)) + 4;
	*ilsz = CLAMP(tmp, 0, 8);
	tmp = (int) (-8.0f * dx / (xdev * 2.0f)) + 4;
	*ilsx = CLAMP(tmp, 0, 8);
}

void nav_update(AMX *amx, int vehicleid,
	float x, float y, float z, float heading)
{
	struct NAVDATA *n = nav[vehicleid];
	float dx, dy;
	float dist, crs, vorangle, horizontaldeviation;
	struct vec3 *pos;

	if (n == NULL) {
		return;
	}

	if (n->beacon != NULL) {
		pos = &n->beacon->pos;
	} else if (n->vor != NULL) {
		pos = &n->vor->pos;
	} else {
		return;
	}

	heading = 360.0f - heading;

	dx = x - pos->x;
	dy = pos->y - y;
	dist = sqrt(dx * dx + dy * dy);
	n->dist = (int) dist;
	if (n->dist > 1000) {
		n->dist = (n->dist / 100) * 100;
	}
	n->alt = (int) (z - pos->z);
	crs = -atan2(dx, dy);
	if (n->vor != NULL ) {
		vorangle = crs + M_PI2 - n->vor->headingr;
		horizontaldeviation = dist * cos(vorangle);
		n->vorvalue = (int) (horizontaldeviation / 50.0f * 85.0f);
		n->vorvalue = CLAMP(n->vorvalue, -85, 85);
		n->vorvalue = 320 - n->vorvalue;
		crs = heading - n->vor->heading;
	} else {
		n->vorvalue = 1000;
		crs = heading - (crs * 180.0f / M_PI);
	}
	n->crs = (int) (crs - floor((crs + 180.0f) / 360.0f) * 360.0f);
	if (n->vor == NULL || !n->ils) {
		n->ilsx = 0;
		n->ilsz = INVALID_ILS_VALUE;
	} else if (dist > ILS_MAX_DIST || (dist *= sin(vorangle)) <= 0.0f) {
		n->ilsx = INVALID_ILS_VALUE;
		n->ilsz = 0;
	} else {
		calc_ils_values(&n->ilsx, &n->ilsz, dist, z, horizontaldeviation);
	}
}
