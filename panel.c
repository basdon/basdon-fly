
/* vim: set filetype=c ts=8 noexpandtab: */

#define _CRT_SECURE_NO_DEPRECATE
#include "common.h"
#include "a_samp.h"
#include <math.h>
#include <string.h>

/*
value vs small vs large:
+---------+
|   large |
|   large |
|+--------+
||  small |
||+-------+
||| value |
||+-------+
||  small |
|+--------+
|   large |
|   large |
+---------+
*/

#define PANEL_BG 0x77
#define SMALL_METER_BG 0x77
#define VORBAR_BG 0x66

#define LARGE_METER_COLOR 0x989898FF
#define SMALL_METER_COLOR 0x585858FF

struct PANELCACHE {
	short altitude;
	short altitudemetersmall;
	short altitudemeterlarge;
	short speed;
	short speedmeterlarge;
	short heading;
} caches[MAX_PLAYERS];

/**
Panel background global textdraw id.
*/
static int txt_bg;
/**
Panel vertical speed indicator global textdraw id.
*/
static int txt_vai;
/**
Panel speed indicator background global textdraw id.
*/
static int txt_bg_spd;
/**
Panel altitude indicator background global textdraw id.
*/
static int txt_bg_alt;
/**
Panel adf labels global textdraw id.
*/
static int txt_adf_labels;
/**
Panel vor bar global textdraw id.
*/
static int txt_vorbar;

static int ptxt_spd_large[MAX_PLAYERS];
static int ptxt_spd_small[MAX_PLAYERS];
static int ptxt_spd_value[MAX_PLAYERS];
static int ptxt_alt_large[MAX_PLAYERS];
static int ptxt_alt_small[MAX_PLAYERS];
static int ptxt_alt_value[MAX_PLAYERS];
static int ptxt_hdg_meter[MAX_PLAYERS];
static int ptxt_hdg_value[MAX_PLAYERS];
static int ptxt_adf_dis[MAX_PLAYERS];
static int ptxt_adf_alt[MAX_PLAYERS];
static int ptxt_adf_crs[MAX_PLAYERS];
static int ptxt_hpflodo[MAX_PLAYERS];
/**
Vertical Airspeed Indicator value indicator textdraw.
*/
static int ptxt_vai[MAX_PLAYERS];
/**
VOR value indicator textdraw.
*/
static int ptxt_vor[MAX_PLAYERS];

/**
Players that should receive panel updates.
*/
static int panelplayers[MAX_PLAYERS];
/**
Amount of players int he panelplayers array.
*/
static int numpanelplayers;

/**
Resets caches for panel stuff, to make sure textdraws update next iteration.
*/
static void panel_reset_caches(int playerid)
{
	void nav_resetcache(int);

	memset(&caches[playerid], 0xFF, sizeof(struct PANELCACHE));
	nav_resetcache(playerid);
}

/**
Update the altitude textdraws.

The large values show counts of 50 above and below.

The small values show counts of 10 above and below.

Value should have leading zeros.

+--------+
|  100 - | large
|   50 - | large
|+-------+
||   20  | small
||+------+
||| 015  | value
||+------+
||   10  | small
|+-------+
|    0 - | large
|  -50 - | large
+--------+
*/
static void panel_update_altitude(AMX *amx, int playerid, int altitude)
{
	int altitude50 = altitude / 50;
	int i, tmp, tmp2;
	char buf[50];
	struct PANELCACHE *pcache = caches + playerid;

	if (altitude == pcache->altitude) {
		return;
	}
	pcache->altitude = altitude;

	nc_params[0] = 3;
	nc_params[1] = playerid;
	nc_params[3] = buf144a;

	/* alt */
	sprintf(buf, "%03d", altitude);
	amx_SetUString(buf144, buf, sizeof(buf));
	nc_params[2] = ptxt_alt_value[playerid];
	NC(n_PlayerTextDrawSetString);

	/* alt meter large */
	altitude50 -= ((unsigned int) altitude & 0x80000000) >> 31;
	if (altitude50 != pcache->altitudemeterlarge) {
		pcache->altitudemeterlarge = altitude50;
		strcpy(buf, "____-~n~____-~n~~n~~n~~n~~n~____-~n~____-~n~");
		for (i = 0; i < 4; i++) {
			tmp = altitude50 + 2 - i;
			if (tmp < -18 || 19 < tmp) {
				continue;
			}
			tmp2 = i * 8 + (i >> 1) * 12;
			sprintf(buf + tmp2, "%4d", (tmp * 50));
			buf[tmp2 + 4] = '-';
		}
		amx_SetUString(buf144, buf, sizeof(buf));
		nc_params[2] = ptxt_alt_large[playerid];
		NC(n_PlayerTextDrawSetString);
	}

	/* alt meter small */
	tmp = altitude + 10 * (((unsigned int) altitude & 0x8000000) >> 22);
	tmp = (tmp / 10) % 10;
	if (tmp != pcache->altitudemetersmall) {
		pcache->altitudemetersmall = tmp;
		strcpy(buf, "_00~n~~n~_00");
		if (altitude >= 0) {
			tmp = altitude + 10;
			tmp2 = altitude;
		} else {
			buf[0] = buf[9] = '-';
			tmp = -altitude;
			tmp2 = -altitude + 10;
		}
		buf[1] = '0' + (tmp / 10) % 10;
		buf[10] = '0' + (tmp2 / 10) % 10;
		amx_SetUString(buf144, buf, sizeof(buf));
		nc_params[2] = ptxt_alt_small[playerid];
		NC(n_PlayerTextDrawSetString);
	}
}

