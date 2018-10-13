
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "airport.h"
#include <string.h>
#include <math.h>

static struct navdata {
	struct airport *beacon;
	struct runway *vor;
	struct runway *ils;
	float dist;
	float alt;
	float crs;
} *nav[MAX_VEHICLES];

void nav_init()
{
	int i = 0;
	while (i < MAX_VEHICLES) {
		nav[i++] = NULL;
	}
}

/* native Nav_Reset(vehicleid) */
cell AMX_NATIVE_CALL Nav_Reset(AMX *amx, cell *params)
{
	int vid = params[1];
	if (nav[vid] != NULL) {
		free(nav[vid]);
		nav[vid] = NULL;
	}
	return 1;
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

/* native Nav_Update(vehicleid, Float:x, Float:y, Float:z, Float:heading) */
cell AMX_NATIVE_CALL Nav_Update(AMX *amx, cell *params)
{
	struct navdata *n = nav[params[1]];
	float x = amx_ctof(params[2]);
	float y = amx_ctof(params[3]);
	float z = amx_ctof(params[4]);
	float heading = amx_ctof(params[5]);
	float dx, dy;
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

	dx = pos->x - x;
	dy = pos->y - y;
	n->dist = sqrt(dx * dx + dy * dy);
	n->alt = z - pos->z;
	n->crs = atan2(dy, dx) * 180.0f / M_PI;

	return 1;
}
