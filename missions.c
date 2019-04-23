
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include <math.h>
#include <string.h>
#include <time.h>
#include "airport.h"
#include "playerdata.h"
#include "vehicles.h"

const char *destinationtype_gate = "gate";
const char *destinationtype_cargo = "cargo";
const char *destinationtype_heliport = "heliport";

struct missionpoint {
	unsigned short id;
	float x, y, z;
	unsigned int type;
	unsigned char numberofsametype;
	unsigned short currentlyactivemissions;
	struct airport *ap;
	struct missionpoint *next;
};

struct mission {
	int id;
	int stage;
	int missiontype;
	struct missionpoint *startpoint, *endpoint;
	float distance, actualdistance;
	int passenger_satisfaction;
	struct dbvehicle *veh;
	int vehicle_reincarnation_value;
	time_t starttime;
	short lastvehiclehp, damagetaken;
	float lastfuel, fuelburned;
	short weatherbonus;
};

static struct mission *activemission[MAX_PLAYERS];

void missions_init()
{
	int i = MAX_PLAYERS;
	while (i--) {
		activemission[i] = NULL;
	}
}

void missions_add_distance(int playerid, float distance)
{
	if (activemission[playerid] != NULL) {
		activemission[playerid]->actualdistance += distance;
	}
}

/* returns amount of characters appended */
static int mission_append_pay(char *buf, char *description, int amount)
{
	char tmp[12], *ptmp;
	int len, p = sprintf(buf, "%s", description);
	if (amount < 0) {
		if (amount < -2147483647) {
			amount = -2147483647;
		}
		amount = -amount;
		buf[p++] = '{'; buf[p++] = 'F'; buf[p++] = '6'; buf[p++] = '0';
		buf[p++] = '0'; buf[p++] = '0'; buf[p++] = '0'; buf[p++] = '}';
		buf[p++] = '-';
	} else {
		if (amount > 2147483647) {
			amount = 2147483647;
		}
		buf[p++] = '{'; buf[p++] = '0'; buf[p++] = '0'; buf[p++] = 'F';
		buf[p++] = '6'; buf[p++] = '0'; buf[p++] = '0'; buf[p++] = '}';
	}
	buf[p++] = '$';
	sprintf(tmp, "%d", amount);
	ptmp = tmp;
	len = strlen(tmp);
	while (len-- > 0) {
		buf[p++] = *(ptmp++);
		if (len % 3 == 0 && len) {
			buf[p++] = ',';
		}
	}
	buf[p++] = '\n';
	return p;
}

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

/* in units/s */
static float mission_get_vehicle_maximum_speed(int model)
{
	/* max horizontal speed is about 81.6 */
	switch (model) {
	case MODEL_DODO: return 140.0f / 270.0f * 81.6f;
	case MODEL_BEAGLE: return 125.0f / 270.0f * 81.6f;
	case MODEL_NEVADA: return 185.0f / 270.0f * 81.6f;
	case MODEL_MAVERICK:
	case MODEL_VCNMAV: return 154.0f / 270.0f * 81.6f;
	case MODEL_RAINDANC: return 113.0f / 270.0f * 81.6f;
	case MODEL_LEVIATHN: return 108.0f / 270.0f * 81.6f;
	case MODEL_POLMAV: return 154.0f / 270.0f * 81.6f;
	case MODEL_SPARROW: return 106.0f / 270.0f * 81.6f;
	case MODEL_HUNTER: return 190.0f / 270.0f * 81.6f;
	case MODEL_CARGOBOB: return 126.0f / 270.0f * 81.6f;
	case MODEL_RUSTLER: return 235.0f / 270.0f * 81.6f;
	case MODEL_SKIMMER: return 135.0f / 270.0f * 81.6f;
	default:
		logprintf("mission_get_vehicle_minimum_missiontime: unknown model: %d", model);
	case MODEL_SHAMAL:
	case MODEL_HYDRA:
	case MODEL_ANDROM:
	case MODEL_AT400:
		return 81.6f;
	}
}

static float mission_get_vehicle_paymp(int model)
{
	const float heli_mp = 1.18f;

	switch (model) {
	case MODEL_DODO: return 270.0f / 140.0f;
	case MODEL_SHAMAL: return 1.0f;
	case MODEL_BEAGLE: return 270.0f / 125.0f;
	case MODEL_AT400: return 1.15f;
	case MODEL_ANDROM: return 1.1f;
	case MODEL_NEVADA: return 270.0f / 185.0f;
	case MODEL_MAVERICK:
	case MODEL_VCNMAV: return heli_mp * 270.0f / 154.0f;
	case MODEL_RAINDANC: return heli_mp * 270.0f / 113.0f;
	case MODEL_LEVIATHN: return heli_mp * 270.0f / 108.0f;
	case MODEL_POLMAV: return heli_mp * 270.0f / 154.0f;
	case MODEL_SPARROW: return heli_mp * 270.0f / 106.0f;
	case MODEL_HUNTER: return heli_mp * 270.0f / 190.0f;
	case MODEL_CARGOBOB: return heli_mp * 270.0f / 126.0f;
	case MODEL_HYDRA: return 0.9f;
	case MODEL_RUSTLER: return 270.0f / 235.0f;
	case MODEL_SKIMMER: return 270.0f / 135.0f;
	default:
		logprintf("mission_get_vehicle_paymp: unknown model: %d", model);
		return 1.0f;
	}
}