/**
Update the speed textdraws.

The large values show counts of 10 above and below with a dash next to it.
If 0 or below, show only the dash.

The small values show one above and one below. If 0, do not show.

Value should have leading zeros.

+--------+
|   30 - | large
|   20 - | large
|+-------+
||   15  | small
||+------+
||| 014  | value
||+------+
||   13  | small
|+-------+
|   10 - | large
|      - | large
+--------+
*/
static void panel_update_speed(AMX *amx, int playerid, int speed)
{
	static const char SPDMETERDATA[] =
		"160-~n~150-~n~140-~n~130-~n~120-~n~110-~n~100-~n~_90-~n~_"
		"80-~n~_70-~n~_60-~n~_50-~n~_40-~n~_30-~n~_20-~n~_10-~n~___-"
		"~n~___-~n~___";

	int tmp;
	char buf[50];
	struct PANELCACHE *pcache = caches + playerid;

	if (speed == pcache->speed) {
		return;
	}
	pcache->speed = speed;

	nc_params[0] = 3;
	nc_params[1] = playerid;
	nc_params[3] = buf144a;

	/* spd */
	sprintf(buf, "%03d", speed);
	amx_SetUString(buf144, buf, sizeof(buf));
	nc_params[2] = ptxt_spd_value[playerid];
	NC(n_PlayerTextDrawSetString);

	/*0 and 149 are important boundary checks for code below*/
	if (speed < 0 || 149 < speed) {
		return;
	}

	/* spd meter large */
	tmp = (14 - speed / 10) * 7;
	if (tmp != pcache->speedmeterlarge) {
		pcache->speedmeterlarge = tmp;
		strcpy(buf, "xxx-~n~xxx-~n~~n~~n~~n~~n~xxx-~n~xxx-~n~");
		memcpy(buf, SPDMETERDATA + tmp, 11);
		memcpy(buf + 26, SPDMETERDATA + tmp + 14, 11);
		amx_SetUString(buf144, buf, sizeof(buf));
		nc_params[2] = ptxt_spd_large[playerid];
		NC(n_PlayerTextDrawSetString);
	}

	/* spd meter small */
	strcpy(buf, "0~n~~n~0");
	buf[0] = '0' + ((speed + 1) % 10);
	buf[7] = '0' + ((speed + 9) % 10);
	amx_SetUString(buf144, buf, sizeof(buf));
	nc_params[2] = ptxt_spd_small[playerid];
	NC(n_PlayerTextDrawSetString);
}

static void panel_update_heading(AMX *amx, int playerid, int heading)
{
	char buf[50];

	/* normalize before cache, because rounding (0 & 360)*/
	heading = 360 - heading;
	if (heading == 0) {
		heading = 360;
	}

	if (heading == caches[playerid].heading) {
		return;
	}
	caches[playerid].heading = heading;

	nc_params[0] = 3;
	nc_params[1] = playerid;
	nc_params[3] = buf144a;

	/* hdg */
	sprintf(buf, "%03d", heading);
	amx_SetUString(buf144, buf, sizeof(buf));
	nc_params[2] = ptxt_hdg_value[playerid];
	NC(n_PlayerTextDrawSetString);

	/* hdg meter */
	heading--;
	sprintf(buf,
		"%03d_%03d_%03d________%03d_%03d_%03d",
		((heading + 357) % 360) + 1,
		((heading + 358) % 360) + 1,
		((heading + 359) % 360) + 1,
		((heading + 1) % 360) + 1,
		((heading + 2) % 360) + 1,
		((heading + 3) % 360) + 1);
	amx_SetUString(buf144, buf, sizeof(buf));
	nc_params[2] = ptxt_hdg_meter[playerid];
	NC(n_PlayerTextDrawSetString);
}

/**
Panel loop. Call from timer100.
*/
void panel_timed_update(AMX *amx)
{
	void nav_update(AMX*, int, float, float, float, float);
	void nav_update_textdraws(AMX*, int, int, int*, int*, int*, int*);

	int playerid, vehicleid, n = numpanelplayers;
	float x, y, z, heading, hp;

	while (n--) {
		playerid = panelplayers[n];

		NC_GetPlayerVehicleID_(playerid, &vehicleid);

		NC_GetVehiclePos(vehicleid, buf32a, buf64a, buf144a);
		x = amx_ctof(*buf32);
		y = amx_ctof(*buf64);
		z = amx_ctof(*buf144);
		panel_update_altitude(amx, playerid, (int) z);

		NC_GetVehicleZAngle(vehicleid, buf144a);
		heading = *((float*) buf144);
		panel_update_heading(amx, playerid, (int) heading);

		NC_GetPlayerState(playerid);
		if (nc_result == PLAYER_STATE_DRIVER) {
			nav_update(amx, vehicleid, x, y, z, heading);
		}

		nav_update_textdraws(amx,
			playerid,
			vehicleid,
			ptxt_adf_dis,
			ptxt_adf_alt,
			ptxt_adf_crs,
			ptxt_vor);

		hp = anticheat_NC_GetVehicleHealth(amx, vehicleid);
		/*
		veh things here (FL/ODO)
		if (Veh_FormatPanelText(playerid, vid, hp, buf144)) {
			PlayerTextDrawSetString playerid,
			playerpnltxt[playerid][PNLTXT_HPFLODO], buf144
		}
		*/

		/*vai*/
		NC_GetVehicleVelocity(vehicleid, buf32a, buf64a, buf144a);
		x = *((float*) buf32);
		y = *((float*) buf64);
		z = *((float*) buf144);
		x = VEL_TO_KTS_VAL * sqrt(x * x + y * y + z * z);
		panel_update_speed(amx, playerid, (int) x);

		z *= VEL_TO_KFPM_VAL * 14.5;
		if (z < -34.0f) {
			z = -34.0f;
		} else if (z > 34.0f) {
			z = 34.0f;
		}
		nc_params[0] = 2;
		nc_params[1] = playerid;
		nc_params[2] = ptxt_vai[playerid];
		NC(n_PlayerTextDrawDestroy);
		nc_params[0] = 4;
		buf144[0] = '~';
		buf144[1] = '<';
		buf144[2] = '~';
		buf144[3] = 0;
		nc_params[1] = playerid;
		*((float*) (nc_params + 2)) = 458.0f;
		*((float*) (nc_params + 3)) = 391.0f - z;
		nc_params[4] = buf144a;
		NC_(n_CreatePlayerTextDraw, ptxt_vai + playerid);
		nc_params[2] = ptxt_vai[playerid];
		*((float*) (nc_params + 3)) = 0.25f;
		*((float*) (nc_params + 4)) = 1.1f;
		NC(n_PlayerTextDrawLetterSize);
		nc_params[0] = 3;
		nc_params[3] = 0xFF0000FF;
		NC(n_PlayerTextDrawColor);
		nc_params[3] = 2;
		NC(n_PlayerTextDrawAlignment);
		NC(n_PlayerTextDrawFont);
		nc_params[3] = 0;
		NC(n_PlayerTextDrawSetOutline);
		NC(n_PlayerTextDrawSetShadow);
		nc_params[0] = 2;
		NC(n_PlayerTextDrawShow);
	}
}

