
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include "airport.h"
#include <string.h>

const char *destinationtype_gate = " gate";
const char *destinationtype_cargo = " cargo";
const char *destinationtype_heliport = " heliport";
const char *destinationtype_none = "";

struct missionpoint {
	unsigned short id;
	float x, y, z;
	unsigned int type;
	unsigned short currentlyactivemissions;
	struct airport *ap;
	struct missionpoint *next;
};

void missions_freepoints()
{
	int i = airportscount;
	struct airport *ap = airports;
	struct missionpoint *msp, *tmp;

	while (i--) {
		msp = ap->missionpoints;
		while (msp != NULL) {
			tmp = msp->next;
			free(msp);
			msp = tmp;
		}
		ap++;
	}
}

/* native Missions_AddPoint(aptindex, id, Float:x, Float:y, Float:z, type) */
cell AMX_NATIVE_CALL Missions_AddPoint(AMX *amx, cell *params)
{
	struct missionpoint *mp;
	struct missionpoint *newmp;
	struct airport *ap = airports + params[1];

	newmp = malloc(sizeof(struct missionpoint));
	newmp->id = params[2];
	newmp->x = amx_ctof(params[3]);
	newmp->y = amx_ctof(params[4]);
	newmp->z = amx_ctof(params[5]);
	ap->missiontypes |= newmp->type = params[6];
	newmp->currentlyactivemissions = 0;
	newmp->ap = ap;
	newmp->next = NULL;

	mp = ap->missionpoints;
	if (mp == NULL) {
		ap->missionpoints = newmp;
		return 1;
	}

	while (mp->next != NULL) {
		mp = mp->next;
	}
	mp->next = newmp;
	return 1;
}

struct missionpoint *getRandomEndPointForType(int missiontype, struct airport *blacklistedairport)
{
#define TMP_PT_SIZE 5
	struct missionpoint *points[TMP_PT_SIZE], *msp;
	int pointc = 0, leastamtofcurrentmissions = 1000000;
	int randomap, applicableap;

	randomap = applicableap = 4; /* TODO actual random one */
	while ((airports + applicableap) == blacklistedairport ||
		!((airports + applicableap)->missiontypes & missiontype))
	{
		applicableap++;
		if (applicableap == airportscount) {
			applicableap = 0;
		}
		if (randomap == applicableap) {
			return NULL;
		}
	}

	msp = (airports + applicableap)->missionpoints;
	while (msp != NULL) {
		if (msp->type & missiontype) {
			if (msp->currentlyactivemissions < leastamtofcurrentmissions) {
				leastamtofcurrentmissions = msp->currentlyactivemissions;
				pointc = 0;
			}
			if (pointc < TMP_PT_SIZE) {
				points[pointc++] = msp;
			}
		}
		msp = msp->next;
	}

	if (pointc == 0) {
		return NULL;
	}

	if (pointc == 1) {
		return points[0];
	}

	return points[1]; /* TODO actual random	*/
#undef TMP_PT_SIZE
}

/* native Missions_Start(Float:x, Float:y, Float:z, vehiclemodel, msg[]) */
cell AMX_NATIVE_CALL Missions_Start(AMX *amx, cell *params)
{
	cell *xaddr, *yaddr, *zaddr, *msgaddr;
	char msg[144];
	struct missionpoint *msp, *startpoint, *endpoint;
	struct airport *ap = airports, *closestap = NULL;
	int missiontype, i = airportscount;
	const char *msptypename = destinationtype_none;
	float x, y, z, dx, dy, dz, dist, shortestdistance = 0x7F800000;

	amx_GetAddr(amx, params[5], &msgaddr);
	switch (params[4]) {
	case MODEL_DODO: missiontype = 1; msptypename = destinationtype_gate; break;
	case MODEL_SHAMAL:
	case MODEL_BEAGLE: missiontype = 2; msptypename = destinationtype_gate; break;
	case MODEL_AT400:
	case MODEL_ANDROM: missiontype = 4; msptypename = destinationtype_gate; break;
	case MODEL_NEVADA: missiontype = 16; msptypename = destinationtype_cargo; break;
	case MODEL_MAVERICK:
	case MODEL_VCNMAV:
	case MODEL_RAINDANC:
	case MODEL_LEVIATHN:
	case MODEL_POLMAV:
	case MODEL_SPARROW: missiontype = 64; msptypename = destinationtype_heliport; break;
	case MODEL_HUNTER:
	case MODEL_CARGOBOB: missiontype = 256; msptypename = destinationtype_heliport; break;
	case MODEL_HYDRA:
	case MODEL_RUSTLER: missiontype = 512; break;
	default:
		strcpy(msg, "This vehicle can't complete any type of missions!");
		amx_SetUString(msgaddr, msg, sizeof(msg));
		return 0;
	}

	amx_GetAddr(amx, params[1], &xaddr);
	amx_GetAddr(amx, params[2], &yaddr);
	amx_GetAddr(amx, params[3], &zaddr);
	x = amx_ctof(*xaddr);
	y = amx_ctof(*yaddr);
	z = amx_ctof(*zaddr);

	while (i--) {
		if (ap->missiontypes & missiontype) {
			dx = ap->pos.x - x;
			dy = ap->pos.y - y;
			dz = ap->pos.z - z;
			if ((dist = dx * dx + dy * dy + dz * dz) < shortestdistance) {
				shortestdistance = dist;
				closestap = ap;
			}
		}
		ap++;
	}

	if (closestap == NULL) {
		strcpy(msg, "There are no missions available for this type of vehicle!");
		amx_SetUString(msgaddr, msg, sizeof(msg));
		return 0;
	}

	/* startpoint should be the point with 1) least amount of active missions 2) shortest distance */
	startpoint = NULL;
	i = 1000000;
thisisworsethanbubblesort:
	shortestdistance = 0x78F00000;
	msp = closestap->missionpoints;
	while (msp != NULL) {
		if (msp->type & missiontype && msp->currentlyactivemissions <= i) {
			dx = msp->x - x;
			dy = msp->y - y;
			dz = msp->z - z;
			if ((dist = dx * dx + dy * dy + dz * dz) < shortestdistance) {
				if (msp->currentlyactivemissions < i) {
					i = msp->currentlyactivemissions;
					goto thisisworsethanbubblesort;
				}
				shortestdistance = dist;
				startpoint = msp;
			}
		}
		msp = msp->next;
	}

	if (startpoint == NULL) {
		/* this should not be happening	*/
		logprintf("ERR: could not find suitable mission startpoint");
		strcpy(msg, "Failed to find a starting point, please try again later.");
		amx_SetUString(msgaddr, msg, sizeof(msg));
		return 0;
	}

	*xaddr = amx_ftoc(startpoint->x);
	*yaddr = amx_ftoc(startpoint->y);
	*zaddr = amx_ftoc(startpoint->z);

	endpoint = getRandomEndPointForType(missiontype, closestap);
	if (endpoint == NULL) {
		strcpy(msg, "There is no available destination for this type of plane at this time.");
		amx_SetUString(msgaddr, msg, sizeof(msg));
	}

	sprintf(msg,
	        "Flight from %s%s to %s%s",
	        closestap->name,
		msptypename,
	        endpoint->ap->name,
		msptypename);
	amx_SetUString(msgaddr, msg, sizeof(msg));

	/* TODO store stuff (missiondata, queries) */
	return 1;
}
