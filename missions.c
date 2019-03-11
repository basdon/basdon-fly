
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include "airport.h"

struct missionpoint {
	unsigned int id;
	float x, y, z;
	unsigned int type;
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
	newmp->type = params[6];
	newmp->next = NULL;

	if (ap->missionpoints == NULL) {
		ap->missionpoints = newmp;
		return 1;
	}

	mp = ap->missionpoints;
	while (mp->next != NULL) {
		mp = mp->next;
	}
	mp->next = newmp;
	return 1;
}