int calculate_airport_tax(struct airport *ap, int missiontype)
{
	struct missionpoint *msp;
	struct runway *rnw;
	int runwayendcount = 0;
	int costpermsp, tax = 500, chargernws = 0;

	switch (missiontype) {
	case 1: /* dodo, no rnw charge */
		costpermsp = 20;
		break;
	case 2: /* small passenger */
		costpermsp = 30;
		chargernws = 1;
		break;
	case 4: /* big passenger */
		costpermsp = 50;
		chargernws = 1;
		break;
	case 8:
	case 16:
	case 32: /* cargo */
		costpermsp = 40;
		chargernws = 1;
		break;
	case 64:
	case 128:
	case 256: /* heli, only charge amount of heliports */
		costpermsp = 30;
		break;
	case 512: /* military is govt, and no tax for special missions */
	case 1024:
	case 2048:
	case 4096:
	default:
		return 0;
	case 8192: /* skimmer, also for runways (lights maintenance etc) */
		costpermsp = 15;
		chargernws = 1;
		break;
	}

	msp = ap->missionpoints;
	while (msp != NULL) {
		if (msp->type & (64 | 128 | 256)) {
			tax += costpermsp;
		}
		msp = msp->next;
	}

	if (chargernws) {
		rnw = ap->runways;
		while (rnw != NULL) {
			runwayendcount++;
			if (rnw->nav & NAV_VOR) {
				tax += 15;
			}
			if (rnw->nav & NAV_ILS) {
				tax += 15;
			}
			rnw = rnw->next;
		}
		tax += 50 * runwayendcount;
	}
	return tax;
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
	if (mp != NULL) {
		newmp->next = mp;
	}
	ap->missionpoints = newmp;
	return 1;
}

struct airport *getRandomAirportForType(AMX *amx, int missiontype, struct airport *blacklistedairport)
{
	struct airport *ap = airports, **aps;
	int apsc = 0;
	int i = airportscount;

	aps = malloc(sizeof(aps) * airportscount);

	while (i--) {
		if (ap != blacklistedairport && ap->missiontypes & missiontype) {
			aps[apsc++] = ap;
		}
		ap++;
	}

	switch (apsc) {
	case 0: ap = NULL; break;
	case 1: ap = aps[0]; break;
	default: ap = aps[getrandom(amx, apsc)]; break;
	}

	free(aps);
	return ap;
}

struct missionpoint *getRandomEndPointForType(AMX *amx, int missiontype, struct airport *blacklistedairport)
{
#define TMP_PT_SIZE 7
	struct missionpoint *points[TMP_PT_SIZE], *msp;
	int pointc = 0, leastamtofcurrentmissions = 1000000;
	struct airport *airport = getRandomAirportForType(amx, missiontype, blacklistedairport);

	if (airport == NULL) {
		return NULL;
	}

	msp = airport->missionpoints;
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
		/* should not happen */
		return NULL;
	}

	if (pointc == 1) {
		return points[0];
	}

	return points[getrandom(amx, pointc)];
#undef TMP_PT_SIZE
}

/* native Missions_Create(playerid, Float:x, Float:y, Float:z, vehicleid, vv, vehiclehp, msg[], querybuf[]) */
cell AMX_NATIVE_CALL Missions_Create(AMX *amx, cell *params)
{
	cell *bufaddr;
	char buf[255], tmpuseridornullbuf[12];
	struct missionpoint *msp, *startpoint, *endpoint;
	struct airport *ap = airports, *closestap = NULL;
	struct dbvehicle *veh;
	struct mission *mission;
	int missiontype, i = airportscount;
	const int playerid = params[1], vehicleid = params[5], vv = params[6];
	float x, y, z, dx, dy, dz, dist, shortestdistance = 0x7F800000;

	amx_GetAddr(amx, params[8], &bufaddr);
	if (activemission[playerid] != NULL) {
		sprintf(buf,
		        WARN"You're already working! Use /s to stop your current work first ($%d fine).",
                        MISSION_CANCEL_FINE);
		amx_SetUString(bufaddr, buf, sizeof(buf));
		return 0;
	}

	if ((veh = gamevehicles[vehicleid].dbvehicle) == NULL) {
		goto unknownvehicle;
	}

	switch (veh->model) {
	case MODEL_DODO: missiontype = 1; break;
	case MODEL_SHAMAL:
	case MODEL_BEAGLE: missiontype = 2; break;
	case MODEL_AT400:
	case MODEL_ANDROM: missiontype = 4; break;
	case MODEL_NEVADA: missiontype = 16; break;
	case MODEL_MAVERICK:
	case MODEL_VCNMAV:
	case MODEL_RAINDANC:
	case MODEL_LEVIATHN:
	case MODEL_POLMAV:
	case MODEL_SPARROW: missiontype = 64; break;
	case MODEL_HUNTER:
	case MODEL_CARGOBOB: missiontype = 256; break;
	case MODEL_HYDRA:
	case MODEL_RUSTLER: missiontype = 512; break;
	case MODEL_SKIMMER: missiontype = 8192; break;
	default:
unknownvehicle:
		strcpy(buf, WARN"This vehicle can't complete any type of missions!");
		amx_SetUString(bufaddr, buf, sizeof(buf));
		return 0;
	}

	x = amx_ctof(params[2]);
	y = amx_ctof(params[3]);
	z = amx_ctof(params[4]);

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
		strcpy(buf, WARN"There are no missions available for this type of vehicle!");
		amx_SetUString(bufaddr, buf, sizeof(buf));
		return 0;
	}

	/* startpoint should be the point with 1) least amount of active missions 2) shortest distance */
	startpoint = NULL;
	i = 1000000;