void panel_remove_panel_player(int playerid)
{
	int i = numpanelplayers;

	while (i--) {
		if (panelplayers[i] == playerid) {
			panelplayers[i]	= panelplayers[--numpanelplayers];
			return;
		}
	}
}

static void panel_reset_nav(AMX *amx, int playerid)
{
	if (ptxt_vai[playerid] != -1) {
		buf144[0] = '-';
		buf144[1] = 0;
		nc_params[0] = 3;
		nc_params[1] = playerid;
		nc_params[2] = ptxt_adf_dis[playerid];
		nc_params[3] = buf144a;
		NC(n_PlayerTextDrawSetString);
		nc_params[2] = ptxt_adf_alt[playerid];
		NC(n_PlayerTextDrawSetString);
		nc_params[2] = ptxt_adf_crs[playerid];
		NC(n_PlayerTextDrawSetString);
		if (ptxt_vor[playerid] != -1) {
			nc_params[2] = ptxt_vor[playerid];
			NC(n_PlayerTextDrawDestroy);
			ptxt_vor[playerid] = -1;
		}
		nc_params[2] = txt_vorbar;
		NC(n_TextDrawHideForPlayer);
		panel_reset_caches(playerid);
	}
}

/**
Show the VOR bar for passengers of given vehicle.
*/
void panel_show_vor_bar_for_passengers(AMX *amx, int vehicleid)
{
	/*TODO: use on_player_was_afk to show VOR bar to passengers that
	were afk while the pilot enabled VOR,
	then can change to panelplayers*/
	int playerid, n = playercount;

	while (n--) {
		playerid = players[n];
		NC_GetPlayerVehicleID(playerid);
		if (nc_result == vehicleid) {
			NC_TextDrawShowForPlayer(playerid, txt_vorbar);
		}
	}
}

/**
Hides the VOR bar for passengers of given vehicle.
*/
void panel_hide_vor_bar_for_passengers(AMX *amx, int vehicleid)
{
	/*TODO: use on_player_was_afk to hide VOR bar to passengers that
	were afk while the pilot disabled VOR,
	then can change to panelplayers*/
	int playerid, n = playercount;

	while (n--) {
		playerid = players[n];
		NC_GetPlayerVehicleID(playerid);
		if (nc_result == vehicleid) {
			NC_TextDrawHideForPlayer(playerid, txt_vorbar);
		}
	}
}

/**
Resets nav indicators for all players in given vehicle.
*/
void panel_reset_nav_for_passengers(AMX *amx, int vehicleid)
{
	/*TODO: use on_player_was_afk to hide VOR bar to passengers that
	were afk while the pilot disabled VOR,
	then can change to panelplayers*/
	int playerid, n = playercount;

	while (n--) {
		playerid = players[n];
		NC_GetPlayerVehicleID(playerid);
		if (nc_result == vehicleid) {
			panel_reset_nav(amx, playerid);
		}
	}
}

