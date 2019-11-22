
/* vim: set filetype=c ts=8 noexpandtab: */

#include "a_samp.h"
#include "common.h"
#include "game_sa.h"
#include "playerdata.h"
#include "playtime.h"
#include "zones.h"
#include <string.h>
#include <math.h>

#define IN_ZONES
#include "zones_data.c"

static int lastzoneid[MAX_PLAYERS];
static int lastregionid[MAX_PLAYERS];
static int lastzoneindex[MAX_PLAYERS];

/**
Holds player text draw id for each player.
*/
static int ptextid[MAX_PLAYERS];

void zones_init()
{
	struct REGION *r = regions, *rmax = regions + regioncount;
	struct ZONE *pz = zones;
	int z = 0, zmax = zonecount;
#ifdef ZONES_DEBUG
	int zonesinregion = 0;
#endif /*ZONES_DEBUG*/

	while (z < zmax) {
nr:
		if (pz->x1 < r->zone.x1 || r->zone.x2 < pz->x2 ||
			pz->y1 < r->zone.y1 || r->zone.y2 < pz->y2)
		{
#ifdef ZONES_DEBUG
			printf("%d zones in region %d\n", zonesinregion,
				r - regions);
			zonesinregion = 0;
#endif /*ZONES_DEBUG*/
			r->maxzone = z;
			r++;
			if (r >= rmax) {
				logprintf("zones.c error: at zone %d but "
					"regions are depleted", z);
				return;
			}
			r->minzone = z;
			goto nr;
		}
		z++;
		pz++;
#ifdef ZONES_DEBUG
		zonesinregion++;
#endif /*ZONES_DEBUG*/
	}
	r->maxzone = z;

	for (z = 0; z < MAX_PLAYERS; z++) {
		lastzoneindex[z] = lastzoneid[z] = -1;
		lastregionid[z] = ZONE_INVALID;
	}
}

/**
Check if given position is inside given zone
*/
static
int zones_is_in_zone(struct vec3 pos, struct ZONE *zone)
{
	return zone->x1 <= pos.x && pos.x <= zone->x2 &&
		zone->y1 <= pos.y && pos.y <= zone->y2 &&
		zone->z1 <= pos.z && pos.z <= zone->z2;
}

void zones_on_player_connect(int playerid)
{
	lastzoneindex[playerid] = lastzoneid[playerid] = -1;
	lastregionid[playerid] = ZONE_INVALID;

	NC_PARS(4);
	nc_params[1] = playerid;
	nc_paramf[2] = 88.0f; /*x*/
	nc_paramf[3] = 320.0f; /*y*/
	nc_params[4] = emptystringa; /*txt*/
	ptextid[playerid] = NC(n_CreatePlayerTextDraw);

	nc_params[2] = ptextid[playerid];
	nc_paramf[3] = 0.3f; /*x*/
	nc_paramf[4] = 1.0f; /*y*/
	NC(n_PlayerTextDrawLetterSize);

	NC_PARS(3);
	nc_params[3] = TD_ALIGNMENT_CENTER;
	NC(n_PlayerTextDrawAlignment);

	nc_params[3] = 1;
	NC(n_PlayerTextDrawFont);
	NC(n_PlayerTextDrawSetOutline);
	NC(n_PlayerTextDrawSetProportional);

	nc_params[3] = 0;
	NC(n_PlayerTextDrawSetShadow);

	nc_params[3] = -1;
	NC(n_PlayerTextDrawColor);
}

void zones_on_player_spawn(int playerid, struct vec3 pos)
{
	NC_PlayerTextDrawShow(playerid, ptextid[playerid]);
	zones_update(playerid, pos);
}

void zones_hide_text(int playerid)
{
	NC_PlayerTextDrawHide(playerid, ptextid[playerid]);
}