thisisworsethanbubblesort:
	shortestdistance = 0x7F800000;
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
		strcpy(buf, WARN"Failed to find a starting point, please try again later.");
		amx_SetUString(bufaddr, buf, sizeof(buf));
		return 0;
	}

	endpoint = getRandomEndPointForType(amx, missiontype, closestap);
	if (endpoint == NULL) {
		strcpy(buf, WARN"There is no available destination for this type of plane at this time.");
		amx_SetUString(bufaddr, buf, sizeof(buf));
		return 0;
	}

	dx = startpoint->x - endpoint->x;
	dy = startpoint->y - endpoint->y;

	activemission[playerid] = mission = malloc(sizeof(struct mission));
	mission->id = -1;
	mission->stage = MISSION_STAGE_CREATE;
	mission->missiontype = missiontype;
	mission->startpoint = startpoint;
	mission->endpoint = endpoint;
	mission->distance = sqrt(dx * dx + dy * dy);
	mission->actualdistance = 0.0f;
	mission->passenger_satisfaction = 100;
	mission->veh = veh;
	mission->vehicle_reincarnation_value = vv;
	mission->starttime = time(NULL);
	mission->lastvehiclehp = amx_ctof(params[7]);
	mission->damagetaken = 0;
	mission->lastfuel = veh->fuel;
	mission->fuelburned = 0.0f;
	mission->weatherbonus = 0;

	amx_GetAddr(amx, params[9], &bufaddr);
	startpoint->currentlyactivemissions++;
	endpoint->currentlyactivemissions++;
	sprintf(buf, "UPDATE msp SET o=o+1 WHERE i=%d", startpoint->id);
	amx_SetUString(bufaddr, buf, sizeof(buf));
	sprintf(buf, "UPDATE msp SET p=p+1 WHERE i=%d", endpoint->id);
	amx_SetUString(bufaddr + 200, buf, sizeof(buf));
	useridornull(playerid, tmpuseridornullbuf);
	sprintf(buf,
	        "INSERT INTO flg(player,vehicle,missiontype,fapt,tapt,fmsp,tmsp,distance,tstart,tlastupdate) "
			"VALUES(%s,%d,%d,%d,%d,%d,%d,%.4f,UNIX_TIMESTAMP(),UNIX_TIMESTAMP())",
	        tmpuseridornullbuf,
		veh->id,
		missiontype,
		startpoint->ap->id,
		endpoint->ap->id,
		startpoint->id,
		endpoint->id,
		mission->distance);
	amx_SetUString(bufaddr + 400, buf, sizeof(buf));
	return 1;
}

/* native Missions_CreateTrackerMessage(playerid, vid, Float:hp,
                                        Float:vx, Float:vy, Float:vz, Float:alt, buf[]) */
