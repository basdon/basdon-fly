
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
	int passenger_satisfaction;
	struct dbvehicle *veh;
	int vehicle_reincarnation_value;
	time_t starttime;
	short lastvehiclehp;
	short damagetaken;
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

int calculate_airport_tax(struct airport *ap, int missiontype)
{
	struct missionpoint *msp;
	struct runway *rnw;
	int runwayendcount = 0;
	int chargetype, costpercharge, tax = 500, chargernws = 0;

	switch (missiontype) {
	case 1: /* dodo, no rnw charge */
		chargetype = 1;
		costpercharge = 20;
		break;
	case 2: /* small passenger */
		chargetype = 2;
		costpercharge = 30;
		chargernws = 1;
		break;
	case 4: /* big passenger */
		chargetype = 4;
		costpercharge = 50;
		chargernws = 1;
		break;
	case 8:
	case 16:
	case 32: /* cargo */
		chargetype = 8 | 16 | 32;
		costpercharge = 40;
		chargernws = 1;
		break;
	case 64:
	case 128:
	case 256: /* heli, only charge amount of heliports */
		chargetype = 64 | 128 | 256;
		costpercharge = 30;
		break;
	case 512: /* military is govt, and no tax for special missions */
	case 1024:
	case 2048:
	case 4096:
		return 0;
	}

	msp = ap->missionpoints;
	while (msp != NULL) {
		if (msp->type & (64 | 128 | 256)) {
			tax += costpercharge;
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

	amx_GetAddr(amx, params[9], &bufaddr);
	startpoint->currentlyactivemissions++;
	endpoint->currentlyactivemissions++;
	sprintf(buf, "UPDATE msp SET o=o+1 WHERE i=%d", startpoint->id);
	amx_SetUString(bufaddr, buf, sizeof(buf));
	sprintf(buf, "UPDATE msp SET p=p+1 WHERE i=%d", endpoint->id);
	amx_SetUString(bufaddr + 200, buf, sizeof(buf));
	useridornull(playerid, tmpuseridornullbuf);
	dx = startpoint->x - endpoint->x;
	dy = startpoint->y - endpoint->y;
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
		sqrt(dx * dx + dy * dy));
	amx_SetUString(bufaddr + 400, buf, sizeof(buf));
	activemission[playerid] = mission = malloc(sizeof(struct mission));
	mission->id = -1;
	mission->stage = MISSION_STAGE_CREATE;
	mission->missiontype = missiontype;
	mission->startpoint = startpoint;
	mission->endpoint = endpoint;
	mission->passenger_satisfaction = 100;
	mission->veh = veh;
	mission->vehicle_reincarnation_value = vv;
	mission->starttime = time(NULL);
	mission->lastvehiclehp = amx_ftoc(params[7]);
	mission->damagetaken = 0;
	mission->weatherbonus = 0;
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

	sprintf(buf, "UPDATE flg SET state=%d WHERE id=%d", reason, mission->id);
	amx_GetAddr(amx, params[3], &addr);
	amx_SetUString(addr, buf, sizeof(buf));
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

/* native Missions_GetState(playerid) */
cell AMX_NATIVE_CALL Missions_GetState(AMX *amx, cell *params)
{
	struct mission *mission = activemission[params[1]];
	if (mission != NULL) {
		return mission->stage;
	}
	return -1;
}

/* native Missions_OnVehicleRepaired(playerid, vehicleid, Float:oldhp, Float:newhp) */
cell AMX_NATIVE_CALL Missions_OnVehicleRepaired(AMX *amx, cell *params)
{
	struct mission *miss;
	int i = MAX_PLAYERS;
	const int playerid = params[1], vehicleid = params[2];
	const float newhp = amx_ctof(params[4]), hpdiff = newhp - amx_ctof(params[3]);

	if ((miss = activemission[playerid]) != NULL &&
		miss->veh->spawnedvehicleid == vehicleid)
	{
		goto add_to_miss_data;
	}
	while (i--) {
		miss = activemission[i];
		if (miss != NULL && miss->veh->spawnedvehicleid == vehicleid) {
			goto add_to_miss_data;
		}
	}

	return 1;
add_to_miss_data:
	miss->damagetaken += hpdiff;
	miss->lastvehiclehp = newhp;
	logprintf("dmgtaken %d last %d", miss->damagetaken, miss->lastvehiclehp);
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

/* native Missions_PostUnload(playerid, Float:vehiclehp) */
cell AMX_NATIVE_CALL Missions_PostUnload(AMX *amx, cell *params)
{
	struct mission *mission;
	const int playerid = params[1];
	float vehiclehp = amx_ctof(params[2]);

	if ((mission = activemission[playerid]) == NULL) {
		return 0;
	}

	mission->damagetaken += mission->lastvehiclehp - vehiclehp;
	mission->lastvehiclehp = vehiclehp;

	/* TODO: stuff */

	free(mission);
	activemission[playerid] = NULL;
	return 1;
}

/* native Missions_Start(playerid, missionid, &Float:x, &Float:y, &Float:z, msg[]) */
cell AMX_NATIVE_CALL Missions_Start(AMX *amx, cell *params)
{
	struct mission *mission = activemission[params[1]];
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

	return 1;
}
