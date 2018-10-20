
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
	struct runway *ils;
	int dist;
	int alt;
	int crs;
	int vorvalue;
} *nav[MAX_VEHICLES];

static struct playercache {
	int dist;
	int alt;
	int crs;
	int vorvalue;
} pcache[MAX_PLAYERS];

#define INVALID_CACHE 500000

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

/* native Nav_EnableADF(vehicleid, beacon[]) */
cell AMX_NATIVE_CALL Nav_EnableADF(AMX *amx, cell *params)
{
	int vid = params[1];
	cell *addr;
	char beacon[5], *bp = beacon, len = 0;
	struct airport *ap = airports;

	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(beacon, addr, 5);

	while (1) {
		if (*bp == 0) {
			break;
		}
		if (++len >= 5) {
			return 0;
		}
		*bp &= ~0x20;
		if (*bp < 'A' || 'Z' < *bp) {
			return 0;
		}
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
			nav[vid]->vor = nav[vid]->ils = NULL;
			return 1;
		}
		ap++;
	}

	return 0;
}

/* native Nav_EnableVOR(vehicleid, cmdtext[], buf64[]) */
cell AMX_NATIVE_CALL Nav_EnableVOR(AMX *amx, cell *params)
{
	int vid = params[1];
	cell *addr;
	char cmdtext[144], *cp = cmdtext;
	char beacon[5], *bp = beacon;
	char rwt[4], *rp = rwt;
	int len = 0;
	struct airport *ap = airports;
	struct runway *rw;

	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(cmdtext, addr, sizeof(cmdtext));
	amx_GetAddr(amx, params[3], &addr);

	*addr = (nav[vid] != NULL); /* syntax msg / turn off */
	while (1) {
		if (*cp < ' ') {
			if (nav[vid] != NULL) {
				free(nav[vid]);
				nav[vid] = NULL;
			}
			return 0;
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
	return 0;

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
			nav[vid]->ils = NULL;
			nav[vid]->vor = rw;
			return 1;
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
	return 0;
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
	return 0;
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
	float dist, crs;
	struct vec3 *pos;

	if (n == NULL) {
		return 0;
	}

	if (n->beacon != NULL) {
		pos = &n->beacon->pos;
	} else if (n->vor != NULL) {
		pos = &n->vor->pos;
	} else if (n->ils != NULL) {
		pos = &n->ils->pos;
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
	if (n->vor != NULL) {
		n->vorvalue = (int) (dist * cos(crs + M_PI2 - n->vor->headingr) / 50.0f * 85.0f);
		if (n->vorvalue > 85) {
			n->vorvalue = 85;
		} else if (n->vorvalue < -85) {
			n->vorvalue = -85;
		}
		n->vorvalue = 320 - n->vorvalue;
		crs = heading - n->vor->heading;
	} else {
		n->vorvalue = 1000;
		crs = heading - (crs * 180.0f / M_PI);
	}
	n->crs = (int) crs = crs - floor((crs + 180.0f) / 360.0f) * 360.0f;;

	return 1;
}

/* native Nav_Format(playerid, vehicleid, bufdist[], bufalt[], bufcrs[], &Float:vorvalue) */
cell AMX_NATIVE_CALL Nav_Format(AMX *amx, cell *params)
{
	int pid = params[1];
	struct navdata *n = nav[params[2]];
	cell *addr;
	float vorvalue;
	char dist[16], alt[16], crs[16];

	if (n == NULL) {
		return 0;
	}

	dist[0] = alt[0] = crs[0] = 0;

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

	amx_GetAddr(amx, params[3], &addr);
	amx_SetUString(addr, dist, 16);
	amx_GetAddr(amx, params[4], &addr);
	amx_SetUString(addr, alt, 16);
	amx_GetAddr(amx, params[5], &addr);
	amx_SetUString(addr, crs, 16);

	vorvalue = (float) n->vorvalue;
	amx_GetAddr(amx, params[6], &addr);
	*addr = amx_ftoc(vorvalue);

	if (n->vorvalue != pcache[pid].vorvalue) {
		pcache[pid].vorvalue = n->vorvalue;
		return 1;
	}

	return dist[0] != 0 || alt[0] != 0 || crs[0] != 0;
}
