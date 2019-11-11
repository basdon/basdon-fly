
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "airport.h"
#include "anticheat.h"
#include "cmd.h"
#include "playerdata.h"
#include "math.h"
#include "missions.h"
#include "money.h"
#include "nav.h"
#include "vehicles.h"
#include <string.h>
#include <time.h>

#define TRACKER_PORT 7766

const static char *SATISFACTION_TEXT_FORMAT = "Passenger~n~Satisfaction: %d%%";

struct MISSIONPOINT {
	unsigned short id;
	float x, y, z;
	unsigned int type;
	char name[MAX_MSP_NAME + 1];
	unsigned short currentlyactivemissions;
	struct AIRPORT *ap;
	struct MISSIONPOINT *next;
};

struct MISSION {
	int id;
	int stage;
	int missiontype;
	struct MISSIONPOINT *startpoint, *endpoint;
	float distance, actualdistanceM;
	int passenger_satisfaction;
	struct dbvehicle *veh;
	int vehicle_reincarnation_value;
	time_t starttime;
	short lastvehiclehp, damagetaken;
	float lastfuel, fuelburned;
	short weatherbonus;
};

static struct MISSION *activemission[MAX_PLAYERS];
/*whether one flightdata data packet with afk flag has been sent already*/
static char tracker_afk_packet_sent[MAX_PLAYERS];
/**
Tracker socket handle.
*/
static int tracker;
/**
Player textdraw handles for passenger satisfaction indicator.
*/
static int ptxt_satisfaction[MAX_PLAYERS];

int missions_is_player_on_mission(int playerid)
{
	return activemission[playerid] != NULL;
}

void missions_create_tracker_socket(AMX *amx)
{
	NC_ssocket_create_(SOCKET_UDP, &tracker);
	if (tracker == SOCKET_INVALID_SOCKET) {
		logprintf("failed to create flighttracker socket");
	} else {
		amx_SetUString(buf144, "127.0.0.1", 144);
		NC_ssocket_connect(tracker, buf144a, TRACKER_PORT);
		*buf32 = 0x04594C46;
		NC_ssocket_send(tracker, buf32a, 4);
	}
}

void missions_destroy_tracker_socket(AMX *amx)
{
	if (tracker != SOCKET_INVALID_SOCKET) {
		*buf32 = 0x05594C46;
		NC_ssocket_send(tracker, buf32a, 4);
	}
}

void missions_init(AMX *amx)
{
	struct AIRPORT *ap;
	struct MISSIONPOINT *msp;
	int apid, lastapid, i, dbcache;

	for (i = 0; i < MAX_PLAYERS; i++) {
		activemission[i] = NULL;
	}

	/*load missionpoints*/
	amx_SetUString(buf144,
		"SELECT a,i,x,y,z,name,t "
		"FROM msp "
		"ORDER BY a ASC,i ASC",
		144);
	NC_mysql_query_(buf144a, &dbcache);
	NC_cache_get_row_count_(&i);
	lastapid = -1;
	while (i--) {
		NC_cache_get_field_int(i, 0, &apid);
		if (apid != lastapid) {
			lastapid = apid;
			ap = airports + apid;
			msp = malloc(sizeof(struct MISSIONPOINT));
			ap->missionpoints = msp;
		} else {
			msp->next = malloc(sizeof(struct MISSIONPOINT));
			msp = msp->next;
		}
		msp->next = NULL;
		msp->ap = ap;
		msp->currentlyactivemissions = 0;
		NC_cache_get_field_int(i, 1, &msp->id);
		NC_cache_get_field_flt(i, 2, msp->x);
		NC_cache_get_field_flt(i, 3, msp->y);
		NC_cache_get_field_flt(i, 4, msp->z);
		NC_cache_get_field_str(i, 6, buf32a);
		NC_cache_get_field_int(i, 6, &msp->type);
		amx_GetUString(msp->name, buf32, MAX_MSP_NAME + 1);
		ap->missiontypes |= msp->type;

	}
	NC_cache_delete(dbcache);

	/*end unfinished dangling flights*/
	amx_SetUString(buf144,
		"UPDATE flg "
		"SET state="EQ(MISSION_STATE_SERVER_ERR)" "
		"WHERE state="EQ(MISSION_STAGE_FLIGHT)"",
		144);
	NC_mysql_tquery_nocb(buf144a);
}