void panel_on_player_state_change(AMX *amx, int playerid, int from, int to)
{
	int nav_get_active_type(int);

	int vehicleid;

	if (to == PLAYER_STATE_DRIVER || to == PLAYER_STATE_PASSENGER) {
		NC_GetPlayerVehicleID_(playerid, &vehicleid);
		if (!vehicleid) {
			return;
		}
		NC_GetVehicleModel(vehicleid);
		if (!game_is_air_vehicle(nc_result)) {
			return;
		}

		panelplayers[numpanelplayers++] = playerid;
		panel_reset_nav(amx, playerid);

		amx_SetUString(buf144, "_", 2);
		nc_params[0] = 4;
		nc_params[1] = playerid;
		*((float*) (nc_params + 2)) = -10.0f;
		*((float*) (nc_params + 3)) = -10.0f;
		nc_params[4] = buf144a;
		NC_(n_CreatePlayerTextDraw, ptxt_vai + playerid);

		nc_params[0] = 2;
		nc_params[2] = ptxt_adf_alt[playerid];
		NC(n_PlayerTextDrawShow);
		nc_params[2] = ptxt_spd_large[playerid];
		NC(n_PlayerTextDrawShow);
		nc_params[2] = ptxt_spd_small[playerid];
		NC(n_PlayerTextDrawShow);
		nc_params[2] = ptxt_spd_value[playerid];
		NC(n_PlayerTextDrawShow);
		nc_params[2] = ptxt_alt_large[playerid];
		NC(n_PlayerTextDrawShow);
		nc_params[2] = ptxt_alt_small[playerid];
		NC(n_PlayerTextDrawShow);
		nc_params[2] = ptxt_alt_value[playerid];
		NC(n_PlayerTextDrawShow);
		nc_params[2] = ptxt_hdg_meter[playerid];
		NC(n_PlayerTextDrawShow);
		nc_params[2] = ptxt_hdg_value[playerid];
		NC(n_PlayerTextDrawShow);
		nc_params[2] = ptxt_adf_dis[playerid];
		NC(n_PlayerTextDrawShow);
		nc_params[2] = ptxt_adf_alt[playerid];
		NC(n_PlayerTextDrawShow);
		nc_params[2] = ptxt_adf_crs[playerid];
		NC(n_PlayerTextDrawShow);
		nc_params[2] = ptxt_hpflodo[playerid];
		NC(n_PlayerTextDrawShow);
		nc_params[2] = txt_bg;
		NC(n_TextDrawShowForPlayer);
		nc_params[2] = txt_vai;
		NC(n_TextDrawShowForPlayer);
		nc_params[2] = txt_bg_spd;
		NC(n_TextDrawShowForPlayer);
		nc_params[2] = txt_bg_alt;
		NC(n_TextDrawShowForPlayer);
		nc_params[2] = txt_adf_labels;
		NC(n_TextDrawShowForPlayer);

		if (nav_get_active_type(vehicleid) & (NAV_VOR | NAV_ILS)) {
			nc_params[2] = txt_vorbar;
			NC(n_TextDrawShowForPlayer);
		}
	} else if (ptxt_vai[playerid] != -1 /*if panel active*/) {
		nc_params[0] = 2;
		nc_params[1] = playerid;
		nc_params[2] = ptxt_adf_alt[playerid];
		NC(n_PlayerTextDrawHide);
		nc_params[2] = ptxt_spd_large[playerid];
		NC(n_PlayerTextDrawHide);
		nc_params[2] = ptxt_spd_small[playerid];
		NC(n_PlayerTextDrawHide);
		nc_params[2] = ptxt_spd_value[playerid];
		NC(n_PlayerTextDrawHide);
		nc_params[2] = ptxt_alt_large[playerid];
		NC(n_PlayerTextDrawHide);
		nc_params[2] = ptxt_alt_small[playerid];
		NC(n_PlayerTextDrawHide);
		nc_params[2] = ptxt_alt_value[playerid];
		NC(n_PlayerTextDrawHide);
		nc_params[2] = ptxt_hdg_meter[playerid];
		NC(n_PlayerTextDrawHide);
		nc_params[2] = ptxt_hdg_value[playerid];
		NC(n_PlayerTextDrawHide);
		nc_params[2] = ptxt_adf_dis[playerid];
		NC(n_PlayerTextDrawHide);
		nc_params[2] = ptxt_adf_alt[playerid];
		NC(n_PlayerTextDrawHide);
		nc_params[2] = ptxt_adf_crs[playerid];
		NC(n_PlayerTextDrawHide);
		nc_params[2] = ptxt_hpflodo[playerid];
		NC(n_PlayerTextDrawHide);
		nc_params[2] = txt_bg;
		NC(n_TextDrawHideForPlayer);
		nc_params[2] = txt_vai;
		NC(n_TextDrawHideForPlayer);
		nc_params[2] = txt_bg_spd;
		NC(n_TextDrawHideForPlayer);
		nc_params[2] = txt_bg_alt;
		NC(n_TextDrawHideForPlayer);
		nc_params[2] = txt_adf_labels;
		NC(n_TextDrawHideForPlayer);
		nc_params[2] = txt_vorbar;
		NC(n_TextDrawHideForPlayer);
		nc_params[2] = ptxt_vai[playerid];
		NC(n_PlayerTextDrawDestroy);
		ptxt_vai[playerid] = -1;
		if (ptxt_vor[playerid] != -1) {
			nc_params[2] = ptxt_vor[playerid];
			NC(n_PlayerTextDrawDestroy);
			ptxt_vor[playerid] = -1;
		}

		panel_remove_panel_player(playerid);
	}
}

void panel_on_player_was_afk(AMX* amx, int playerid)
{
	NC_GetPlayerVehicleID(playerid);
	if (nc_result) {
		NC_GetVehicleModel(nc_result);
		if (game_is_air_vehicle(nc_result)) {
			panelplayers[numpanelplayers++] = playerid;
		}
	}
}

#define SETB144(X) amx_SetUString(buf144,X,144)