cell AMX_NATIVE_CALL Missions_CreateTrackerMessage(AMX *amx, cell *params)
{
	const int playerid = params[1], vehicleid = params[2];
	cell *addr;
	char buf[50];
	struct mission *mission;
	float vx, vy, vz;
	short spd, alt, hp;

	if ((mission = activemission[playerid]) == NULL || mission->veh->spawnedvehicleid != vehicleid) {
		return 0;
	}

	vx = amx_ctof(params[4]);
	vy = amx_ctof(params[5]);
	vz = amx_ctof(params[6]);
	hp = (short) params[3];
	spd = (short) (VEL_TO_KTS_VAL * sqrt(vx * vx + vy * vy + vz * vz));
	alt = (short) amx_ctof(params[7]);

	/* flight tracker packet 2 */
	buf[0] = 'F';
	buf[1] = 'L';
	buf[2] = 'Y';
	buf[3] = 2;
	buf[4] = 0x40 | (*((char*)&mission->id) & 0xF);
	buf[5] = 0x40 | ((*((char*)&mission->id) >> 4) & 0xF);
	buf[6] = 0x40 | (*((char*)&mission->id + 1) & 0xF);
	buf[7] = 0x40 | ((*((char*)&mission->id + 1) >> 4) & 0xF);
	buf[8] = 0x40 | (*((char*)&mission->id + 2) & 0xF);
	buf[9] = 0x40 | ((*((char*)&mission->id + 2) >> 4) & 0xF);
	buf[10] = 0x40 | (*((char*)&mission->id + 3) & 0xF);
	buf[11] = 0x40 | ((*((char*)&mission->id + 3) >> 4) & 0xF);
	buf[12] = 0x40 | (*((char*)&spd) & 0xF);
	buf[13] = 0x40 | ((*((char*)&spd) >> 4) & 0xF);
	buf[14] = 0x40 | (*((char*)&spd + 1) & 0xF);
	buf[15] = 0x40 | ((*((char*)&spd + 1) >> 4) & 0xF);
	buf[16] = 0x40 | (*((char*)&alt) & 0xF);
	buf[17] = 0x40 | ((*((char*)&alt) >> 4) & 0xF);
	buf[18] = 0x40 | (*((char*)&alt + 1) & 0xF);
	buf[19] = 0x40 | ((*((char*)&alt + 1) >> 4) & 0xF);
	buf[20] = 0x40 | (*((char*)&mission->passenger_satisfaction) & 0xF);
	buf[21] = 0x40 | ((*((char*)&mission->passenger_satisfaction) >> 4) & 0xF);
	buf[22] = 0x40 | (*((char*)&hp) & 0xF);
	buf[23] = 0x40 | ((*((char*)&hp) >> 4) & 0xF);
	buf[24] = 0x40 | (*((char*)&hp + 1) & 0xF);
	buf[25] = 0x40 | ((*((char*)&hp + 1) >> 4) & 0xF);
	buf[26] = 0;
	amx_GetAddr(amx, params[8], &addr);
	*addr = 26;
	amx_SetUString(addr + 1, buf, sizeof(buf));
	return 1;
}

/* native Missions_EndUnfinished(playerid, reason, buf[]) */
cell AMX_NATIVE_CALL Missions_EndUnfinished(AMX *amx, cell *params)
{
	const int playerid = params[1], reason = params[2];
	struct mission *mission;
	cell *addr;
	char buf[144];

	if ((mission = activemission[playerid]) == NULL) {
		return 0;
	}

	sprintf(buf,
	        "UPDATE flg SET state=%d,tlastupdate=UNIX_TIMESTAMP(),adistance=%f WHERE id=%d",
	        reason,
		mission->actualdistance,
	        mission->id);
	amx_GetAddr(amx, params[3], &addr);
	amx_SetUString(addr, buf, sizeof(buf));

	/* flight tracker packet 3 */
	buf[0] = 'F';
	buf[1] = 'L';
	buf[2] = 'Y';
	buf[3] = 3;
	buf[4] = 0x40 | (*((char*)&mission->id) & 0xF);
	buf[5] = 0x40 | ((*((char*)&mission->id) >> 4) & 0xF);
	buf[6] = 0x40 | (*((char*)&mission->id + 1) & 0xF);
	buf[7] = 0x40 | ((*((char*)&mission->id + 1) >> 4) & 0xF);
	buf[8] = 0x40 | (*((char*)&mission->id + 2) & 0xF);
	buf[9] = 0x40 | ((*((char*)&mission->id + 2) >> 4) & 0xF);
	buf[10] = 0x40 | (*((char*)&mission->id + 3) & 0xF);
	buf[11] = 0x40 | ((*((char*)&mission->id + 3) >> 4) & 0xF);
	buf[12] = 0;
	*(addr + 500) = 12;
	amx_SetUString(addr + 501, buf, sizeof(buf));

	free(mission);
	activemission[playerid] = NULL;
	return 1;
}

/* native Missions_EnterCheckpoint(playerid, vehicleid, vv, x, y, z, errmsg[]) */
cell AMX_NATIVE_CALL Missions_EnterCheckpoint(AMX *amx, cell *params)
{
	const int playerid = params[1], vehicleid = params[2], vv = params[3];
	float x, y, z;
	struct mission *mission;
	struct dbvehicle *veh;
	cell *addr;
	char msg[144];

	if ((mission = activemission[playerid]) == NULL) {
		return 0;
	}

	if (vehicleid == 0 ||
		vv != mission->vehicle_reincarnation_value ||
		(veh = gamevehicles[vehicleid].dbvehicle) == NULL ||
		veh->id != mission->veh->id)
	{
		strcpy(msg, WARN"You must be in the starting vehicle to continue!");
		goto exit_set_errmsg;
	}

	x = amx_ctof(params[4]);
	y = amx_ctof(params[5]);
	z = amx_ctof(params[6]);

#if VEL_VER != 2
#error "recalc this"
#endif
	if (x * x + y * y + z * z > /*(35/VEL_TO_KTS_VAL)^2*/0.1307962) {
		strcpy(msg, WARN"You need to slow down to load/unload! Re-enter the checkpoint.");
		goto exit_set_errmsg;
	}

	switch (mission->stage) {
	case MISSION_STAGE_PRELOAD:
		mission->stage = MISSION_STAGE_LOAD;
		return MISSION_ENTERCHECKPOINTRES_LOAD;
	case MISSION_STAGE_FLIGHT:
		mission->stage = MISSION_STAGE_UNLOAD;
		return MISSION_ENTERCHECKPOINTRES_UNLOAD;
	default:
		logprintf("ERR: player entered mission checkpoint in invalid stage: %d",
		          mission->stage);
		return 0;
	}

exit_set_errmsg:
	amx_GetAddr(amx, params[7], &addr);
	amx_SetUString(addr, msg, sizeof(msg));
	return MISSION_ENTERCHECKPOINTRES_ERR;
}