void missions_add_distance(int playerid, float distance_in_m)
{
	if (activemission[playerid] != NULL) {
		activemission[playerid]->actualdistanceM += distance_in_m;
	}
}

/**
Append a pay description and colorized, formatted number to given buffer.

@return amount of characters appended
*/
static
int missions_append_pay(char *buf, char *description, int amount)
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
	int i = numairports;
	struct AIRPORT *ap = airports;
	struct MISSIONPOINT *msp, *tmp;

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

/**
@return speed in units/s
*/
static
float missions_get_vehicle_maximum_speed(int model)
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
		logprintf("mission_get_vehicle_maximum_speed: "
			"unknown model: %d", model);
	case MODEL_SHAMAL:
	case MODEL_HYDRA:
	case MODEL_ANDROM:
	case MODEL_AT400:
		return 81.6f;
	}
}

/**
Gets the pay multiplier for missions done with given vehicle model.
*/
static
float missions_get_vehicle_paymp(int model)
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

/**
Calculates airport tax for given missiontype at given airport.
*/
static
int calculate_airport_tax(struct AIRPORT *ap, int missiontype)
{
	struct MISSIONPOINT *msp;
	struct RUNWAY *rnw;
	int costpermsp, tax = 500, chargernws = 0;

	switch (missiontype) {
	case MISSION_TYPE_PASSENGER_WATER:
		/* skimmer, also for runways (lights maintenance etc) */
		costpermsp = 15;
		chargernws = 1;
		break;
	case MISSION_TYPE_PASSENGER_S:
		/*dodo is excluded from runway cost*/
		costpermsp = 20;
		break;
	case MISSION_TYPE_PASSENGER_M:
		costpermsp = 30;
		chargernws = 1;
		break;
	case MISSION_TYPE_PASSENGER_L:
		costpermsp = 50;
		chargernws = 1;
		break;
	case MISSION_TYPE_CARGO_S:
	case MISSION_TYPE_CARGO_M:
	case MISSION_TYPE_CARGO_L:
		costpermsp = 40;
		chargernws = 1;
		break;
	case MISSION_TYPE_HELI:
	case MISSION_TYPE_HELI_CARGO:
		costpermsp = 30;
		break;
	case MISSION_TYPE_MIL_HELI:
	case MISSION_TYPE_MIL:
	case MISSION_TYPE_AWACS:
	case MISSION_TYPE_STUNT:
	case MISSION_TYPE_CROPD:
	default:
		/* military is govt, and no tax for special missions */
		return 0;
	}

	/*missionpoint cost for every missionpoint this missiontype has*/
	msp = ap->missionpoints;
	while (msp != NULL) {
		if (msp->type & missiontype) {
			tax += costpermsp;
		}
		msp = msp->next;
	}

	/*for helis: 40 per helipad*/
	/*for others: 50 per runway + 15 if VOR + 15 if ILS*/
	rnw = ap->runways;
	while (rnw != ap->runwaysend) {
		if (missiontype & HELI_MISSIONTYPES) {
			if (rnw->type == RUNWAY_TYPE_HELIPAD) {
				tax += 40;
			}
		} else {
			if (chargernws && rnw->type == RUNWAY_TYPE_RUNWAY) {
				tax += 50;
				if (rnw->nav & NAV_VOR) {
					tax += 15;
				}
				if (rnw->nav & NAV_ILS) {
					tax += 15;
				}
				rnw++;
			}
		}
	}
	return tax;
}

#ifdef DEV
/**
@see dev_missions_toggle_closest_point
@see dev_missions_update_closest_point
*/
static int dev_show_closest_point = 0;

void dev_missions_toggle_closest_point(AMX *amx)
{
	int i;
	char buf[144];
	if (!(dev_show_closest_point ^= 1)) {
		for (i = 0; i < playercount; i++) {
			NC_DisablePlayerRaceCheckpoint(players[i]);
		}
	}
	sprintf(buf, "showing mission points: %d", dev_show_closest_point);
	amx_SetUString(buf144, buf, sizeof(buf));
	NC_SendClientMessageToAll(-1, buf144a);
}

