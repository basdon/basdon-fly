
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "a_samp.h"
#include "airport.h"
#include "game_sa.h"
#include "kneeboard.h"
#include "panel.h"
#include "nav.h"
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
/**
Vertical Airspeed Indicator value indicator player textdraw.
*/
static int ptxt_vai[MAX_PLAYERS];
/**
VOR value indicator player textdraw.
*/
static int ptxt_vor[MAX_PLAYERS];
/**
ILS indicator player textdraw.
*/
static int ptxt_ils[MAX_PLAYERS];

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
static
void panel_reset_caches(int playerid)
{
	memset(&caches[playerid], 0xFF, sizeof(struct PANELCACHE));
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
static
void panel_update_altitude(int playerid, int altitude)
{
	int altitude50 = altitude / 50;
	int i, tmp, tmp2;
	char buf[50];
	struct PANELCACHE *pcache = caches + playerid;

	if (altitude == pcache->altitude) {
		return;
	}
	pcache->altitude = altitude;

	NC_PARS(3);
	nc_params[1] = playerid;
	nc_params[3] = buf144a;

	/* alt */
	csprintf(buf144, "%03d", altitude);
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
		B144(buf);
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
		B144(buf);
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
static
void panel_update_speed(int playerid, int speed)
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

	NC_PARS(3);
	nc_params[1] = playerid;
	nc_params[3] = buf144a;

	/* spd */
	csprintf(buf144, "%03d", speed);
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
		B144(buf);
		nc_params[2] = ptxt_spd_large[playerid];
		NC(n_PlayerTextDrawSetString);
	}

	/* spd meter small */
	strcpy(buf, "0~n~~n~_");
	buf[0] = '0' + ((speed + 1) % 10);
	if (speed != 0) {
		buf[7] = '0' + ((speed + 9) % 10);
	}
	B144(buf);
	nc_params[2] = ptxt_spd_small[playerid];
	NC(n_PlayerTextDrawSetString);
}

/**
Update heading textdraws.
*/
static
void panel_update_heading(int playerid, int heading)
{
	/* normalize before cache, because rounding (0 & 360)*/
	heading = 360 - heading;
	if (heading == 0) {
		heading = 360;
	}

	if (heading == caches[playerid].heading) {
		return;
	}
	caches[playerid].heading = heading;

	NC_PARS(3);
	nc_params[1] = playerid;
	nc_params[3] = buf144a;

	/* hdg */
	csprintf(buf144, "%03d", heading);
	nc_params[2] = ptxt_hdg_value[playerid];
	NC(n_PlayerTextDrawSetString);

	/* hdg meter */
	heading--;
	csprintf(buf144,
		"%03d_%03d_%03d________%03d_%03d_%03d",
		((heading + 357) % 360) + 1,
		((heading + 358) % 360) + 1,
		((heading + 359) % 360) + 1,
		((heading + 1) % 360) + 1,
		((heading + 2) % 360) + 1,
		((heading + 3) % 360) + 1);
	nc_params[2] = ptxt_hdg_meter[playerid];
	NC(n_PlayerTextDrawSetString);
}