void panel_on_player_connect(AMX *amx, int playerid)
{
	float *f2, *f3, *f4;
	int *i2, *i3;

	f2 = (float*) (nc_params + 2);
	f3 = (float*) (nc_params + 3);
	f4 = (float*) (nc_params + 4);
	i2 = (int*) (nc_params + 2);
	i3 = (int*) (nc_params + 3);

	panel_reset_caches(playerid);
	ptxt_vai[playerid] = ptxt_vor[playerid] = -1;

	nc_params[0] = 4;
	nc_params[1] = playerid;
	buf144[0] = '_';
	buf144[1] = 0;
	*f2 = 220.0f;
	*f3 = 360.0f;
	NC_(n_CreatePlayerTextDraw, &ptxt_spd_large[playerid]);
	*f2 = 217.0f;
	*f3 = 380.0f;
	NC_(n_CreatePlayerTextDraw, &ptxt_spd_small[playerid]);
	*f2 = 222.0f;
	*f3 = 389.0f;
	NC_(n_CreatePlayerTextDraw, &ptxt_spd_value[playerid]);
	*f2 = 453.0f;
	*f3 = 360.0f;
	NC_(n_CreatePlayerTextDraw, &ptxt_alt_large[playerid]);
	*f2 = 442.0f;
	*f3 = 380.0f;
	NC_(n_CreatePlayerTextDraw, &ptxt_alt_small[playerid]);
	*f2 = 455.0f;
	*f3 = 389.0f;
	NC_(n_CreatePlayerTextDraw, &ptxt_alt_value[playerid]);
	*f2 = 320.0f;
	*f3 = 423.0f;
	NC_(n_CreatePlayerTextDraw, &ptxt_hdg_meter[playerid]);
	*f2 = 320.0f;
	*f3 = 420.0f;
	NC_(n_CreatePlayerTextDraw, &ptxt_hdg_value[playerid]);
	buf144[0] = '-';
	buf144[1] = 0;
	*f2 = 265.0f;
	*f3 = 360.0f;
	NC_(n_CreatePlayerTextDraw, &ptxt_adf_dis[playerid]);
	*f2 = 330.0f;
	*f3 = 360.0f;
	NC_(n_CreatePlayerTextDraw, &ptxt_adf_alt[playerid]);
	*f2 = 395.0f;
	*f3 = 360.0f;
	NC_(n_CreatePlayerTextDraw, &ptxt_adf_crs[playerid]);
	*f2 = 227.0f;
	*f3 = 381.0f;
	NC_(n_CreatePlayerTextDraw, &ptxt_hpflodo[playerid]);

	*i2 = ptxt_spd_small[playerid];
	*f3 = 0.3f;
	*f4 = 1.2f;
	NC(n_PlayerTextDrawLetterSize);
	*i2 = ptxt_alt_small[playerid];
	NC(n_PlayerTextDrawLetterSize);
	*i2 = ptxt_hdg_meter[playerid];
	*f3 = 0.22f;
	*f4 = 1.0f;
	NC(n_PlayerTextDrawLetterSize);
	*i2 = ptxt_spd_value[playerid];
	*f3 = 0.4f;
	*f4 = 1.6f;
	NC(n_PlayerTextDrawLetterSize);
	*i2 = ptxt_alt_value[playerid];
	NC(n_PlayerTextDrawLetterSize);
	*i2 = ptxt_hdg_value[playerid];
	NC(n_PlayerTextDrawLetterSize);
	*i2 = ptxt_spd_large[playerid];
	*f3 = 0.25f;
	*f4 = 1.0f;
	NC(n_PlayerTextDrawLetterSize);
	*i2 = ptxt_alt_large[playerid];
	NC(n_PlayerTextDrawLetterSize);
	*i2 = ptxt_adf_dis[playerid];
	NC(n_PlayerTextDrawLetterSize);
	*i2 = ptxt_adf_alt[playerid];
	NC(n_PlayerTextDrawLetterSize);
	*i2 = ptxt_adf_crs[playerid];
	NC(n_PlayerTextDrawLetterSize);
	*i2 = ptxt_hpflodo[playerid];
	NC(n_PlayerTextDrawLetterSize);

	nc_params[0] = 3;

	*i2 = ptxt_spd_large[playerid];
	*i3 = LARGE_METER_COLOR;
	NC(n_PlayerTextDrawColor);
	*i3 = 3;
	NC(n_PlayerTextDrawAlignment);
	*i3 = 2;
	NC(n_PlayerTextDrawFont);
	*i3 = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	*i2 = ptxt_spd_small[playerid];
	*i3 = SMALL_METER_COLOR;
	NC(n_PlayerTextDrawColor);
	*i3 = 2;
	NC(n_PlayerTextDrawAlignment);
	NC(n_PlayerTextDrawFont);
	*i3 = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	*i2 = ptxt_spd_value[playerid];
	*i3 = -1;
	NC(n_PlayerTextDrawColor);
	*i3 = 3;
	NC(n_PlayerTextDrawAlignment);
	*i3 = 2;
	NC(n_PlayerTextDrawFont);
	*i3 = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	*i2 = ptxt_alt_large[playerid];
	*i3 = LARGE_METER_COLOR;
	NC(n_PlayerTextDrawColor);
	*i3 = 3;
	NC(n_PlayerTextDrawAlignment);
	*i3 = 2;
	NC(n_PlayerTextDrawFont);
	*i3 = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	*i2 = ptxt_alt_small[playerid];
	*i3 = SMALL_METER_COLOR;
	NC(n_PlayerTextDrawColor);
	*i3 = 2;
	NC(n_PlayerTextDrawAlignment);
	NC(n_PlayerTextDrawFont);
	*i3 = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	*i2 = ptxt_alt_value[playerid];
	*i3 = -1;
	NC(n_PlayerTextDrawColor);
	*i3 = 3;
	NC(n_PlayerTextDrawAlignment);
	*i3 = 2;
	NC(n_PlayerTextDrawFont);
	*i3 = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	*i2 = ptxt_hdg_meter[playerid];
	*i3 = LARGE_METER_COLOR;
	NC(n_PlayerTextDrawColor);
	*i3 = 2;
	NC(n_PlayerTextDrawAlignment);
	NC(n_PlayerTextDrawFont);
	*i3 = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	*i2 = ptxt_hdg_value[playerid];
	*i3 = -1;
	NC(n_PlayerTextDrawColor);
	*i3 = 2;
	NC(n_PlayerTextDrawAlignment);
	NC(n_PlayerTextDrawFont);
	*i3 = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	*i2 = ptxt_adf_dis[playerid];
	*i3 = 0xFF00FFFF;
	NC(n_PlayerTextDrawColor);
	*i3 = 2;
	NC(n_PlayerTextDrawAlignment);
	NC(n_PlayerTextDrawFont);
	*i3 = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);

	*i2 = ptxt_adf_alt[playerid];
	*i3 = 0xFF00FFFF;
	NC(n_PlayerTextDrawColor);
	*i3 = 2;
	NC(n_PlayerTextDrawAlignment);
	NC(n_PlayerTextDrawFont);
	*i3 = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);

	*i2 = ptxt_adf_crs[playerid];
	*i3 = 0xFF00FFFF;
	NC(n_PlayerTextDrawColor);
	*i3 = 2;
	NC(n_PlayerTextDrawAlignment);
	NC(n_PlayerTextDrawFont);
	*i3 = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);

	*i2 = ptxt_hpflodo[playerid];
	*i3 = -1;
	NC(n_PlayerTextDrawColor);
	*i3 = 2;
	NC(n_PlayerTextDrawFont);
	*i3 = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	/*
	tmp = playerpnltxt[playerid][PNLTXT_SPD_METER] =
		CreatePlayerTextDraw(playerid, 220.0, 360.0, TXT_EMPTY);
	PlayerTextDrawAlignment(playerid, tmp, 3);
	PlayerTextDrawFont(playerid, tmp, 2);
	PlayerTextDrawLetterSize(playerid, tmp, 0.25, 1.0);
	PlayerTextDrawColor(playerid, tmp, METER_COLOR);
	PlayerTextDrawSetOutline(playerid, tmp, 0);
	PlayerTextDrawSetShadow(playerid, tmp, 0);
	PlayerTextDrawSetProportional(playerid, tmp, 0);

	tmp = playerpnltxt[playerid][PNLTXT_SPD_METER2] =
		CreatePlayerTextDraw(playerid, 217.0, 380.0, TXT_EMPTY);
	PlayerTextDrawAlignment(playerid, tmp, 2);
	PlayerTextDrawFont(playerid, tmp, 2);
	PlayerTextDrawLetterSize(playerid, tmp, 0.3, 1.2);
	PlayerTextDrawColor(playerid, tmp, METER2_COLOR);
	PlayerTextDrawSetOutline(playerid, tmp, 0);
	PlayerTextDrawSetShadow(playerid, tmp, 0);
	PlayerTextDrawSetProportional(playerid, tmp, 0);

	tmp = playerpnltxt[playerid][PNLTXT_SPD] =
		CreatePlayerTextDraw(playerid, 222.0, 389.0, TXT_EMPTY);
	PlayerTextDrawAlignment(playerid, tmp, 3);
	PlayerTextDrawFont(playerid, tmp, 2);
	PlayerTextDrawLetterSize(playerid, tmp, 0.4, 1.6);
	PlayerTextDrawColor(playerid, tmp, 0xFFFFFFFF);
	PlayerTextDrawSetOutline(playerid, tmp, 0);
	PlayerTextDrawSetShadow(playerid, tmp, 0);
	PlayerTextDrawSetProportional(playerid, tmp, 0);

	tmp = playerpnltxt[playerid][PNLTXT_ALT_METER] =
		CreatePlayerTextDraw(playerid, 453.0, 360.0, TXT_EMPTY);
	PlayerTextDrawAlignment(playerid, tmp, 3);
	PlayerTextDrawFont(playerid, tmp, 2);
	PlayerTextDrawLetterSize(playerid, tmp, 0.25, 1.0);
	PlayerTextDrawColor(playerid, tmp, METER_COLOR);
	PlayerTextDrawSetOutline(playerid, tmp, 0);
	PlayerTextDrawSetShadow(playerid, tmp, 0);
	PlayerTextDrawSetProportional(playerid, tmp, 0);

	tmp = playerpnltxt[playerid][PNLTXT_ALT_METER2] =
		CreatePlayerTextDraw(playerid, 442.0, 380.0, TXT_EMPTY);
	PlayerTextDrawAlignment(playerid, tmp, 2);
	PlayerTextDrawFont(playerid, tmp, 2);
	PlayerTextDrawLetterSize(playerid, tmp, 0.3, 1.2);
	PlayerTextDrawColor(playerid, tmp, METER2_COLOR);
	PlayerTextDrawSetOutline(playerid, tmp, 0);
	PlayerTextDrawSetShadow(playerid, tmp, 0);
	PlayerTextDrawSetProportional(playerid, tmp, 0);

	tmp = playerpnltxt[playerid][PNLTXT_ALT] = CreatePlayerTextDraw(playerid, 455.0, 389.0, TXT_EMPTY);
	PlayerTextDrawAlignment(playerid, tmp, 3);
	PlayerTextDrawFont(playerid, tmp, 2);
	PlayerTextDrawLetterSize(playerid, tmp, 0.4, 1.6);
	PlayerTextDrawColor(playerid, tmp, 0xFFFFFFFF);
	PlayerTextDrawSetOutline(playerid, tmp, 0);
	PlayerTextDrawSetShadow(playerid, tmp, 0);
	PlayerTextDrawSetProportional(playerid, tmp, 0);

	tmp = playerpnltxt[playerid][PNLTXT_HDG_METER] =
		CreatePlayerTextDraw(playerid, 320.0, 423.0, TXT_EMPTY);
	PlayerTextDrawAlignment(playerid, tmp, 2);
	PlayerTextDrawFont(playerid, tmp, 2);
	PlayerTextDrawLetterSize(playerid, tmp, 0.22, 1.0);
	PlayerTextDrawColor(playerid, tmp, METER_COLOR);
	PlayerTextDrawSetOutline(playerid, tmp, 0);
	PlayerTextDrawSetShadow(playerid, tmp, 0);
	PlayerTextDrawSetProportional(playerid, tmp, 0);

	tmp = playerpnltxt[playerid][PNLTXT_HDG] =
		CreatePlayerTextDraw(playerid, 320.0, 420.0, TXT_EMPTY);
	PlayerTextDrawAlignment(playerid, tmp, 2);
	PlayerTextDrawFont(playerid, tmp, 2);
	PlayerTextDrawLetterSize(playerid, tmp, 0.4, 1.6);
	PlayerTextDrawColor(playerid, tmp, 0xFFFFFFFF);
	PlayerTextDrawSetOutline(playerid, tmp, 0);
	PlayerTextDrawSetShadow(playerid, tmp, 0);
	PlayerTextDrawSetProportional(playerid, tmp, 0);

	tmp = playerpnltxt[playerid][PNLTXT_ADF_DIS] =
		CreatePlayerTextDraw(playerid, 265.0, 360.0, "-");
	PlayerTextDrawAlignment(playerid, tmp, 2);
	PlayerTextDrawFont(playerid, tmp, 2);
	PlayerTextDrawLetterSize(playerid, tmp, 0.25, 1.0);
	PlayerTextDrawColor(playerid, tmp, 0xff00ffff);
	PlayerTextDrawSetOutline(playerid, tmp, 0);
	PlayerTextDrawSetShadow(playerid, tmp, 0);

	tmp = playerpnltxt[playerid][PNLTXT_ADF_ALT] =
		CreatePlayerTextDraw(playerid, 330.0, 360.0, "-");
	PlayerTextDrawAlignment(playerid, tmp, 2);
	PlayerTextDrawFont(playerid, tmp, 2);
	PlayerTextDrawLetterSize(playerid, tmp, 0.25, 1.0);
	PlayerTextDrawColor(playerid, tmp, 0xff00ffff);
	PlayerTextDrawSetOutline(playerid, tmp, 0);
	PlayerTextDrawSetShadow(playerid, tmp, 0);

	tmp = playerpnltxt[playerid][PNLTXT_ADF_CRS] =
		CreatePlayerTextDraw(playerid, 395.0, 360.0, "-");
	PlayerTextDrawAlignment(playerid, tmp, 2);
	PlayerTextDrawFont(playerid, tmp, 2);
	PlayerTextDrawLetterSize(playerid, tmp, 0.25, 1.0);
	PlayerTextDrawColor(playerid, tmp, 0xff00ffff);
	PlayerTextDrawSetOutline(playerid, tmp, 0);
	PlayerTextDrawSetShadow(playerid, tmp, 0);

	tmp = playerpnltxt[playerid][PNLTXT_HPFLODO] =
		CreatePlayerTextDraw(playerid, 227.0, 381.0, "-");
	PlayerTextDrawFont(playerid, tmp, 2);
	PlayerTextDrawLetterSize(playerid, tmp, 0.25, 1.0);
	PlayerTextDrawColor(playerid, tmp, 0xffffffff);
	PlayerTextDrawSetOutline(playerid, tmp, 0);
	PlayerTextDrawSetShadow(playerid, tmp, 0);
	PlayerTextDrawSetProportional(playerid, tmp, 0);

/*
	// ILS
	tmp = CreatePlayerTextDraw(playerid, 320.0, 100.0, "-")
	//"~w~X~n~~w~X~n~~w~X~n~~w~X~n~~w~X ~w~X ~w~X ~w~X ~w~X ~w~X "
	//"~w~X ~w~X ~w~X~n~~w~X~n~~w~X~n~~w~X~n~~w~X")
	PlayerTextDrawAlignment(playerid, tmp, 2)
	PlayerTextDrawBackgroundColor(playerid, tmp, 0x000000FF)
	PlayerTextDrawFont(playerid, playerid, tmp, 2)
	PlayerTextDrawLetterSize(playerid, tmp, 0.45, 2.5)
	PlayerTextDrawColor(playerid, tmp, -1)
	PlayerTextDrawSetOutline(playerid, tmp, 1)
	PlayerTextDrawSetProportional(playerid, tmp, 1)
*/
}

