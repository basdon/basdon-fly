
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include "airport.h"
#include <string.h>
#include <math.h>

static struct navdata {
	struct airport *beacon;
	struct runway *vor;
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

/* native Nav_EnableADF(vehicleid, cmdtext[], buf64[]) */
cell AMX_NATIVE_CALL Nav_EnableADF(AMX *amx, cell *params)
{
	int vid = params[1];
	cell *addr;
	char cmdtext[144], *cp = cmdtext;
	char beacon[5], *bp = beacon, len = 0;
	struct airport *ap = airports;

	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(cmdtext, addr, sizeof(cmdtext));
	amx_GetAddr(amx, params[3], &addr);

	while (1) {
		if (*cp < ' ') {
			if (nav[vid] != NULL) {
				free(nav[vid]);
				nav[vid] = NULL;
				return RESULT_ADF_OFF;
			}
			amx_SetUString(addr, WARN"Syntax: /adf [beacon] - see /beacons or /nearest", 64);
			return RESULT_ADF_ERR;
		}
		if (*cp != ' ') {
			break;
		}
		cp++;
	}

	while (1) {
		if (*cp == 0) {
			*bp = 0;
			break;
		}
		if (++len >= 5) {
			goto unkbeacon;
		}
		*bp = *cp & ~0x20;
		if (*bp < 'A' || 'Z' < *bp) {
			goto unkbeacon;
		}
		cp++;
		bp++;
	}

	len = airportscount;
	while (len--) {
		if (strcmp(beacon, ap->beacon) == 0) {
			if (nav[vid] == NULL) {
				nav[vid] = malloc(sizeof(struct navdata));
				nav[vid]->alt = nav[vid]->crs = nav[vid]->dist = 0.0f;
			}
			nav[vid]->beacon = ap;
			nav[vid]->vor = NULL;
			nav[vid]->ils = 0;
			return RESULT_ADF_ON;
		}
		ap++;
	}

unkbeacon:
	amx_SetUString(addr, WARN"Unknown beacon - see /beacons or /nearest", 64);
	return RESULT_ADF_ERR;
}

/* native Nav_EnableVOR(vehicleid, cmdtext[], buf64[]) */
cell AMX_NATIVE_CALL Nav_EnableVOR(AMX *amx, cell *params)
{
	int vid = params[1];
	cell *addr;
	char cmdtext[144], *cp = cmdtext;
	char beacon[5], *bp = beacon;
	int len = 0;
	struct airport *ap = airports;
	struct runway *rw;

	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(cmdtext, addr, sizeof(cmdtext));
	amx_GetAddr(amx, params[3], &addr);

	while (1) {
		if (*cp < ' ') {
			if (nav[vid] != NULL) {
				free(nav[vid]);
				nav[vid] = NULL;
				return RESULT_VOR_OFF;
			}
			amx_SetUString(addr, WARN"Syntax: /vor [beacon][runway] - see /nearest or /beacons", 64);
			return RESULT_VOR_ERR;
		}
		if (*cp != ' ') {
			break;
		}
		cp++;
	}

	/* beacon */
	while (1) {
		if (*cp == 0 || *cp == ' ' || ('0' <= *cp && *cp <= '9')) {
			*bp = 0;
			break;
		}
		if (++len >= 5) {
			goto unkbeacon;
		}
		*bp = *cp & ~0x20;
		if (*bp < 'A' || 'Z' < *bp) {
			goto unkbeacon;
		}
		bp++;
		cp++;
	}

	if (len) {
		len = airportscount;
		while (len--) {
			if (strcmp(beacon, ap->beacon) == 0) {
				goto havebeacon;
			}
			ap++;
		}
	}

unkbeacon:
	amx_SetUString(addr, WARN"Unknown beacon", 64);
	return RESULT_VOR_ERR;

havebeacon:
	while (1) {
		if (*cp < ' ') {
			goto tellrws;
		}
		if (*cp != ' ') {
			break;
		}
		cp++;
	}

	/* rw */
	if (*cp < '0' || '9' < *cp) {
		goto tellrws;
	}
	beacon[0] = *cp++;
	if (*cp < '0' || '9' < *cp) {
		goto tellrws;
	}
	beacon[1] = *cp++;
	if (*cp == 0) {
		beacon[2] = 0;
	} else {
		beacon[2] = *cp++ & ~0x20;
		if (*cp != 0 || (beacon[2] != 'L' && beacon[2] != 'R')) {
			goto tellrws;
		}
		beacon[3] = 0;
	}

	rw = ap->runways;
	while (rw != NULL) {
		if (strcmp(rw->id, beacon) == 0) {
			if (nav[vid] == NULL) {
				nav[vid] = malloc(sizeof(struct navdata));
				nav[vid]->alt = nav[vid]->crs = nav[vid]->dist = 0.0f;
			}
			nav[vid]->beacon = NULL;
			nav[vid]->ils = 0;
			nav[vid]->vor = rw;
			return RESULT_VOR_ON;
		}
		rw = rw->next;
	}

tellrws:
	rw = ap->runways;
	while (rw != NULL) {
		if (rw->nav) {
			goto hasnav;
		}
		rw = rw->next;
	}
	amx_SetUString(addr, WARN"There are no VOR capable runways at this beacon", 64);
	return RESULT_VOR_ERR;
hasnav:
	len = sprintf(cmdtext, WARN"Unknown runway, try one of:");
	rw = ap->runways;
	while (rw != NULL) {
		if (rw ->nav) {
			len += sprintf(cmdtext + len, " %s", rw->id);
		}
		rw = rw->next;
	}
	amx_SetUString(addr, cmdtext, sizeof(cmdtext));
	return RESULT_VOR_ERR;
}

/* native Nav_Format(playerid, vehicleid, bufdist[], bufalt[], bufcrs[], bufils[], &Float:vorvalue) */
cell AMX_NATIVE_CALL Nav_Format(AMX *amx, cell *params)
{
	int pid = params[1];
	struct navdata *n = nav[params[2]];
	cell *addr;
	float vorvalue;
	char dist[16], alt[16], crs[16], ils[144];

	if (n == NULL) {
		return 0;
	}

	dist[0] = alt[0] = crs[0] = ils[0] = 0;

	if (n->dist != pcache[pid].dist) {
		if (n->dist >= 1000) {
			sprintf(dist, "%.1fK", n->dist / 1000.0f);
		} else {
			sprintf(dist, "%d", n->dist);
		}
		pcache[pid].dist = n->dist;
	}
	if (n->alt != pcache[pid].alt) {
		sprintf(alt, "%d", n->alt);
		pcache[pid].alt = n->alt;
	}
	if (n->crs != pcache[pid].crs) {
		sprintf(crs, "%d", n->crs);
		pcache[pid].crs = n->crs;
	}

	if (n->ilsx < 0 || 8 < n->ilsx) {
		strcpy(ils, "~r~~h~no ILS signal");
	} else if (0 <= n->ilsz &&  n->ilsz <= 8) {
		strcpy(ils, "~w~X~n~~w~X~n~~w~X~n~~w~X~n~~w~X ~w~X ~w~X ~w~X ~w~X"
			" ~w~X ~w~X ~w~X ~w~X~n~~w~X~n~~w~X~n~~w~X~n~~w~X");
		ils[29 + 5 * n->ilsx] = ils[(unsigned char) "\x01\x08\x0f\x16\x31\x4c\x53\x5a\x61"[n->ilsz]] = 'r';
	}

	amx_GetAddr(amx, params[3], &addr);
	amx_SetUString(addr, dist, sizeof(dist));
	amx_GetAddr(amx, params[4], &addr);
	amx_SetUString(addr, alt, sizeof(alt));
	amx_GetAddr(amx, params[5], &addr);
	amx_SetUString(addr, crs, sizeof(crs));
	amx_GetAddr(amx, params[6], &addr);
	amx_SetUString(addr, ils, sizeof(ils));

	vorvalue = (float) n->vorvalue;
	amx_GetAddr(amx, params[7], &addr);
	*addr = amx_ftoc(vorvalue);

	if (n->vorvalue != pcache[pid].vorvalue) {
		pcache[pid].vorvalue = n->vorvalue;
		return 1;
	}

	return dist[0] != 0 || alt[0] != 0 || crs[0] != 0 || ils[0] != 0;
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

/* native Nav_Reset(vehicleid) */
cell AMX_NATIVE_CALL Nav_Reset(AMX *amx, cell *params)
{
	int vid = params[1];
	if (nav[vid] != NULL) {
		free(nav[vid]);
		nav[vid] = NULL;
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

/* native Nav_ToggleILS(vehicleid) */
cell AMX_NATIVE_CALL Nav_ToggleILS(AMX *amx, cell *params)
{
	int vid = params[1];
	if (nav[vid] == NULL || nav[vid]->vor == NULL) {
		return RESULT_ILS_NOVOR;
	}
	if ((nav[vid]->vor->nav & NAV_ILS) != NAV_ILS) {
		return RESULT_ILS_NOILS;
	}
	nav[vid]->ilsx = nav[vid]->ilsz = INVALID_ILS_VALUE;
	return (nav[vid]->ils ^= 1);
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

/* native Nav_Update(vehicleid, Float:x, Float:y, Float:z, Float:heading) */
cell AMX_NATIVE_CALL Nav_Update(AMX *amx, cell *params)
{
	struct navdata *n = nav[params[1]];
	float x = amx_ctof(params[2]);
	float y = amx_ctof(params[3]);
	float z = amx_ctof(params[4]);
	float heading = 360.0f - amx_ctof(params[5]);
	float dx, dy;
	float dist, crs, vorangle, horizontaldeviation;
	struct vec3 *pos;

	if (n == NULL) {
		return 0;
	}

	if (n->beacon != NULL) {
		pos = &n->beacon->pos;
	} else if (n->vor != NULL) {
		pos = &n->vor->pos;
	} else {
		return 0;
	}

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

	return 1;
}