/* native Missions_FinalizeAddPoints() */
cell AMX_NATIVE_CALL Missions_FinalizeAddPoints(AMX *amx, cell *params)
{
	struct airport *ap = airports;
	struct missionpoint *msp;
	int i = airportscount;
	unsigned char gate, cargo, heliport;

	while (i--) {
		gate = cargo = heliport = 1;
		msp = ap->missionpoints;
		while (msp != NULL) {
			if (msp->type & (1 | 2 | 4)) {
				msp->numberofsametype = gate++;
			} else if (msp->type & (8 | 16 | 32)) {
				msp->numberofsametype = cargo++;
			} else if (msp->type & (64 | 128 | 256)) {
				msp->numberofsametype = heliport++;
			} else {
				msp->numberofsametype = 1;
			}
			msp = msp->next;
		}
		ap++;
	}

	return 1;
}

/* native Missions_GetMissionNavData(playerid, &vehicleid, &vehiclemodel, &airportidx) */
cell AMX_NATIVE_CALL Missions_GetMissionNavData(AMX *amx, cell *params)
{
	const int playerid = params[1];
	cell *addr;
	struct mission *miss;
	if ((miss = activemission[playerid]) != NULL) {
		amx_GetAddr(amx, params[2], &addr);
		if ((*addr = miss->veh->spawnedvehicleid) == -1) {
			return 0;
		}
		amx_GetAddr(amx, params[3], &addr);
		*addr = miss->veh->model;
		amx_GetAddr(amx, params[4], &addr);
		if (miss->stage <= MISSION_STAGE_PRELOAD) {
			*addr = miss->startpoint->ap->id;
		} else {
			*addr = miss->endpoint->ap->id;
		}
		return 1;
	}
	return 0;
}

/* native Missions_GetState(playerid) */
cell AMX_NATIVE_CALL Missions_GetState(AMX *amx, cell *params)
{
	struct mission *mission = activemission[params[1]];
	if (mission != NULL) {
		return mission->stage;
	}
	return -1;
}

/* native Missions_OnVehicleRefueled(playerid, vehicleid, Float:refuelamount) */
cell AMX_NATIVE_CALL Missions_OnVehicleRefueled(AMX *amx, cell *params)
{
	struct mission *miss;
	const int playerid = params[1], vehicleid = params[2];
	const float refuelamount = amx_ctof(params[3]);

	if ((miss = activemission[playerid]) != NULL &&
		miss->veh->spawnedvehicleid == vehicleid)
	{
		miss->fuelburned += refuelamount;
		miss->lastfuel = miss->veh->fuel;
	}
	return 1;
}

/* native Missions_OnVehicleRepaired(playerid, vehicleid, Float:oldhp, Float:newhp) */
cell AMX_NATIVE_CALL Missions_OnVehicleRepaired(AMX *amx, cell *params)
{
	struct mission *miss;
	const int playerid = params[1], vehicleid = params[2];
	const float newhp = amx_ctof(params[4]), hpdiff = newhp - amx_ctof(params[3]);

	if ((miss = activemission[playerid]) != NULL &&
		miss->veh->spawnedvehicleid == vehicleid)
	{
		miss->damagetaken += hpdiff;
		miss->lastvehiclehp = newhp;
	}
	return 1;
}

/* native Missions_OnWeatherChanged(newweatherid) */
cell AMX_NATIVE_CALL Missions_OnWeatherChanged(AMX *amx, cell *params)
{
	int bonusvalue, i = MAX_PLAYERS;

	switch (params[1]) {
	case WEATHER_SF_RAINY:
	case WEATHER_CS_RAINY: bonusvalue = MISSION_WEATHERBONUS_RAINY; break;
	case WEATHER_SF_FOGGY:
	case WEATHER_UNDERWATER: bonusvalue = MISSION_WEATHERBONUS_FOGGY; break;
	case WEATHER_DE_SANDSTORMS: bonusvalue = MISSION_WEATHERBONUS_SANDSTORM; break;
	default: return 0;
	}
	bonusvalue += getrandom(amx, MISSION_WEATHERBONUS_DEVIATION);
	while (i--) {
		if (activemission[i] != NULL) {
			activemission[i]->weatherbonus += bonusvalue;
		}
	}
	return 1;
}