void zones_update(int playerid, struct vec3 pos)
{
	struct REGION *r = regions, *rmax = regions + regioncount;
	struct ZONE *pz;
	int i;
	int lrid = lastregionid[playerid];
	int lzid = lastzoneid[playerid];
	char buf[144];

	if (lastzoneindex[playerid] >= 0 &&
		zones_is_in_zone(pos, zones + lastzoneindex[playerid]))
	{
		return;
	}

	lastzoneindex[playerid] = -1;
	r = regions;
	rmax = regions + regioncount;

	while (r < rmax) {
		if (zones_is_in_zone(pos, &r->zone)) {
			lastregionid[playerid] = r->zone.id;
			for (i = r->minzone; i < r->maxzone; i++) {
				pz = zones + i;
				if (zones_is_in_zone(pos, pz)) {
					lastzoneindex[playerid] = i;
					lastzoneid[playerid] = pz->id;
					goto gotcha;
				}
			}
			lastzoneid[playerid] = -1;
			goto gotcha;
		}
		r++;
	}

	lastregionid[playerid] =
		ZONE_NONE_NW + ((pos.y < 0.0f) << 1) + (pos.x > 0.0f);
gotcha:
	if (lrid != lastregionid[playerid] || lzid != lastzoneid[playerid]) {
		if (lastzoneid[playerid] != -1) {
			sprintf(buf, "%s~n~%s", zonenames[lastzoneid[playerid]],
				zonenames[lastregionid[playerid]]);
		} else {
			/*regionid should _always_ be valid*/
			buf[0] = buf[2] = '~';
			buf[1] = 'n';
			strcpy(buf + 3, zonenames[lastregionid[playerid]]);
		}
		amx_SetUString(buf144, buf, 144);
		NC_PlayerTextDrawSetString(playerid, ptextid[playerid],
			buf144a);
	}
}

void zones_update_for_all()
{
	int idx, playerid;
	struct vec3 pos;

	idx = playercount;
	while (idx--) {
		playerid = players[idx];
		if (!isafk[playerid]) {
			common_GetPlayerPos(playerid, &pos);
			zones_update(playerid, pos);
		}
	}
}

int zones_cmd_loc(CMDPARAMS)
{
	static const char *CMD_SYN = WARN"Syntax: /loc [id/part of name]";
	static const char *CMD_NOT_ONLINE = WARN"That player is not online.";
	int target, vehicleid, model;
	struct vec3 pos;
	float vx, vy, vz;
	char buf[144], *b;

	if (!cmd_get_player_param(cmdtext, &parseidx, &target)) {
		amx_SetUString(buf144, CMD_SYN, 144);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 1;
	}
	if (target == INVALID_PLAYER_ID) {
		amx_SetUString(buf144, CMD_NOT_ONLINE, 144);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 1;
	}
	common_GetPlayerPos(playerid, &pos);
	zones_update(playerid, pos);

	b = buf;
	b += sprintf(buf, "%s(%d) is located in ",
		pdata[playerid]->name, playerid);
	if (lastzoneid[playerid] >= 0) {
		b += sprintf(b, "%s, ", zonenames[lastzoneid[playerid]]);
	}
	b += sprintf(b, "%s ", zonenames[lastregionid[playerid]]);
	vehicleid = NC_GetPlayerVehicleID(playerid);
	if (vehicleid) {
		model = NC_GetVehicleModel(vehicleid);
		NC_GetVehicleVelocity(vehicleid, buf32a, buf64a, buf144a);
		vx = *fbuf32;
		vy = *fbuf64;
		vz = *fbuf144;
		sprintf(b, "travelling at %.0f KPH in a %s (%.0f FT)",
			VEL_TO_KPH_VAL * sqrt(vx * vx + vy * vy + vz * vz),
			vehnames[model - 400],
			pos.z);
	} else {
		sprintf(b, "on foot (%.0f FT)", pos.z);
	}

	amx_SetUString(buf144, buf, 144);
	NC_SendClientMessage(playerid, COL_INFO_GENERIC, buf144a);
	return 1;
}