void panel_timed_update()
{
	struct vec3 vpos;
	int playerid, vehicleid, n = numpanelplayers;
	float x, y, z, heading;

	while (n--) {
		playerid = panelplayers[n];

		vehicleid = NC_GetPlayerVehicleID(playerid);

		common_GetVehiclePos(vehicleid, &vpos);
		kneeboard_update_distance(playerid, &vpos);
		panel_update_altitude(playerid, (int) vpos.z);

		NC_GetVehicleZAngle(vehicleid, buf144a);
		heading = *fbuf144;
		panel_update_heading(playerid, (int) heading);

		if (NC_GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
			nav_update(vehicleid, &vpos, heading);
		}

		nav_update_textdraws(
			playerid,
			vehicleid,
			ptxt_adf_dis,
			ptxt_adf_alt,
			ptxt_adf_crs,
			ptxt_vor,
			ptxt_ils);

		/*vai*/
		NC_GetVehicleVelocity(vehicleid, buf32a, buf64a, buf144a);
		x = *fbuf32;
		y = *fbuf64;
		z = *fbuf144;
		x = VEL_TO_KTS * sqrt(x * x + y * y + z * z);
		panel_update_speed(playerid, (int) x);

		z *= VEL_TO_KFPM * 14.5f;
		if (z < -34.0f) {
			z = -34.0f;
		} else if (z > 34.0f) {
			z = 34.0f;
		}
		NC_PARS(2);
		nc_params[1] = playerid;
		nc_params[2] = ptxt_vai[playerid];
		NC(n_PlayerTextDrawDestroy);
		NC_PARS(4);
		buf144[0] = '~';
		buf144[1] = '<';
		buf144[2] = '~';
		buf144[3] = 0;
		nc_params[1] = playerid;
		nc_paramf[2] = 458.0f;
		nc_paramf[3] = 391.0f - z;
		nc_params[4] = buf144a;
		ptxt_vai[playerid] = NC(n_CreatePlayerTextDraw);
		nc_params[2] = ptxt_vai[playerid];
		nc_paramf[3] = 0.25f;
		nc_paramf[4] = 1.1f;
		NC(n_PlayerTextDrawLetterSize);
		NC_PARS(3);
		nc_params[3] = 0xFF0000FF;
		NC(n_PlayerTextDrawColor);
		nc_params[3] = 2;
		NC(n_PlayerTextDrawAlignment);
		NC(n_PlayerTextDrawFont);
		nc_params[3] = 0;
		NC(n_PlayerTextDrawSetOutline);
		NC(n_PlayerTextDrawSetShadow);
		NC_PARS(2);
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

/**
Reset nav textdraws, meaning dashes for values and hide vor/ils textdraws.
*/
static
void panel_reset_nav(int playerid)
{
	if (ptxt_vai[playerid] != -1) {
		buf144[0] = '-';
		buf144[1] = 0;
		NC_PARS(3);
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
		if (ptxt_ils[playerid] != -1) {
			nc_params[2] = ptxt_ils[playerid];
			NC(n_PlayerTextDrawHide);
			ptxt_ils[playerid] = -1;
		}
		nc_params[2] = txt_vorbar;
		NC(n_TextDrawHideForPlayer);
	}
	panel_reset_caches(playerid);
}

void panel_show_vor_bar_for_passengers(int vehicleid)
{
	/*TODO: use on_player_was_afk to show VOR bar to passengers that
	were afk while the pilot enabled VOR,
	then can change to panelplayers*/
	int playerid, n = playercount;

	while (n--) {
		playerid = players[n];
		if (NC_GetPlayerVehicleID(playerid) == vehicleid) {
			NC_TextDrawShowForPlayer(playerid, txt_vorbar);
		}
	}
}

void panel_hide_vor_bar_for_passengers(int vehicleid)
{
	/*TODO: use on_player_was_afk to hide VOR bar to passengers that
	were afk while the pilot disabled VOR,
	then can change to panelplayers*/
	int pid, n = playercount;

	while (n--) {
		pid = players[n];
		if (NC_GetPlayerVehicleID(pid) == vehicleid) {
			NC_TextDrawHideForPlayer(pid, txt_vorbar);
			if (ptxt_vor[pid] != -1) {
				NC_PlayerTextDrawHide(pid, ptxt_vor[pid]);
			}
		}
	}
}

void panel_show_ils_for_passengers(int vehicleid)
{
	/*TODO: use on_player_was_afk to hide VOR bar to passengers that
	were afk while the pilot disabled VOR,
	then can change to panelplayers*/
	int pid, n = playercount;

	while (n--) {
		pid = players[n];
		if (NC_GetPlayerVehicleID(pid) == vehicleid) {
			NC_PlayerTextDrawShow(pid, ptxt_ils[pid]);
		}
	}
}

void panel_hide_ils_for_passengers(int vehicleid)
{
	/*TODO: use on_player_was_afk to hide VOR bar to passengers that
	were afk while the pilot disabled VOR,
	then can change to panelplayers*/
	int pid, n = playercount;

	while (n--) {
		pid = players[n];
		if (NC_GetPlayerVehicleID(pid) == vehicleid) {
			NC_PlayerTextDrawHide(pid, ptxt_ils[pid]);
		}
	}
}

void panel_reset_nav_for_passengers(int vehicleid)
{
	/*TODO: use on_player_was_afk to hide VOR bar to passengers that
	were afk while the pilot disabled VOR,
	then can change to panelplayers*/
	int playerid, n = playercount;

	while (n--) {
		playerid = players[n];
		if (NC_GetPlayerVehicleID(playerid) == vehicleid) {
			panel_reset_nav(playerid);
		}
	}
}

void panel_on_player_state_change(int playerid, int from, int to)
{
	int nav_get_active_type(int);
	void nav_reset_cache(int);

	int vehicleid, activenav;

	if (to == PLAYER_STATE_DRIVER || to == PLAYER_STATE_PASSENGER) {
		vehicleid = NC_GetPlayerVehicleID(playerid);
		if (!vehicleid) {
			return;
		}
		if (!game_is_air_vehicle(NC_GetVehicleModel(vehicleid))) {
			return;
		}

		panelplayers[numpanelplayers++] = playerid;
		panel_reset_nav(playerid);
		nav_reset_cache(playerid);

		buf144[0] = '_';
		buf144[1] = 0;
		NC_PARS(4);
		nc_params[1] = playerid;
		nc_paramf[2] = -10.0f;
		nc_paramf[3] = -10.0f;
		nc_params[4] = buf144a;
		ptxt_vai[playerid] = NC(n_CreatePlayerTextDraw);

		NC_PARS(2);
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

		buf144[0] = '-';
		buf144[1] = 0;
		nc_params[2] = ptxt_adf_dis[playerid];
		nc_params[3] = buf144a;
		NC(n_PlayerTextDrawSetString);
		nc_params[2] = ptxt_adf_alt[playerid];
		NC(n_PlayerTextDrawSetString);
		nc_params[2] = ptxt_adf_crs[playerid];
		NC(n_PlayerTextDrawSetString);

		activenav = nav_get_active_type(vehicleid);
		if (activenav & NAV_ILS) {
			nc_params[2] = ptxt_ils[playerid];
			NC(n_PlayerTextDrawShow);
		}
		if (activenav & (NAV_ILS | NAV_VOR)) {
			nc_params[2] = txt_vorbar;
			NC(n_TextDrawShowForPlayer);
		}
	} else if (ptxt_vai[playerid] != -1 /*if panel active*/) {
		NC_PARS(2);
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
		nc_params[2] = ptxt_ils[playerid];
		NC(n_PlayerTextDrawHide);

		panel_remove_panel_player(playerid);
	}
}

void panel_on_player_was_afk(int playerid)
{
	int vehicleid, vehiclemodel;
	vehicleid = NC_GetPlayerVehicleID(playerid);
	if (vehicleid) {
		vehiclemodel = NC_GetVehicleModel(vehicleid);
		if (game_is_air_vehicle(vehiclemodel)) {
			panelplayers[numpanelplayers++] = playerid;
		}
	}
}

void panel_on_player_connect(int playerid)
{
	panel_reset_caches(playerid);
	ptxt_vai[playerid] = ptxt_vor[playerid] = ptxt_ils[playerid] = -1;

	NC_PARS(4);
	nc_params[1] = playerid;
	buf144[0] = '_';
	buf144[1] = 0;
	nc_params[4] = buf144a;
	nc_paramf[2] = 220.0f;
	nc_paramf[3] = 360.0f;
	ptxt_spd_large[playerid] = NC(n_CreatePlayerTextDraw);
	nc_paramf[2] = 217.0f;
	nc_paramf[3] = 380.0f;
	ptxt_spd_small[playerid] = NC(n_CreatePlayerTextDraw);
	nc_paramf[2] = 222.0f;
	nc_paramf[3] = 389.0f;
	ptxt_spd_value[playerid] = NC(n_CreatePlayerTextDraw);
	nc_paramf[2] = 453.0f;
	nc_paramf[3] = 360.0f;
	ptxt_alt_large[playerid] = NC(n_CreatePlayerTextDraw);
	nc_paramf[2] = 442.0f;
	nc_paramf[3] = 380.0f;
	ptxt_alt_small[playerid] = NC(n_CreatePlayerTextDraw);
	nc_paramf[2] = 455.0f;
	nc_paramf[3] = 389.0f;
	ptxt_alt_value[playerid] = NC(n_CreatePlayerTextDraw);
	nc_paramf[2] = 320.0f;
	nc_paramf[3] = 100.0f;
	ptxt_ils[playerid] = NC(n_CreatePlayerTextDraw);
	nc_paramf[3] = 423.0f;
	ptxt_hdg_meter[playerid] = NC(n_CreatePlayerTextDraw);
	nc_paramf[3] = 420.0f;
	ptxt_hdg_value[playerid] = NC(n_CreatePlayerTextDraw);
	nc_paramf[2] = 265.0f;
	nc_paramf[3] = 360.0f;
	ptxt_adf_dis[playerid] = NC(n_CreatePlayerTextDraw);
	nc_paramf[2] = 330.0f;
	nc_paramf[3] = 360.0f;
	ptxt_adf_alt[playerid] = NC(n_CreatePlayerTextDraw);
	nc_paramf[2] = 395.0f;
	nc_paramf[3] = 360.0f;
	ptxt_adf_crs[playerid] = NC(n_CreatePlayerTextDraw);

	/*letter size*/
	nc_params[2] = ptxt_spd_small[playerid];
	nc_paramf[3] = 0.3f;
	nc_paramf[4] = 1.2f;
	NC(n_PlayerTextDrawLetterSize);
	nc_params[2] = ptxt_alt_small[playerid];
	NC(n_PlayerTextDrawLetterSize);
	nc_params[2] = ptxt_hdg_meter[playerid];
	nc_paramf[3] = 0.22f;
	nc_paramf[4] = 1.0f;
	NC(n_PlayerTextDrawLetterSize);
	nc_params[2] = ptxt_spd_value[playerid];
	nc_paramf[3] = 0.4f;
	nc_paramf[4] = 1.6f;
	NC(n_PlayerTextDrawLetterSize);
	nc_params[2] = ptxt_alt_value[playerid];
	NC(n_PlayerTextDrawLetterSize);
	nc_params[2] = ptxt_hdg_value[playerid];
	NC(n_PlayerTextDrawLetterSize);
	nc_params[2] = ptxt_spd_large[playerid];
	nc_paramf[3] = 0.25f;
	nc_paramf[4] = 1.0f;
	NC(n_PlayerTextDrawLetterSize);
	nc_params[2] = ptxt_alt_large[playerid];
	NC(n_PlayerTextDrawLetterSize);
	nc_params[2] = ptxt_adf_dis[playerid];
	NC(n_PlayerTextDrawLetterSize);
	nc_params[2] = ptxt_adf_alt[playerid];
	NC(n_PlayerTextDrawLetterSize);
	nc_params[2] = ptxt_adf_crs[playerid];
	NC(n_PlayerTextDrawLetterSize);
	nc_params[2] = ptxt_ils[playerid];
	nc_paramf[3] = 0.45f;
	nc_paramf[4] = 2.5f;
	NC(n_PlayerTextDrawLetterSize);

	NC_PARS(3);
	/*rest*/
	nc_params[2] = ptxt_spd_large[playerid];
	nc_params[3] = LARGE_METER_COLOR;
	NC(n_PlayerTextDrawColor);
	nc_params[3] = 3;
	NC(n_PlayerTextDrawAlignment);
	nc_params[3] = 2;
	NC(n_PlayerTextDrawFont);
	nc_params[3] = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	nc_params[2] = ptxt_spd_small[playerid];
	nc_params[3] = SMALL_METER_COLOR;
	NC(n_PlayerTextDrawColor);
	nc_params[3] = 2;
	NC(n_PlayerTextDrawAlignment);
	NC(n_PlayerTextDrawFont);
	nc_params[3] = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	nc_params[2] = ptxt_spd_value[playerid];
	nc_params[3] = -1;
	NC(n_PlayerTextDrawColor);
	nc_params[3] = 3;
	NC(n_PlayerTextDrawAlignment);
	nc_params[3] = 2;
	NC(n_PlayerTextDrawFont);
	nc_params[3] = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	nc_params[2] = ptxt_alt_large[playerid];
	nc_params[3] = LARGE_METER_COLOR;
	NC(n_PlayerTextDrawColor);
	nc_params[3] = 3;
	NC(n_PlayerTextDrawAlignment);
	nc_params[3] = 2;
	NC(n_PlayerTextDrawFont);
	nc_params[3] = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	nc_params[2] = ptxt_alt_small[playerid];
	nc_params[3] = SMALL_METER_COLOR;
	NC(n_PlayerTextDrawColor);
	nc_params[3] = 2;
	NC(n_PlayerTextDrawAlignment);
	NC(n_PlayerTextDrawFont);
	nc_params[3] = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	nc_params[2] = ptxt_alt_value[playerid];
	nc_params[3] = -1;
	NC(n_PlayerTextDrawColor);
	nc_params[3] = 3;
	NC(n_PlayerTextDrawAlignment);
	nc_params[3] = 2;
	NC(n_PlayerTextDrawFont);
	nc_params[3] = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	nc_params[2] = ptxt_hdg_meter[playerid];
	nc_params[3] = LARGE_METER_COLOR;
	NC(n_PlayerTextDrawColor);
	nc_params[3] = 2;
	NC(n_PlayerTextDrawAlignment);
	NC(n_PlayerTextDrawFont);
	nc_params[3] = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	nc_params[2] = ptxt_hdg_value[playerid];
	nc_params[3] = -1;
	NC(n_PlayerTextDrawColor);
	nc_params[3] = 2;
	NC(n_PlayerTextDrawAlignment);
	NC(n_PlayerTextDrawFont);
	nc_params[3] = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawSetProportional);

	nc_params[2] = ptxt_adf_dis[playerid];
	nc_params[3] = 0xFF00FFFF;
	NC(n_PlayerTextDrawColor);
	nc_params[3] = 2;
	NC(n_PlayerTextDrawAlignment);
	NC(n_PlayerTextDrawFont);
	nc_params[3] = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);

	nc_params[2] = ptxt_adf_alt[playerid];
	nc_params[3] = 0xFF00FFFF;
	NC(n_PlayerTextDrawColor);
	nc_params[3] = 2;
	NC(n_PlayerTextDrawAlignment);
	NC(n_PlayerTextDrawFont);
	nc_params[3] = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);

	nc_params[2] = ptxt_adf_crs[playerid];
	nc_params[3] = 0xFF00FFFF;
	NC(n_PlayerTextDrawColor);
	nc_params[3] = 2;
	NC(n_PlayerTextDrawAlignment);
	NC(n_PlayerTextDrawFont);
	nc_params[3] = 0;
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetShadow);

	nc_params[2] = ptxt_ils[playerid];
	nc_params[3] = 2;
	NC(n_PlayerTextDrawAlignment);
	NC(n_PlayerTextDrawFont);
	nc_params[3] = 1;
	NC(n_PlayerTextDrawSetOutline);
}

void panel_on_gamemode_init()
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

	NC_PARS(3);
	*f1 = 320.0f;
	*f2 = 360.0f;
	*i3 = buf144a;
	B144("~n~~n~~n~~n~~n~~n~~n~~n~");
	txt_bg = NC(n_TextDrawCreate);

	*f2 = 410.0f;
	B144("O_____O_____O_____-_____O_____O_____O");
	txt_vorbar = NC(n_TextDrawCreate);

	*f1 = 461.0f;
	*f2 = 364.0f;
	B144("-2~n~-_~n~-1~n~-_~n~-0~n~-_~n~-1~n~-_~n~-2");
	txt_vai = NC(n_TextDrawCreate);

	*f1 = 203.0f;
	*f2 = 383.0f;
	B144("~n~~n~~n~");
	txt_bg_spd = NC(n_TextDrawCreate);

	*f1 = 436.0f;
	txt_bg_alt = NC(n_TextDrawCreate);

	*f1 = 227.0f;
	*f2 = 360.0f;
	B144("DIS_______________ALT_______________CRS");
	txt_adf_labels = NC(n_TextDrawCreate);

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

	NC_PARS(2);

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