/* native Missions_PostLoad(playerid, &Float:x, &Float:y, &Float:z buf[]) */
cell AMX_NATIVE_CALL Missions_PostLoad(AMX *amx, cell *params)
{
	const int playerid = params[1];
	struct mission *mission;
	cell *addr;
	char buf[144];

	if ((mission = activemission[playerid]) == NULL ||
		mission->stage != MISSION_STAGE_LOAD)
	{
		return 0;
	}

	mission->stage = MISSION_STAGE_FLIGHT;
	sprintf(buf,
	        "UPDATE flg SET tload=UNIX_TIMESTAMP(),tlastupdate=UNIX_TIMESTAMP() WHERE id=%d",
	        mission->id);
	amx_GetAddr(amx, params[2], &addr);
	*addr = amx_ftoc(mission->endpoint->x);
	amx_GetAddr(amx, params[3], &addr);
	*addr = amx_ftoc(mission->endpoint->y);
	amx_GetAddr(amx, params[4], &addr);
	*addr = amx_ftoc(mission->endpoint->z);
	amx_GetAddr(amx, params[5], &addr);
	amx_SetUString(addr, buf, sizeof(buf));
	return 1;
}

/* native Missions_PostUnload(playerid, Float:vehiclehp, &pay, buf[]) */
cell AMX_NATIVE_CALL Missions_PostUnload(AMX *amx, cell *params)
{
	struct mission *mission;
	const int playerid = params[1];
	const float vehiclehp = amx_ctof(params[2]);
	float paymp;
	cell *addr;
	char buf[4096];
	int ptax, psatisfaction = 0, pdistance, pbonus = 0, ptotal, pdamage, pcheat = 0, tmp;
	int totaltime, mintime, duration_h, duration_m;
	int p;

	if ((mission = activemission[playerid]) == NULL || pdata[playerid] == NULL) {
		return 0;
	}

	amx_GetAddr(amx, params[4], &addr);

	tmp = mission->lastvehiclehp - vehiclehp;
	if (tmp < 0) {
		tmp = 0;
		pcheat -= 250000;
		sprintf(buf, "flg(#%d) vhh: was: %hd now: %.0f", mission->id, mission->lastvehiclehp, vehiclehp);
		amx_SetUString(addr + 2100, buf, sizeof(buf));
	} else {
		*(addr + 2100) = 0; /* ac log hp cheat */
	}
	mission->damagetaken += tmp;
	mission->lastvehiclehp = vehiclehp;
	mission->fuelburned += mission->lastfuel - mission->veh->fuel;

	totaltime = (int) difftime(time(NULL), mission->starttime);
	duration_m = totaltime % 60;
	duration_h = (totaltime - duration_m) / 60;

	/* don't use adistance because it also includes z changes */
	mintime = (int) (mission->distance / mission_get_vehicle_maximum_speed(mission->veh->model));
	if (totaltime < mintime) {
		pcheat -= 250000;
		sprintf(buf, "flg(#%d) too fast: min: %d actual: %d", mission->id, mintime, totaltime);
		amx_SetUString(addr + 2000, buf, sizeof(buf));
	} else {
		*(addr + 2000) = 0; /* ac log speed cheat */
	}

	paymp = mission_get_vehicle_paymp(mission->veh->model);
	ptax = -calculate_airport_tax(mission->endpoint->ap, mission->missiontype);
	pdistance = 500 + (int) (mission->distance * 1.135f);
	pdistance = (int) (pdistance * paymp);
	if (mission->missiontype & PASSENGER_MISSIONTYPES) {
		if (mission->passenger_satisfaction == 100) {
			psatisfaction = 500;
		} else {
			psatisfaction = (mission->passenger_satisfaction - 100) * 30;
		}
	}
	pdamage = -3 * mission->damagetaken;
	ptotal = mission->weatherbonus + psatisfaction + pdistance + pbonus + ptax + pdamage + pcheat;

	sprintf(buf,
		"%s completed flight #%d from %s (%s) to %s (%s) in %dh%02dm",
		pdata[playerid]->name,
	        mission->id,
		mission->startpoint->ap->name,
	        mission->startpoint->ap->beacon,
	        mission->endpoint->ap->name,
	        mission->endpoint->ap->beacon,
	        duration_h,
	        duration_m);
	amx_SetUString(addr, buf, sizeof(buf));
	sprintf(buf,
	        "UPDATE flg SET tunload=UNIX_TIMESTAMP(),tlastupdate=UNIX_TIMESTAMP(),"
	        "state=%d,fuel=%f,ptax=%d,pweatherbonus=%d,psatisfaction=%d,"
	        "pdistance=%d,pdamage=%d,pcheat=%d,pbonus=%d,ptotal=%d,satisfaction=%d,adistance=%f,"
	        "paymp=%f WHERE id=%d",
	        MISSION_STATE_FINISHED,
	        mission->fuelburned,
	        ptax,
	        mission->weatherbonus,
	        psatisfaction,
	        pdistance,
	        pdamage,
		pcheat,
	        pbonus,
	        ptotal,
	        mission->passenger_satisfaction,
	        mission->actualdistance,
	        paymp,
	        mission->id);
	amx_SetUString(addr + 200, buf, sizeof(buf));
	p = 0;
	p += sprintf(buf,
	             "{ffffff}Flight:\t\t\t"ECOL_MISSION"#%d\n"
	             "{ffffff}Origin:\t\t\t"ECOL_MISSION"%s\n"
	             "{ffffff}Destination:\t\t"ECOL_MISSION"%s\n"
	             "{ffffff}Distance (pt to pt):\t"ECOL_MISSION"%.0fm\n"
	             "{ffffff}Distance (actual):\t"ECOL_MISSION"%.0fm\n"
	             "{ffffff}Duration:\t\t"ECOL_MISSION"%dh%02dm\n"
	             "{ffffff}Fuel Burned:\t\t"ECOL_MISSION"%.1fL\n"
	             "{ffffff}Vehicle pay category:\t"ECOL_MISSION"%.1fx\n",
	             mission->id,
	             mission->startpoint->ap->name,
	             mission->endpoint->ap->name,
	             mission->distance,
	             mission->actualdistance,
	             duration_h,
	             duration_m,
		     mission->fuelburned,
	             paymp);
	if (mission->missiontype & PASSENGER_MISSIONTYPES) {
		p += sprintf(buf + p,
		             "{ffffff}Passenger Satisfaction:\t"ECOL_MISSION"%d%%\n",
		             mission->passenger_satisfaction);
	}
	buf[p++] = '\n';
	buf[p++] = '\n';
	if (ptax) {
		p += mission_append_pay(buf + p, "{ffffff}Airport Tax:\t\t", ptax);
	}
	if (mission->weatherbonus) {
		p += mission_append_pay(buf + p, "{ffffff}Weather bonus:\t\t", mission->weatherbonus);
	}
	p += mission_append_pay(buf + p, "{ffffff}Distance Pay:\t\t", pdistance);
	if (mission->missiontype & PASSENGER_MISSIONTYPES) {
		if (psatisfaction > 0) {
			p += mission_append_pay(buf + p, "{ffffff}Satisfaction Bonus:\t", psatisfaction);
		} else {
			p += mission_append_pay(buf + p, "{ffffff}Satisfaction Penalty:\t", psatisfaction);
		}
	}
	if (pdamage) {
		p += mission_append_pay(buf + p, "{ffffff}Damage Penalty:\t", pdamage);
	}
	if (pcheat) {
		p += mission_append_pay(buf + p, "{ffffff}Cheat Penalty:\t\t", pcheat);
	}
	if (pbonus) {
		p += mission_append_pay(buf + p, "{ffffff}Bonus:\t\t\t", pbonus);
	}
	p += mission_append_pay(buf + p, "\n\n\t{ffffff}Total Pay: ", ptotal);
	buf[--p] = 0;
	amx_SetUString(addr + 1000, buf, sizeof(buf));

	/* flight tracker packet 3 */
	buf[0] = 'F';
	buf[1] = 'L';
	buf[2] = 'Y';
	buf[3] = 3;
	buf[4] = 0x40 | (*((char*)&mission->id) & 0xF);
	buf[5] = 0x40 | ((*((char*)&mission->id) >> 4) & 0xF);
	buf[6] = 0x40 | (*((char*)&mission->id + 1) & 0xF);
	buf[7] = 0x40 | ((*((char*)&mission->id + 1) >> 4) & 0xF);
	buf[8] = 0x40 | (*((char*)&mission->id + 2) & 0xF);
	buf[9] = 0x40 | ((*((char*)&mission->id + 2) >> 4) & 0xF);
	buf[10] = 0x40 | (*((char*)&mission->id + 3) & 0xF);
	buf[11] = 0x40 | ((*((char*)&mission->id + 3) >> 4) & 0xF);
	buf[12] = 0;
	*(addr + 2200) = 12;
	amx_SetUString(addr + 2201, buf, sizeof(buf));

	amx_GetAddr(amx, params[3], &addr);
	*addr = ptotal;

	free(mission);
	activemission[playerid] = NULL;
	return 1;
}