void dev_missions_update_closest_point(AMX *amx)
{
	static struct MISSIONPOINT *dev_closest_point[MAX_PLAYERS];

	const float size = 11.0f;
	int i, playerid;
	float px, py, dx, dy, shortestdistance, dist;
	struct AIRPORT *ap, *apend, *closestap;
	struct MISSIONPOINT *mp, *closestmp;

	if (!dev_show_closest_point) {
		return;
	}

	apend = airports + numairports;

	for (i = 0; i < playercount; i++) {
		playerid = players[i];
		NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
		px = amx_ctof(*buf32);
		py = amx_ctof(*buf64);
		shortestdistance = 0x7F800000;
		closestap = NULL;
		ap = airports;
		while (ap != apend) {
			dx = ap->pos.x - px;
			dy = ap->pos.y - py;
			dist = dx * dx + dy * dy;
			if (dist < shortestdistance) {
				shortestdistance = dist;
				closestap = ap;
			}
			ap++;
		}
		if (closestap == NULL) {
			continue;
		}
		shortestdistance = 0x7F800000;
		closestmp = NULL;
		mp = closestap->missionpoints;
		while (mp) {
			dx = mp->x - px;
			dy = mp->y - py;
			dist = dx * dx + dy * dy;
			if (dist < shortestdistance) {
				shortestdistance = dist;
				closestmp = mp;
			}
			mp = mp->next;
		}
		if (closestmp && dev_closest_point[playerid] != closestmp) {
			dev_closest_point[playerid] = closestmp;
			nc_params[0] = 9;
			nc_params[1] = playerid;
			nc_params[2] = 2;
			nc_params[3] = nc_params[6] = amx_ftoc(closestmp->x);
			nc_params[4] = nc_params[7] = amx_ftoc(closestmp->y);
			nc_params[5] = nc_params[8] = amx_ftoc(closestmp->z);
			nc_params[9] = amx_ftoc(size);
			NC(n_SetPlayerRaceCheckpoint);
		}
	}
}
#endif /*DEV*/

static
struct AIRPORT *getRandomAirportForType(AMX *amx, int missiontype, struct AIRPORT *blacklistedairport)
{
	struct AIRPORT *ap = airports, **aps;
	int apsc = 0;
	int i = numairports;

	aps = malloc(sizeof(aps) * numairports);

	while (i--) {
		if (ap != blacklistedairport && ap->missiontypes & missiontype) {
			aps[apsc++] = ap;
		}
		ap++;
	}

	switch (apsc) {
	case 0: ap = NULL; break;
	case 1: ap = aps[0]; break;
	default:
		NC_random(apsc);
		ap = aps[nc_result];
		break;
	}

	free(aps);
	return ap;
}

static
struct MISSIONPOINT *getRandomEndPointForType(AMX *amx, int missiontype, struct AIRPORT *blacklistedairport)
{
#define TMP_PT_SIZE 7
	struct MISSIONPOINT *points[TMP_PT_SIZE], *msp;
	int pointc = 0, leastamtofcurrentmissions = 1000000;
	struct AIRPORT *airport = getRandomAirportForType(amx, missiontype, blacklistedairport);

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

	NC_random(pointc);
	return points[nc_result];
#undef TMP_PT_SIZE
}