void panel_on_gamemode_init(AMX *amx)
{
	/*glorious write-only code*/
	int *i1, *i2, *i3;
	float *f1, *f2, *f3;
	i1 = (int*) (nc_params + 1);
	i2 = (int*) (nc_params + 2);
	i3 = (int*) (nc_params + 3);
	f1 = (float*) (nc_params + 1);
	f2 = (float*) (nc_params + 2);
	f3 = (float*) (nc_params + 3);

	/*
#define PANEL_BG 0x00000077
#define METER2_BG 0x00000077

#define TDVAR pnltxt[PNLTXT_BG]
	TDVAR = TextDrawCreate(320.0, 360.0, "~n~~n~~n~~n~~n~~n~~n~~n~");
	TextDrawAlignment(TDVAR, 2);
	TextDrawFont(TDVAR, 1);
	TextDrawLetterSize(TDVAR, 0.5, 1.0);
	TextDrawSetOutline(TDVAR, 0);
	TextDrawUseBox(TDVAR, 1);
	TextDrawBoxColor(TDVAR, PANEL_BG);
	TextDrawTextSize(TDVAR, 100.0, 271.0);
#undef TDVAR

#define TDVAR pnltxt[PNLTXT_VAI_METER]
	TDVAR = TextDrawCreate(461.0, 364.0,
		"-2~n~-_~n~-1~n~-_~n~-0~n~-_~n~-1~n~-_~n~-2");
	TextDrawAlignment(TDVAR, 2);
	TextDrawFont(TDVAR, 2);
	TextDrawLetterSize(TDVAR, 0.2, 0.8);
	TextDrawColor(TDVAR, 0xFFFFFFFF);
	TextDrawSetOutline(TDVAR, 0);
	TextDrawSetShadow(TDVAR, 0);
	TextDrawSetProportional(TDVAR, 1);
	TextDrawUseBox(TDVAR, 1);
	TextDrawBoxColor(TDVAR, PANEL_BG);
	TextDrawTextSize(TDVAR, 476.0, 7.0);
#undef TDVAR

#define TDVAR pnltxt[PNLTXT_BG_SPD]
	TDVAR = TextDrawCreate(203.0, 383.0, "~n~~n~~n~");
	TextDrawAlignment(TDVAR, 2);
	TextDrawFont(TDVAR, 1);
	TextDrawLetterSize(TDVAR, 0.3, 1.0);
	TextDrawSetOutline(TDVAR, 0);
	TextDrawUseBox(TDVAR, 1);
	TextDrawBoxColor(TDVAR, METER2_BG);
	TextDrawTextSize(TDVAR, 100.0, 35.0);
#undef TDVAR

#define TDVAR pnltxt[PNLTXT_BG_ALT]
	TDVAR = TextDrawCreate(436.0, 383.0, "~n~~n~~n~");
	TextDrawAlignment(TDVAR, 2);
	TextDrawFont(TDVAR, 1);
	TextDrawLetterSize(TDVAR, 0.3, 1.0);
	TextDrawSetOutline(TDVAR, 0);
	TextDrawUseBox(TDVAR, 1);
	TextDrawBoxColor(TDVAR, METER2_BG);
	TextDrawTextSize(TDVAR, 100.0, 35.0);
#undef TDVAR

#define TDVAR pnltxt[PNLTXT_ADF]
	TDVAR = TextDrawCreate(227.0, 360.0,
		"DIS_______________ALT_______________CRS");
	TextDrawFont(TDVAR, 2);
	TextDrawLetterSize(TDVAR, 0.25, 1.0);
	TextDrawColor(TDVAR, 0xFFFFFFFF);
	TextDrawSetOutline(TDVAR, 0);
	TextDrawSetShadow(TDVAR, 0);
	TextDrawSetProportional(TDVAR, 1);
#undef TDVAR

#define TDVAR vorbar
	TDVAR = TextDrawCreate(320.0, 410.0,
		"O_____O_____O_____-_____O_____O_____O")
	TextDrawFont(TDVAR, 1)
	TextDrawAlignment(TDVAR, 2)
	TextDrawLetterSize(TDVAR, 0.25, 1.0)
	TextDrawColor(TDVAR, 0xFFFFFFFF)
	TextDrawSetOutline(TDVAR, 0)
	TextDrawSetShadow(TDVAR, 0)
	TextDrawSetProportional(TDVAR, 1)
	TextDrawUseBox(TDVAR, 1)
	TextDrawBoxColor(TDVAR, 0x66)
	TextDrawTextSize(TDVAR, 0.0, 170.0)
#undef TDVAR
	*/

	nc_params[0] = 3;
	*f1 = 320.0f;
	*f2 = 360.0f;
	*i3 = buf144a;
	SETB144("~n~~n~~n~~n~~n~~n~~n~~n~");
	NC_(n_TextDrawCreate, &txt_bg);

	*f2 = 410.0f;
	SETB144("O_____O_____O_____-_____O_____O_____O");
	NC_(n_TextDrawCreate, &txt_vorbar);

	*f1 = 461.0f;
	*f2 = 364.0f;
	SETB144("-2~n~-_~n~-1~n~-_~n~-0~n~-_~n~-1~n~-_~n~-2");
	NC_(n_TextDrawCreate, &txt_vai);

	*f1 = 203.0f;
	*f2 = 383.0f;
	SETB144("~n~~n~~n~");
	NC_(n_TextDrawCreate, &txt_bg_spd);

	*f1 = 436.0f;
	NC_(n_TextDrawCreate, &txt_bg_alt);

	*f1 = 227.0f;
	*f2 = 360.0f;
	SETB144("DIS_______________ALT_______________CRS");
	NC_(n_TextDrawCreate, &txt_adf_labels);

	/*x y txt*/
	*f2 = 0.5f;  *f3 = 1.0f; *i1 = txt_bg; NC(n_TextDrawLetterSize);
	*f2 = 0.25f;
	                         *i1 = txt_adf_labels; NC(n_TextDrawLetterSize);
	                         *i1 = txt_vorbar; NC(n_TextDrawLetterSize);
	*f2 = 0.3f;
	                         *i1 = txt_bg_alt; NC(n_TextDrawLetterSize);
	                         *i1 = txt_bg_spd; NC(n_TextDrawLetterSize);
	*f2 = 0.2f;  *f3 = 0.8f; *i1 = txt_vai; NC(n_TextDrawLetterSize);

	/*x y txt*/
	*f2 = 100.0f; *f3 = 271.0f; *i1 = txt_bg; NC(n_TextDrawTextSize);
	              *f3 =  35.0f; *i1 = txt_bg_alt; NC(n_TextDrawTextSize);
	                            *i1 = txt_bg_spd; NC(n_TextDrawTextSize);
	*f2 = 476.0f; *f3 = 7.0f;   *i1 = txt_vai; NC(n_TextDrawTextSize);
	*f2 = 0.0f;   *f3 = 170.0f; *i1 = txt_vorbar; NC(n_TextDrawTextSize);

	nc_params[0] = 2;

	/*boxcolor txt col*/
	*i2 = SMALL_METER_BG;
	*i1 = txt_bg_alt; NC(n_TextDrawBoxColor);
	*i1 = txt_bg_spd; NC(n_TextDrawBoxColor);
	*i2 = PANEL_BG;
	*i1 = txt_bg; NC(n_TextDrawBoxColor);
	*i1 = txt_vai; NC(n_TextDrawBoxColor);
	*i2 = VORBAR_BG;
	*i1 = txt_vorbar; NC(n_TextDrawBoxColor);

	/*clustertruck*/
	*i2 = TD_ALIGNMENT_CENTER;
	*i1 = txt_bg; NC(n_TextDrawAlignment);
	*i1 = txt_vorbar; NC(n_TextDrawAlignment);
	*i1 = txt_vai; NC(n_TextDrawAlignment);
	*i1 = txt_bg_spd; NC(n_TextDrawAlignment);
	*i1 = txt_bg_alt; NC(n_TextDrawAlignment);

	*i2 = -1;
	*i1 = txt_vai; NC(n_TextDrawColor);
	*i1 = txt_vorbar; NC(n_TextDrawColor);
	*i1 = txt_adf_labels; NC(n_TextDrawColor);

	*i2 = 0;
	*i1 = txt_bg; NC(n_TextDrawSetOutline);
	*i1 = txt_bg_spd; NC(n_TextDrawSetOutline);
	*i1 = txt_bg_alt; NC(n_TextDrawSetOutline);
	*i1 = txt_vai; NC(n_TextDrawSetOutline); NC(n_TextDrawSetShadow);
	*i1 = txt_vorbar; NC(n_TextDrawSetOutline); NC(n_TextDrawSetShadow);
	*i1 = txt_adf_labels; NC(n_TextDrawSetOutline); NC(n_TextDrawSetShadow);

	*i2 = 1;
	*i1 = txt_bg; NC(n_TextDrawUseBox); NC(n_TextDrawFont);
	*i1 = txt_bg_spd; NC(n_TextDrawUseBox); NC(n_TextDrawFont);
	*i1 = txt_bg_alt; NC(n_TextDrawUseBox); NC(n_TextDrawFont);
	*i1 = txt_adf_labels; NC(n_TextDrawSetProportional);
	*i1 = txt_vai; NC(n_TextDrawSetProportional); NC(n_TextDrawUseBox);
	*i1 = txt_vorbar; NC(n_TextDrawSetProportional); NC(n_TextDrawUseBox);
		NC(n_TextDrawFont);

	*i2 = 2;
	*i1 = txt_vai; NC(n_TextDrawFont);
	*i1 = txt_adf_labels; NC(n_TextDrawFont);
}