/* native Missions_ShouldShowSatisfaction(playerid) */
cell AMX_NATIVE_CALL Missions_ShouldShowSatisfaction(AMX *amx, cell *params)
{
	const int playerid = params[1];
	return activemission[playerid] != NULL &&
		activemission[playerid]->missiontype & PASSENGER_MISSIONTYPES;
}

/* native Missions_Start(playerid, missionid, &Float:x, &Float:y, &Float:z, buf[]) */
cell AMX_NATIVE_CALL Missions_Start(AMX *amx, cell *params)
{
	const int playerid = params[1];
	struct mission *mission = activemission[playerid];
	cell *addr;
	char msg[144];
	const char *msptypename = NULL;

	if (mission == NULL) {
		return 0;
	}

	mission->id = params[2];
	mission->stage = MISSION_STAGE_PRELOAD;
	amx_GetAddr(amx, params[3], &addr);
	*addr = amx_ftoc(mission->startpoint->x);
	amx_GetAddr(amx, params[4], &addr);
	*addr = amx_ftoc(mission->startpoint->y);
	amx_GetAddr(amx, params[5], &addr);
	*addr = amx_ftoc(mission->startpoint->z);

	switch (mission->missiontype) {
	case 1: msptypename = destinationtype_gate; break;
	case 2: msptypename = destinationtype_gate; break;
	case 4: msptypename = destinationtype_gate; break;
	case 16: msptypename = destinationtype_cargo; break;
	case 64: msptypename = destinationtype_heliport; break;
	case 256: msptypename = destinationtype_heliport; break;
	}

	if (msptypename == NULL) {
		sprintf(msg,
		        "Flight from %s (%s) to %s (%s)",
		        mission->startpoint->ap->name,
		        mission->startpoint->ap->beacon,
		        mission->endpoint->ap->name,
		        mission->endpoint->ap->beacon);
	} else {
		sprintf(msg,
			"Flight from %s (%s) %s %d to %s (%s) %s %d",
			mission->startpoint->ap->name,
			mission->startpoint->ap->beacon,
			msptypename,
			mission->startpoint->numberofsametype,
			mission->endpoint->ap->name,
			mission->endpoint->ap->beacon,
			msptypename,
			mission->endpoint->numberofsametype);
	}
	amx_GetAddr(amx, params[6], &addr);
	amx_SetUString(addr, msg, sizeof(msg));

	/* flight tracker packet 1 */
	msg[0] = 'F';
	msg[1] = 'L';
	msg[2] = 'Y';
	msg[3] = 1;
	msg[4] = 0x40 | (*((char*)&mission->id) & 0xF);
	msg[5] = 0x40 | ((*((char*)&mission->id) >> 4) & 0xF);
	msg[6] = 0x40 | (*((char*)&mission->id + 1) & 0xF);
	msg[7] = 0x40 | ((*((char*)&mission->id + 1) >> 4) & 0xF);
	msg[8] = 0x40 | (*((char*)&mission->id + 2) & 0xF);
	msg[9] = 0x40 | ((*((char*)&mission->id + 2) >> 4) & 0xF);
	msg[10] = 0x40 | (*((char*)&mission->id + 3) & 0xF);
	msg[11] = 0x40 | ((*((char*)&mission->id + 3) >> 4) & 0xF);
	if (pdata[playerid] != NULL) {
		msg[12] = pdata[playerid]->namelen;
		strcpy(msg + 13, pdata[playerid]->name);
		*(addr + 200) = 13 + pdata[playerid]->namelen;
	} else {
		msg[12] = 1;
		msg[13] = '?';
		msg[14] = 0;
		*(addr + 200) = 14;
	}
	amx_SetUString(addr + 201, msg, sizeof(msg));

	return 1;
}