/* native Missions_Create(playerid, Float:x, Float:y, Float:z, vehicleid, vv, vehiclehp, msg[], querybuf[]) */
cell AMX_NATIVE_CALL Missions_Create(AMX *amx, cell *params)
{
	cell *bufaddr;
	char buf[255], tmpuseridornullbuf[12];
	struct MISSIONPOINT *msp, *startpoint, *endpoint;
	struct AIRPORT *ap = airports, *closestap = NULL;
	struct dbvehicle *veh;
	struct MISSION *mission;
	int missiontype, i = numairports;
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

	activemission[playerid] = mission = malloc(sizeof(struct MISSION));
	mission->id = -1;
	mission->stage = MISSION_STAGE_CREATE;
	mission->missiontype = missiontype;
	mission->startpoint = startpoint;
	mission->endpoint = endpoint;
	mission->distance = sqrt(dx * dx + dy * dy);
	mission->actualdistanceM = 0.0f;
	mission->passenger_satisfaction = 100;
	mission->veh = veh;
	mission->vehicle_reincarnation_value = vv;
	mission->starttime = time(NULL);
	mission->lastvehiclehp = (short) amx_ctof(params[7]);
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

/**
Cleanup stuff and free memory. Does not query. Does send a tracker msg.

Call when ending a mission.

@param mission mission of the player. Must match with given playerid.
*/
static
void missions_cleanup(AMX *amx, struct MISSION *mission, int playerid)
{
	/* flight tracker packet 3 */
	buf32[0] = 0x03594C46;
	buf32[1] = mission->id;
	NC_ssocket_send(tracker, buf32a, 8);

	mission->startpoint->currentlyactivemissions--;
	mission->endpoint->currentlyactivemissions--;
	if (mission->missiontype & PASSENGER_MISSIONTYPES) {
		NC_PlayerTextDrawHide(playerid, ptxt_satisfaction[playerid]);
	}

	free(mission);
	activemission[playerid] = NULL;
}

/**
@param reason one of MISSION_STATE_ constants
*/
static
void missions_end_unfinished(
	AMX *amx, struct MISSION *mission, int playerid, int reason)
{
	sprintf(cbuf144,
	        "UPDATE flg "
		"SET state=%d,tlastupdate=UNIX_TIMESTAMP(),adistance=%f "
		"WHERE id=%d",
	        reason,
		mission->actualdistanceM,
	        mission->id);
	amx_SetUString(buf4096, cbuf144, 144);
	NC_mysql_tquery_nocb(buf4096a);

	missions_cleanup(amx, mission, playerid);
}

void missions_on_player_connect(AMX *amx, int playerid)
{
	nc_params[0] = 4;
	nc_params[1] = playerid;
	nc_paramf[2] = 88.0f;
	nc_paramf[3] = 425.0f;
	nc_params[4] = underscorestringa;
	NC_(n_CreatePlayerTextDraw, nc_params + 2);
	ptxt_satisfaction[playerid] = nc_params[2];
	nc_paramf[3] = 0.3f;
	nc_paramf[4] = 1.0f;
	NC(n_PlayerTextDrawLetterSize);

	nc_params[0] = 3;
	nc_params[2] = 255;
	NC(n_PlayerTextDrawBackgroundColor);
	nc_params[3] = 2;
	NC(n_PlayerTextDrawAlignment);
	nc_params[3] = 1;
	NC(n_PlayerTextDrawFont);
	NC(n_PlayerTextDrawSetProportional);
	NC(n_PlayerTextDrawSetOutline);
	nc_params[3] = -1;
	NC(n_PlayerTextDrawColor);
}

void missions_on_player_death(AMX *amx, int playerid)
{
	struct MISSION *mission;
	int stopreason;
	float hp;

	if ((mission = activemission[playerid]) != NULL) {
		NC_DisablePlayerRaceCheckpoint(playerid);
		stopreason = MISSION_STATE_DIED;
		NC_GetPlayerVehicleID(playerid);
		if (nc_result) {
			hp = anticheat_NC_GetVehicleHealth(amx, nc_result);
			if (hp <= 200.0f) {
				stopreason = MISSION_STATE_CRASHED;
			}
		}
		missions_end_unfinished(amx, mission, playerid, stopreason);
	}
}

void missions_on_player_disconnect(AMX *amx, int playerid)
{
	struct MISSION *miss;

	if ((miss = activemission[playerid]) != NULL) {
		missions_end_unfinished(
			amx, miss, playerid, MISSION_STATE_ABANDONED);
	}
}

void missions_send_tracker_data(
	AMX *amx, int playerid, int vehicleid, float hp,
	struct vec3 *vpos, struct vec3 *vvel, int afk, int engine)
{
	struct MISSION *mission;
	unsigned char flags;
	short spd, alt, hpv;

	if ((mission = activemission[playerid]) == NULL ||
		mission->veh->spawnedvehicleid != vehicleid ||
		(afk && tracker_afk_packet_sent[playerid]))
	{
		return;
	}

	tracker_afk_packet_sent[playerid] = flags = afk == 1;
	if (engine) {
		flags |= 2;
	}

	hpv = (short) hp;
	alt = (short) vpos->z;
	spd = (short) (VEL_TO_KTS_VAL * sqrt(
		vvel->x * vvel->x + vvel->y * vvel->y + vvel->z * vvel->z));

	/* flight tracker packet 2 */
	buf32[0] = 0x02594C46;
	buf32[1] = mission->id;
	cbuf32[8] = flags;
	cbuf32[9] = (char) mission->passenger_satisfaction;
	memcpy(cbuf32 + 10, &spd, 2);
	memcpy(cbuf32 + 12, &alt, 2);
	memcpy(cbuf32 + 14, &hpv, 2);
	memcpy(cbuf32 + 16, &mission->veh->fuel, 4);
	memcpy(cbuf32 + 20, &vpos->x, 4);
	memcpy(cbuf32 + 24, &vpos->y, 4);
	NC_ssocket_send(tracker, buf32a, 28);
}

/**
The /automission cmd, toggles automatically starting new mission on finish.

Aliases: /autow
*/
int missions_cmd_automission(CMDPARAMS)
{
	static const char
		*ENABLED = "Constant work enabled.",
		*DISABLED = "Constant work disabled.";

	if ((prefs[playerid] ^= PREF_CONSTANT_WORK) & PREF_CONSTANT_WORK) {
		amx_SetUString(buf144, ENABLED, 144);
	} else {
		amx_SetUString(buf144, DISABLED, 144);
	}
	NC_SendClientMessage(playerid, COL_SAMP_GREY, buf144a);
	return 1;
}

/**
The /cancelmission cmd. Aliases: /s

Stops current mission for the player, for a fee.
*/
int missions_cmd_cancelmission(CMDPARAMS)
{
	static const char *NOMISSION = WARN"You're not on an active mission.";

	struct MISSION *mission;

	if ((mission = activemission[playerid]) != NULL) {
		NC_DisablePlayerRaceCheckpoint(playerid);
		money_take(amx, playerid, 5000);
		missions_end_unfinished(amx, mission,
			playerid, MISSION_STATE_DECLINED);
	} else {
		amx_SetUString(buf144, NOMISSION, 144);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
	}
	return 1;
}

/* native Missions_EndUnfinished(playerid, reason) */
cell AMX_NATIVE_CALL Missions_EndUnfinished(AMX *amx, cell *params)
{
	const int playerid = params[1], reason = params[2];
	struct MISSION *mission;
	char q[200];

	if ((mission = activemission[playerid]) == NULL) {
		return 0;
	}

	sprintf(q,
	        "UPDATE flg "
		"SET state=%d,tlastupdate=UNIX_TIMESTAMP(),adistance=%f "
		"WHERE id=%d",
	        reason,
		mission->actualdistanceM,
	        mission->id);
	amx_SetUString(buf4096, q, sizeof(q));
	NC_mysql_tquery_nocb(buf4096a);

	missions_cleanup(amx, mission, playerid);
	return 1;
}

/* native Missions_EnterCheckpoint(playerid, vehicleid, vv, x, y, z, errmsg[]) */
cell AMX_NATIVE_CALL Missions_EnterCheckpoint(AMX *amx, cell *params)
{
	const int playerid = params[1], vehicleid = params[2], vv = params[3];
	float x, y, z;
	struct MISSION *mission;
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
		NC_PlayerTextDrawHide(playerid, ptxt_satisfaction[playerid]);
		return MISSION_ENTERCHECKPOINTRES_UNLOAD;
	default:
		logprintf("ERR: player uid %d entered mission checkpoint in invalid stage: %d",
		          pdata[playerid] == NULL ? -1 : pdata[playerid]->userid,
		          mission->stage);
		return 0;
	}

exit_set_errmsg:
	amx_GetAddr(amx, params[7], &addr);
	amx_SetUString(addr, msg, sizeof(msg));
	return MISSION_ENTERCHECKPOINTRES_ERR;
}