/* native Missions_UpdateSatisfaction(playerid, vehicleid, Float:qw, Float:qx, Float:qy, Float:qz, buf[]) */
cell AMX_NATIVE_CALL Missions_UpdateSatisfaction(AMX *amx, cell *params)
{
	struct mission *miss;
	const int playerid = params[1], vehicleid = params[2];
	cell *addr;
	int lastval;
	float qw, qx, qy, qz;
	float value = 0.0f;
	char buf[144];

	if ((miss = activemission[playerid]) != NULL &&
		miss->stage == MISSION_STAGE_FLIGHT &&
		miss->veh->spawnedvehicleid == vehicleid)
	{
		lastval = miss->passenger_satisfaction;
		qw = amx_ctof(params[3]);
		qx = amx_ctof(params[4]);
		qy = amx_ctof(params[5]);
		qz = amx_ctof(params[6]);
		value = fabs(100.0f * 2.0f * (qy * qz - qw * qx)) - 45.0;
		if (value > 0.0) {
			miss->passenger_satisfaction -= (int) (value / 2.0f);
			if (miss->passenger_satisfaction < 0) {
				miss->passenger_satisfaction = 0;
			}
		}
		value = fabs(100.0f * 2.0f * (qx * qz + qw * qy)) - 60.0f;
		if (value > 0.0) {
			miss->passenger_satisfaction -= (int) (value / 2.0f);
			if (miss->passenger_satisfaction < 0) {
				miss->passenger_satisfaction = 0;
			}
		}
		if (lastval != miss->passenger_satisfaction) {
			sprintf(buf, "Passenger~n~Satisfaction: %d%%", miss->passenger_satisfaction);
			amx_GetAddr(amx, params[7], &addr);
			amx_SetUString(addr, buf, sizeof(buf));
			return 1;
		}
	}
	return 0;
}