int missions_get_stage(int playerid)
{
	struct MISSION *mission = activemission[playerid];
	if (mission != NULL) {
		return mission->stage;
	}
	return MISSION_STAGE_NOMISSION;
}

/* native Missions_GetState(playerid) */
cell AMX_NATIVE_CALL Missions_GetState(AMX *amx, cell *params)
{
	return missions_get_stage(params[1]);
}

/* native Missions_OnVehicleRefueled(playerid, vehicleid, Float:refuelamount) */
cell AMX_NATIVE_CALL Missions_OnVehicleRefueled(AMX *amx, cell *params)
{
	struct MISSION *miss;
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
	struct MISSION *miss;
	const int playerid = params[1], vehicleid = params[2];
	const float newhp = amx_ctof(params[4]), hpdiff = newhp - amx_ctof(params[3]);

	if ((miss = activemission[playerid]) != NULL &&
		miss->veh->spawnedvehicleid == vehicleid)
	{
		miss->damagetaken += (short) hpdiff;
		miss->lastvehiclehp = (short) newhp;
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
	NC_random(MISSION_WEATHERBONUS_DEVIATION);
	bonusvalue += nc_result;
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
	struct MISSION *mission;
	cell *addr;
	char buf[144];

	if ((mission = activemission[playerid]) == NULL ||
		mission->stage != MISSION_STAGE_LOAD)
	{
		return 0;
	}

	if (prefs[playerid] & PREF_WORK_AUTONAV) {
		nav_navigate_to_airport(
			amx,
			mission->veh->spawnedvehicleid,
			mission->veh->model,
			mission->endpoint->ap);
	}

	if (mission->missiontype & PASSENGER_MISSIONTYPES) {
		sprintf(cbuf32, SATISFACTION_TEXT_FORMAT, 100);
		amx_SetUString(buf32_1, cbuf32, 32);
		nc_params[0] = 3;
		nc_params[1] = playerid;
		nc_params[2] = ptxt_satisfaction[playerid];
		nc_params[3] = buf32_1a;
		NC(n_PlayerTextDrawSetString);
		nc_params[0] = 2;
		NC(n_PlayerTextDrawShow);
	}

	mission->stage = MISSION_STAGE_FLIGHT;
	sprintf(buf,
	        "UPDATE flg "
		"SET tload=UNIX_TIMESTAMP(),tlastupdate=UNIX_TIMESTAMP() "
		"WHERE id=%d",
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
	struct MISSION *mission;
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

	tmp = (int) (mission->lastvehiclehp - vehiclehp);
	if (tmp < 0) {
		tmp = 0;
		pcheat -= 250000;
		sprintf(buf, "flg(#%d) vhh: was: %hd now: %.0f", mission->id, mission->lastvehiclehp, vehiclehp);
		amx_SetUString(addr + 2100, buf, sizeof(buf));
	} else {
		*(addr + 2100) = 0; /* ac log hp cheat */
	}
	mission->damagetaken += tmp;
	mission->lastvehiclehp = (short) vehiclehp;
	mission->fuelburned += mission->lastfuel - mission->veh->fuel;

	totaltime = (int) difftime(time(NULL), mission->starttime);
	duration_m = totaltime % 60;
	duration_h = (totaltime - duration_m) / 60;

	/* don't use adistance because it also includes z changes */
	mintime = (int) (mission->distance / missions_get_vehicle_maximum_speed(mission->veh->model));
	if (totaltime < mintime) {
		pcheat -= 250000;
		sprintf(buf, "flg(#%d) too fast: min: %d actual: %d", mission->id, mintime, totaltime);
		amx_SetUString(addr + 2000, buf, sizeof(buf));
	} else {
		*(addr + 2000) = 0; /* ac log speed cheat */
	}

	paymp = missions_get_vehicle_paymp(mission->veh->model);
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
	        "paymp=%f,damage=%d WHERE id=%d",
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
	        mission->actualdistanceM,
	        paymp,
	        mission->damagetaken,
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
	             mission->actualdistanceM,
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
		p += missions_append_pay(buf + p, "{ffffff}Airport Tax:\t\t", ptax);
	}
	if (mission->weatherbonus) {
		p += missions_append_pay(buf + p, "{ffffff}Weather bonus:\t\t", mission->weatherbonus);
	}
	p += missions_append_pay(buf + p, "{ffffff}Distance Pay:\t\t", pdistance);
	if (mission->missiontype & PASSENGER_MISSIONTYPES) {
		if (psatisfaction > 0) {
			p += missions_append_pay(buf + p, "{ffffff}Satisfaction Bonus:\t", psatisfaction);
		} else {
			p += missions_append_pay(buf + p, "{ffffff}Satisfaction Penalty:\t", psatisfaction);
		}
	}
	if (pdamage) {
		p += missions_append_pay(buf + p, "{ffffff}Damage Penalty:\t", pdamage);
	}
	if (pcheat) {
		p += missions_append_pay(buf + p, "{ffffff}Cheat Penalty:\t\t", pcheat);
	}
	if (pbonus) {
		p += missions_append_pay(buf + p, "{ffffff}Bonus:\t\t\t", pbonus);
	}
	p += missions_append_pay(buf + p, "\n\n\t{ffffff}Total Pay: ", ptotal);
	buf[--p] = 0;
	amx_SetUString(addr + 1000, buf, sizeof(buf));

	amx_GetAddr(amx, params[3], &addr);
	*addr = ptotal;

	missions_cleanup(amx, mission, playerid);
	return 1;
}

/* native Missions_Start(playerid, missionid, &Float:x, &Float:y, &Float:z,
                         querybuf[], trackerbuf[]) */
cell AMX_NATIVE_CALL Missions_Start(AMX *amx, cell *params)
{
	const int playerid = params[1];
	struct MISSION *mission = activemission[playerid];
	int vehmodel;
	float fuelcapacity;
	cell *addr;
	char msg[144];
	const char *msptypename = NULL;

	if (mission == NULL) {
		return 0;
	}

	tracker_afk_packet_sent[playerid] = 0;

	mission->id = params[2];
	mission->stage = MISSION_STAGE_PRELOAD;
	amx_GetAddr(amx, params[3], &addr);
	*addr = amx_ftoc(mission->startpoint->x);
	amx_GetAddr(amx, params[4], &addr);
	*addr = amx_ftoc(mission->startpoint->y);
	amx_GetAddr(amx, params[5], &addr);
	*addr = amx_ftoc(mission->startpoint->z);

	sprintf(msg,
		"Flight from %s (%s) %s to %s (%s) %s",
		mission->startpoint->ap->name,
		mission->startpoint->ap->beacon,
		mission->startpoint->name,
		mission->endpoint->ap->name,
		mission->endpoint->ap->beacon,
		mission->endpoint->name);
	amx_GetAddr(amx, params[6], &addr);
	amx_SetUString(addr, msg, sizeof(msg));

	/* flight tracker packet 1 */
	buf32[0] = 0x01594C46;
	buf32[1] = mission->id;
	fuelcapacity = model_fuel_capacity(vehmodel = mission->veh->model);
	memcpy(cbuf32 + 8, &fuelcapacity, 4);
	memcpy(cbuf32 + 12, &vehmodel, 2);
	cbuf32[14] = pdata[playerid]->namelen;
	memset(cbuf32 + 15, 0, 24); /* don't leak random data */
	strcpy(cbuf32 + 15, pdata[playerid]->name);
	/*buf32 is len 32 * 4 so 40 is fine*/
	NC_ssocket_send(tracker, buf32a, 40);

	if (prefs[playerid] & PREF_WORK_AUTONAV) {
		nav_navigate_to_airport(
			amx,
			mission->veh->spawnedvehicleid,
			mission->veh->model,
			mission->startpoint->ap);
	}

	return 1;
}

void missions_update_satisfaction(AMX *amx, int pid, int vid, struct quat *vrot)
{
	struct MISSION *miss;
	int last_satisfaction;
	float qw, qx, qy, qz;
	float tmpvalue = 0.0f;;

	if ((miss = activemission[pid]) != NULL &&
		miss->stage == MISSION_STAGE_FLIGHT &&
		miss->veh->spawnedvehicleid == vid)
	{
		last_satisfaction = miss->passenger_satisfaction;
		qx = vrot->qx;
		qy = vrot->qy;
		qz = vrot->qz;
		qw = vrot->qw;
		/* pitch */
		tmpvalue = fabsf(100.0f * 2.0f * (qy * qz - qw * qx)) - 46.0f;
		if (tmpvalue > 0.0) {
			miss->passenger_satisfaction -= (int) (tmpvalue / 2.0f);
			if (miss->passenger_satisfaction < 0) {
				miss->passenger_satisfaction = 0;
			}
		}
		/* roll */
		tmpvalue = fabsf(100.0f * 2.0f * (qx * qz + qw * qy)) - 61.0f;
		if (tmpvalue > 0.0) {
			miss->passenger_satisfaction -= (int) (tmpvalue / 2.0f);
			if (miss->passenger_satisfaction < 0) {
				miss->passenger_satisfaction = 0;
			}
		}
		if (last_satisfaction != miss->passenger_satisfaction) {
			sprintf(cbuf32_1,
				SATISFACTION_TEXT_FORMAT,
				miss->passenger_satisfaction);
			amx_SetUString(buf32, cbuf32_1, 32);
			NC_PlayerTextDrawSetString(
				pid, ptxt_satisfaction[pid], buf32a);
		}
	}
}
