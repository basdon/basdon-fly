#define TRACKER_PORT 7766

#define MAX_SPEED_SQ ((3.0f / VEL_TO_KPH) * (3.0f / VEL_TO_KPH))

static struct TEXTDRAW td_satisfaction = { "textdraw", 50, NULL };

/**
Keeps track how much missions a playerid has started and/or stopped (number increased on mission start AND stop).
Pointers to missions must not be passed in callback data, since they could be free'd before that happens.
Instead, callbacks should pass this number, and if it is equal to the current number it means that the
activemission[playerid] is still the same mission as it was when the async func was started.
*/
static int number_missions_started_stopped[MAX_PLAYERS];

#define MISSION_FLAG_NEEDS_TRACKER_RESUME_FLAG 0x1
#define MISSION_FLAG_IS_PAUSED_SET_TO_IN_PROGRESS_ON_NEXT_DRIVERSYNC 0x2
/*whether or not the last_syncdata field is set*/
#define MISSION_FLAG_HAS_LAST_SYCDATA 0x4
#define MISSION_FLAG_WAS_PAUSED 0x8

struct MISSION {
	int id;
	int missiontype;
	struct MISSIONPOINT *startpoint, *endpoint;
	float distance, actualdistanceM;
	int passenger_satisfaction;
	short vehmodel;
	/**
	The vehicleid of the vehicle the mission was started with.
	*/
	int vehicleid;
	/**if this mission is the continuation of a paused mission, this is the time of resuming*/
	time_t starttime;
	/*if this mission is the continuation of a paused mission, this is the the total duration seconds before resuming*/
	int previous_duration_s;
	short lastvehiclehp, damagetaken;
	float lastfuel, fuelburned;
	short weatherbonus;
	int flags;
	struct SYNCDATA_Driver last_syncdata;
};

struct PAUSED_MISSION {
	int id;
	int missiontype;
	struct MISSIONPOINT *startpoint, *endpoint;
	int passenger_satisfaction;
	int vehid;
	short vehmodel;
	struct vec3 pos;
	struct Quaternion quat;
	struct vec3 vel;
	float fuel_percentage;
	short hp;
	int gear_keys;
	int udlrkeys;
	short weatherbonus;
	int damage;
	float distance, actualdistanceM;
	int previous_duration_s;
	float fuel_burned_percentage;
	int misc;
	/**A paused mission can be cancelled/aborted while still in the 'handover' phase.
	When the player respawns, the current paused mission may still be set (because it gets cleared in the
	'sync' timer callback, which may happen later), so the player might get a dialog to resume the mission
	that was just cancelled. This flag exists to prevent that, if a player's paused mission is set, but this
	cancelled flag is also set, that paused mission should not be considered.*/
	char is_cancelled : 1;
};

static struct PAUSED_MISSION *paused_mission[MAX_PLAYERS];

/**
 * Available mission point types for player, based on their class and vehicle.
 */
static int missions_available_msptype_mask[MAX_PLAYERS];
/**
 * Available mission point types for player, only for the vehicle they're in.
 */
static int missions_player_vehicle_msptype_mask[MAX_PLAYERS];

#define MISSIONPOINT_INDICATOR_STATE_FREE 0
#define MISSIONPOINT_INDICATOR_STATE_USED 1
#define MISSIONPOINT_INDICATOR_STATE_AVAILABLE 2 /*indicator created, but it can be discarded*/
#define MISSIONPOINT_INDICATOR_COLOR_RED 0
#define MISSIONPOINT_INDICATOR_COLOR_GREEN 1
#define MISSIONPOINT_INDICATOR_COLOR_BLUE 2
struct MSP_INDICATOR {
	struct MISSIONPOINT *msp[MAX_MISSION_INDICATORS];
	/**See MISSIONPOINT_INDICATOR_STATE_* defs.*/
	char state[MAX_MISSION_INDICATORS];
	/**See MISSIONPOINT_INDICATOR_COLOR_* defs.*/
	char color[MAX_MISSION_INDICATORS];
};
static struct MSP_INDICATOR msp_indicators[MAX_PLAYERS];
/**
Holds the missionpoint the player is in range of (only when player is driver).
It is always valid for the player's class and the vehicle the player is driver in when not NULL.
*/
static struct MISSIONPOINT *active_msp[MAX_PLAYERS];
/**
Holds the missionpoint that has an active checkpoint for player, for locating purposes.
*/
static struct MISSIONPOINT *locating_msp[MAX_PLAYERS];
/**
See {@code MISSION_STAGE} definitions.
*/
static char mission_stage[MAX_PLAYERS];
static struct MISSION *activemission[MAX_PLAYERS];
/*whether one flightdata data packet with afk flag has been sent already*/
static char tracker_afk_packet_sent[MAX_PLAYERS];
/**
Tracker socket handle.
*/
static int tracker;

#define TRACKER_HDR_FLAG_CANARY 0x1
#define TRACKER_HDR_FLAG_HAS_BANK_PITCH 0x2
#define TRACKER_HDR_FLAG_HAS_ALTITUDE 0x4

static
int missions_get_weatherbonus_for_weather(int weather)
{
	switch (weather) {
	case WEATHER_SF_RAINY:
	case WEATHER_CS_RAINY:
		return MISSION_WEATHERBONUS_RAINY + NC_random(MISSION_WEATHERBONUS_DEVIATION);
	case WEATHER_SF_FOGGY:
		return MISSION_WEATHERBONUS_FOGGY + NC_random(MISSION_WEATHERBONUS_DEVIATION);
	case WEATHER_DE_SANDSTORMS:
		return MISSION_WEATHERBONUS_SANDSTORM + NC_random(MISSION_WEATHERBONUS_DEVIATION);
	default:
		return 0;
	}
}

static
int missions_get_initial_weatherbonus()
{
	register int a, b, c;

	a = missions_get_weatherbonus_for_weather(weather.current);
	b = missions_get_weatherbonus_for_weather(weather.upcoming);
	c = missions_get_weatherbonus_for_weather(weather.locked);
	if (b > a) {
		a = b;
	}
	if (c > a) {
		return c;
	}
	return a;
}

static
void missions_hide_jobmap_set_stage_set_controllable(int playerid)
{
	jobmap_hide(playerid);
	mission_stage[playerid] = MISSION_STAGE_NOMISSION;
	TogglePlayerControllable(playerid, 1);
	ui_closed(playerid, ui_mission_map);
}

/**
Must only be called if the player has a valid active missionpoint.
*/
static
void missions_show_jobmap_set_stage_set_controllable(int playerid)
{
	register struct MISSIONPOINT *msp;
	register unsigned int mission_type;

	if (ui_try_show(playerid, ui_mission_map)) {
		HideGameTextForPlayer(playerid); /*The key or /w help text might still be showing.*/
		msp = active_msp[playerid];
		mission_type = missions_available_msptype_mask[playerid] & msp->type;
		jobmap_show(playerid, mission_type, msp);
		mission_stage[playerid] = MISSION_STAGE_JOBMAP;
		TogglePlayerControllable(playerid, 0);
	}
}

/**
TODO: prioritize mission points that have the least amount of active missions

@return NULL if none applicable for given type mast
*/
static
struct MISSIONPOINT *missions_get_random_msp(struct AIRPORT *airport, unsigned int mission_type_mask)
{
	register struct MISSIONPOINT *tmp_msp;
	struct MISSIONPOINT *applicable_missionpoints[MAX_MISSIONPOINTS_PER_AIRPORT];
	int num_applicable_msp, mspidx;

	/*Collect all applicable missionpoints.*/
	num_applicable_msp = 0;
	for (mspidx = 0; mspidx < airport->num_missionpts; mspidx++) {
		tmp_msp = airport->missionpoints + mspidx;
		if (tmp_msp->type & mission_type_mask) {
			applicable_missionpoints[num_applicable_msp++] = tmp_msp;
		}
	}

	if (!num_applicable_msp) {
		return NULL;
	}

	/*Select one.*/
	mspidx = 0;
	if (num_applicable_msp > 1) {
		mspidx = NC_random(num_applicable_msp);
	}
	return applicable_missionpoints[mspidx];
}

int missions_is_player_on_mission(int playerid)
{
	return activemission[playerid] != NULL;
}

static
int missions_is_player_resuming_mission(int playerid)
{
	return activemission[playerid] && (activemission[playerid]->flags & MISSION_FLAG_WAS_PAUSED);
}

void missions_create_tracker_socket()
{
	tracker = NC_ssocket_create(SOCKET_UDP);
	if (tracker == SOCKET_INVALID_SOCKET) {
		logprintf("failed to create flighttracker socket");
	} else {
		B144("127.0.0.1");
		NC_ssocket_connect(tracker, buf144a, TRACKER_PORT);
		*buf32 = 0x04594C46;
		NC_ssocket_send(tracker, buf32a, 4);
	}
}

void missions_destroy_tracker_socket()
{
	if (tracker != SOCKET_INVALID_SOCKET) {
		*buf32 = 0x05594C46;
		NC_ssocket_send(tracker, buf32a, 4);
	}
}

/**
 * Indicators show missionpoint availability according to class and vehicle:
 * - when wrong class: red enex (objid 19605)
 * - when correct class but wrong vehicle: blue enex (objid 19607)
 * - when correct class and correct vehicle: green enex (objid 19606)
 */
static
void missions_update_missionpoint_indicators(int playerid, float player_x, float player_y, float player_z)
{
#define AIRPORT_RANGE_SQ (1500.0f * 1500.0f)
#define POINT_RANGE_SQ (500.0f * 500.0f)

	struct RPCDATA_CreateObject rpcdata_CreateObject;
	struct RPCDATA_DestroyObject rpcdata_DestroyObject;
	struct BitStream bitstream;
	register struct MISSIONPOINT *msp;
	struct MSP_INDICATOR *indicators;
	unsigned int vehicle_msptype_mask, class_msptype_mask;
	int airportidx, indicatoridx, missionptidx, idxtouse;
	float dx, dy, dz;
	char color;

	indicators = &msp_indicators[playerid];
	vehicle_msptype_mask = missions_player_vehicle_msptype_mask[playerid];
	class_msptype_mask = CLASS_MSPTYPES[classid[playerid]];

	/*change now out-of-range ones, and wrong color ones, to AVAILABLE*/
	for (indicatoridx = 0; indicatoridx < MAX_MISSION_INDICATORS; indicatoridx++) {
		if (indicators->state[indicatoridx] == MISSIONPOINT_INDICATOR_STATE_USED) {
			/*Check if the indicator's color still matches the required color.*/
			msp = indicators->msp[indicatoridx];
			if (msp->type & class_msptype_mask) {
				if (msp->type & class_msptype_mask & vehicle_msptype_mask) {
					color = MISSIONPOINT_INDICATOR_COLOR_GREEN;
				} else {
					color = MISSIONPOINT_INDICATOR_COLOR_BLUE;
				}
			} else {
				color = MISSIONPOINT_INDICATOR_COLOR_RED;
			}
			if (color == indicators->color[indicatoridx]) {
				/*Color ok, check if it's in range.*/
				dx = msp->pos.x - player_x;
				dy = msp->pos.y - player_y;
				if (dx * dx + dy * dy <= POINT_RANGE_SQ + 250.0f) {
					continue;
				}
			}

			/*The last loop of this method will delete AVAILABLE ones if they're not reused by that point.*/
			indicators->state[indicatoridx] = MISSIONPOINT_INDICATOR_STATE_AVAILABLE;
#ifdef MISSIONS_LOG_POINT_INDICATOR_ALLOC
			printf("%d %p available %lu\n", indicatoridx, indicators->msp[indicatoridx], time_timestamp());
#endif
		}
	}

	/*check for new ones in range*/
	for (airportidx = 0; airportidx < numairports; airportidx++) {
		dx = airports[airportidx].pos.x - player_x;
		dy = airports[airportidx].pos.y - player_y;
		if (dx * dx + dy * dy < AIRPORT_RANGE_SQ) {
			msp = airports[airportidx].missionpoints;
			for (missionptidx = airports[airportidx].num_missionpts; missionptidx > 0; missionptidx--) {
				dx = msp->pos.x - player_x;
				dy = msp->pos.y - player_y;
				if (dx * dx + dy * dy > POINT_RANGE_SQ) {
					goto next;
				}

				if (msp->type & class_msptype_mask) {
					if (msp->type & class_msptype_mask & vehicle_msptype_mask) {
						color = MISSIONPOINT_INDICATOR_COLOR_GREEN;
					} else {
						color = MISSIONPOINT_INDICATOR_COLOR_BLUE;
					}
				} else {
					color = MISSIONPOINT_INDICATOR_COLOR_RED;
				}

				idxtouse = -1;
				for (indicatoridx = 0; indicatoridx < MAX_MISSION_INDICATORS; indicatoridx++) {
					if (indicators->state[indicatoridx] != MISSIONPOINT_INDICATOR_STATE_USED) {
						idxtouse = indicatoridx;
					} else if (indicators->msp[indicatoridx] == msp) {
						/*This missionpoint is already created.*/
						goto next;
					}
				}

				if (idxtouse != -1) {
					/*19605 red enex, 19606 green enex, 19607 blue enex*/
					rpcdata_CreateObject.objectid = OBJECT_MISSION_INDICATOR_BASE + idxtouse;
					rpcdata_CreateObject.modelid = 19605 + color;
					rpcdata_CreateObject.x = msp->pos.x;
					rpcdata_CreateObject.y = msp->pos.y;
					rpcdata_CreateObject.z = msp->pos.z;
					rpcdata_CreateObject.rx = 0.0f;
					rpcdata_CreateObject.ry = 0.0f;
					rpcdata_CreateObject.rz = 0.0f;
					rpcdata_CreateObject.drawdistance = 1000.0f;
					rpcdata_CreateObject.no_camera_col = 0;
					rpcdata_CreateObject.attached_object_id = -1;
					rpcdata_CreateObject.attached_vehicle_id = -1;
					rpcdata_CreateObject.num_materials = 0;
					bitstream.ptrData = &rpcdata_CreateObject;
					bitstream.numberOfBitsUsed = sizeof(rpcdata_CreateObject) * 8;
					SAMP_SendRPCToPlayer(RPC_CreateObject, &bitstream, playerid, 2);
					indicators->state[idxtouse] = MISSIONPOINT_INDICATOR_STATE_USED;
					indicators->color[idxtouse] = color;
					indicators->msp[idxtouse] = msp;
#ifdef MISSIONS_LOG_POINT_INDICATOR_ALLOC
					printf("%d %p created %lu\n", idxtouse, msp, time_timestamp());
#endif
				}
next:
				msp++;
			}
		}
	}

	/*delete AVAILABLE ones*/
	for (indicatoridx = 0; indicatoridx < MAX_MISSION_INDICATORS; indicatoridx++) {
		if (indicators->state[indicatoridx] == MISSIONPOINT_INDICATOR_STATE_AVAILABLE) {
			rpcdata_DestroyObject.objectid = OBJECT_MISSION_INDICATOR_BASE + indicatoridx;
			bitstream.ptrData = &rpcdata_DestroyObject;
			bitstream.numberOfBitsUsed = sizeof(rpcdata_DestroyObject) * 8;
			SAMP_SendRPCToPlayer(RPC_DestroyObject, &bitstream, playerid, 2);
			indicators->state[indicatoridx] = MISSIONPOINT_INDICATOR_STATE_FREE;
#ifdef MISSIONS_LOG_POINT_INDICATOR_ALLOC
			printf("%d %p destroyed %lu\n", indicatoridx, indicators->msp[indicatoridx], time_timestamp());
#endif
		}
	}

	/*update active_msp and show 'press ...' text if needed*/
	if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
		for (indicatoridx = 0; indicatoridx < MAX_MISSION_INDICATORS; indicatoridx++) {
			if (indicators->state[indicatoridx] == MISSIONPOINT_INDICATOR_STATE_USED) {
				msp = indicators->msp[indicatoridx];
				dx = msp->pos.x - player_x;
				dy = msp->pos.y - player_y;
				dz = msp->pos.z - player_z;
				if (dx * dx + dy * dy + dz * dz < MISSION_CP_RAD_SQ) {
					if (active_msp[playerid] == indicators->msp[indicatoridx]) {
						return;
					}

					/*TODO: show better help (tell class or vehicle)*/
					/*TODO: this keep showing constantly (because active_msp is not set,
						because active_msp shouldn't be set, because it's not a valid msp for the player)*/
					switch (indicators->color[indicatoridx]) {
					case MISSIONPOINT_INDICATOR_COLOR_RED:
						GameTextForPlayer(playerid, 3000, 3,
							"~r~This missionpoint requires a different class (/reclass)");
						return;
					case MISSIONPOINT_INDICATOR_COLOR_BLUE:
						GameTextForPlayer(playerid, 3000, 3,
							"~r~This missionpoint requires a different vehicle");
						return;
					}

					active_msp[playerid] = indicators->msp[indicatoridx];
					if (activemission[playerid] && active_msp[playerid] == activemission[playerid]->endpoint) {
						GameTextForPlayer(playerid, 3000, 3,
							"~w~Press ~b~~k~~CONVERSATION_YES~~w~ to unload,~n~or type ~b~/w");
						return;
					}

					/*TODO: sometimes when entering/exiting vehicle, this text doesn't show even though code gets hit*/
					GameTextForPlayer(playerid, 3000, 3,
						"~w~Press ~b~~k~~CONVERSATION_YES~~w~ to view missions,~n~or type ~b~/w");
					if (active_msp[playerid] == locating_msp[playerid] && locating_msp[playerid]) {
						locating_msp[playerid] = NULL;
						DisablePlayerRaceCheckpoint(playerid);
					}
					goto active_msp_changed;
				}
			}
		}
	}

	if (active_msp[playerid]) {
		active_msp[playerid] = NULL;
active_msp_changed:
		if (mission_stage[playerid] == MISSION_STAGE_JOBMAP) {
			missions_hide_jobmap_set_stage_set_controllable(playerid);
		}
	}

#undef AIRPORT_RANGE_SQ
#undef POINT_RANGE_SQ
}

static
void cb_missions_post_to_discord_after_finish_query_done(void *data)
{
	discordflightlog_trigger((int) data);
}

static
void missions_dispose()
{
	free(td_satisfaction.rpcdata);
}

static
void missions_init()
{
	/*textdraws*/
	textdraws_load_from_file("jobsatisfaction", TEXTDRAW_JOBSATISFACTION, 1, &td_satisfaction);

	/*missionpoints are loaded in airport.c*/

	/*end unfinished dangling flights*/
	atoc(buf144,
		"UPDATE flg "
		"SET state="EQ(MISSION_STATE_SERVER_ERR)" "
		"WHERE state="EQ(MISSION_STATE_INPROGRESS)"",
		144);
	NC_mysql_tquery_nocb(buf144a);
}

void missions_player_traveled_distance_in_vehicle(int playerid, int vehicleid, float distance_in_m)
{
	/*The state of the mission is not being checked. Being on a mission and in the mission
	vehicle is enough reason to add the traveled distance to the total distance*/
	if (activemission[playerid] != NULL && activemission[playerid]->vehicleid == vehicleid) {
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
			"unk model: %d", model);
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
		logprintf("mission_get_vehicle_paymp: unk model: %d", model);
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
	int i;
	int costpermsp, tax = 500, chargernws = 0;

	switch (missiontype) {
	case SETTING__MISSION_TYPE_PASSENGER_WATER:
		/* skimmer, also for runways (lights maintenance etc) */
		costpermsp = 15;
		chargernws = 1;
		break;
	case SETTING__MISSION_TYPE_PASSENGER_S:
		/*dodo is excluded from runway cost*/
		costpermsp = 20;
		break;
	case SETTING__MISSION_TYPE_PASSENGER_M:
		costpermsp = 30;
		chargernws = 1;
		break;
	case SETTING__MISSION_TYPE_PASSENGER_L:
		costpermsp = 50;
		chargernws = 1;
		break;
	case SETTING__MISSION_TYPE_CARGO_S:
	case SETTING__MISSION_TYPE_CARGO_M:
	case SETTING__MISSION_TYPE_CARGO_L:
		costpermsp = 40;
		chargernws = 1;
		break;
	case SETTING__MISSION_TYPE_HELI:
	case SETTING__MISSION_TYPE_HELI_CARGO:
		costpermsp = 30;
		break;
	case SETTING__MISSION_TYPE_MIL_HELI:
	case SETTING__MISSION_TYPE_MIL:
	default:
		/* military is govt, and no tax for special missions */
		return 0;
	}

	/*missionpoint cost for every missionpoint this missiontype has*/
	msp = ap->missionpoints;
	for (i = ap->num_missionpts; i > 0; i--) {
		if (msp->type & missiontype) {
			tax += costpermsp;
		}
		msp++;
	}

	/*for helis: 40 per helipad*/
	/*for others: 50 per runway + 15 if VOR + 15 if ILS*/
	rnw = ap->runways;
	while (rnw != ap->runwaysend) {
		if (missiontype & SETTING__HELI_MISSIONTYPES) {
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
			}
		}
		rnw++;
	}
	return tax;
}

static
struct MISSION *missions_get_mission_from_vehicleid(int vehicleid)
{
	register int pid;
	int i;

	if (vehicleid) {
		for (i = 0; i < playercount; i++) {
			pid = players[i];
			if (activemission[pid] && activemission[pid]->vehicleid == vehicleid) {
				return activemission[pid];
			}
		}
	}
	return NULL;
}

static
int missions_format_satisfaction_text(int satisfaction, char *out_buf)
{
	return sprintf(out_buf, "Passenger~n~Satisfaction: %d%%", satisfaction);
}

/*
 * The RPC is not sent to passengers whose own mission stage is MISSION_STAGE_FLIGHT.
 *
 * @param vehicleid use {@code 0} to not send it to all passengers in the vehicle.
 */
static
void missions_send_rpc_to_player_and_passengers(int rpc, struct BitStream *bs, int playerid, int vehicleid)
{
	register int pid;
	int i;

	SAMP_SendRPCToPlayer(rpc, bs, playerid, 2);
	if (vehicleid) {
		for (i = 0; i < playercount; i++) {
			pid = players[i];
			if (player[pid]->vehicleid == vehicleid &&
				pid != playerid &&
				/*Check mission stage, in case the passenger is also on their own mission
				(we don't want to show the satisfaction of the mission of the vehicle
				they're in instead of the satisfaction of their own current mission).*/
				mission_stage[pid] != MISSION_STAGE_FLIGHT)
			{
				SAMP_SendRPCToPlayer(rpc, bs, pid, 2);
			}
		}
	}
}

/**
 * Shows the passenger statisfaction textdraw for given playerid and everyone in given vehicleid.
 *
 * @param vehicleid use {@code 0} to not show it for the passengers in the vehicle.
 */
static
void missions_show_passenger_satisfaction_textdraw(int satisfaction, int playerid, int vehicleid)
{
	struct BitStream bs;

	td_satisfaction.rpcdata->text_length = missions_format_satisfaction_text(satisfaction, td_satisfaction.rpcdata->text);
#ifdef DEV
	textdraws_assert_text_length_within_bounds(&td_satisfaction);
#endif
	bs.ptrData = td_satisfaction.rpcdata;
	bs.numberOfBitsUsed = (sizeof(struct RPCDATA_ShowTextDraw) - 1 + td_satisfaction.rpcdata->text_length) * 8;
	missions_send_rpc_to_player_and_passengers(RPC_ShowTextDraw, &bs, playerid, vehicleid);
}

/**
 * Updates satisfaction textdraw for all players that are in the given vehicle.
 */
static
void missions_update_passenger_satisfaction_textdraw(int satisfaction, int playerid, int vehicleid)
{
	struct RPCDATA_TextDrawSetString rpcdata;
	struct BitStream bs;

	rpcdata.textdrawid = TEXTDRAW_JOBSATISFACTION;
	rpcdata.text_length = (short) missions_format_satisfaction_text(satisfaction, rpcdata.text);
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = (2 + 2 + rpcdata.text_length) * 8;
	missions_send_rpc_to_player_and_passengers(RPC_TextDrawSetString, &bs, playerid, vehicleid);
}

/*
 * @param vehicleid use {@code 0} to not hide it for the passengers in the vehicle.
 */
static
void missions_hide_passenger_satisfaction_textdraw(int playerid, int vehicleid)
{
	struct RPCDATA_HideTextDraw rpcdata;
	struct BitStream bs;

	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;

	rpcdata.textdrawid = (short) TEXTDRAW_JOBSATISFACTION;
	missions_send_rpc_to_player_and_passengers(RPC_HideTextDraw, &bs, playerid, vehicleid);
}

/**
Cleanup a mission and free memory. Does not query. Does send a tracker msg. Resets {@code mission_stage}.

Player must be on a mission or get segfault'd.

Call when ending a mission.
*/
static
void missions_cleanup(int playerid)
{
	int was_paused_mission, missionvehicleid;
	struct MISSION *mission;

	mission = activemission[playerid];

	/* flight tracker packet 3 */
	buf32[0] = 0x03594C46;
	buf32[1] = mission->id;
	NC_ssocket_send(tracker, buf32a, 8);

	if (mission->missiontype & SETTING__PASSENGER_MISSIONTYPES) {
		missions_hide_passenger_satisfaction_textdraw(playerid, mission->vehicleid);
	}

	if (GetPlayerVehicleID(playerid) != mission->vehicleid) {
		SetVehicleObjectiveForPlayer(mission->vehicleid, playerid, 0);
	}

	if ((was_paused_mission = (mission->flags & MISSION_FLAG_WAS_PAUSED))) {
		missionvehicleid = mission->vehicleid;
		/*destroy vehicle/respawn player AFTER resetting activemission,
		or the destroy vehicle will cause another mission end and thus
		infinite loop*/

		if (paused_mission[playerid] && paused_mission[playerid]->id == mission->id) {
			paused_mission[playerid]->is_cancelled = 1;
		}
	}

	free(mission);
	activemission[playerid] = NULL;
	mission_stage[playerid] = MISSION_STAGE_NOMISSION;
	number_missions_started_stopped[playerid]++;

	if (was_paused_mission) {
		veh_DestroyVehicle(missionvehicleid);
		playerpool->virtualworld[playerid] = 0;
		if (GetPlayerState(playerid) != PLAYER_STATE_WASTED) {
			natives_SpawnPlayer(playerid);
		}
	} else {
		kneeboard_update_all(playerid, &player[playerid]->pos);
	}
}

/**
player must be on a mission or get segfault'd.

@param reason one of MISSION_STATE_ constants
*/
static
void missions_end_unfinished(int playerid, int reason)
{
	struct MISSION *mission;
	char query[1024];
	int duration;

	mission = activemission[playerid];
	mission->damagetaken += mission->lastvehiclehp - (short) GetVehicleHealth(mission->vehicleid);
	mission->fuelburned += mission->lastfuel - vehicle_fuel[mission->vehicleid];
	duration = mission->previous_duration_s + (int) difftime(time(NULL), mission->starttime);
	sprintf(query,
	        "UPDATE flg "
		"SET state=%d,tlastupdate=UNIX_TIMESTAMP(),duration=%d,adistance=%f,"
		"fuel=%f,damage=%d,satisfaction=%d,pweatherbonus=%d "
		"WHERE id=%d",
	        reason,
	        duration,
		mission->actualdistanceM,
		mission->fuelburned / vehicle_fuel_cap[mission->vehicleid],
	        mission->damagetaken,
	        mission->passenger_satisfaction,
	        mission->weatherbonus,
	        mission->id
	);
	if (reason == MISSION_STATE_PAUSED) {
		common_mysql_tquery(query, NULL, NULL);
	} else {
		common_mysql_tquery(query, cb_missions_post_to_discord_after_finish_query_done, (void*) mission->id);
	}

	missions_cleanup(playerid);
}

/**
player must be on a mission or get segfault'd.

@param reason the quit reason as reported in OnPlayerDisconnect
*/
static
void missions_end_paused(int playerid, int reason)
{
	struct SYNCDATA_Driver *syncdata;
	struct MISSION *mission;
	int gear_keys;
	float fuel;

	mission = activemission[playerid];
	syncdata = &mission->last_syncdata;
	gear_keys = syncdata->partial_keys | ((syncdata->additional_keys & 3) << 16);
	if (syncdata->landing_gear_state) {
		gear_keys |= 0x100000;
	}
	fuel = vehicle_fuel[mission->vehicleid] / vehicle_fuel_cap[mission->vehicleid];
	csprintf(buf4096,
		"INSERT INTO pfl(fid,t,x,y,z,qw,qx,qy,qz,vx,vy,vz,fuel,hp,gear_keys,udlrkeys,misc,reason)"
		"VALUES (%d,UNIX_TIMESTAMP(),%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d)",
		mission->id,
		syncdata->pos.x, syncdata->pos.y, syncdata->pos.z,
		syncdata->quat_w, syncdata->quat_x, syncdata->quat_y, syncdata->quat_z,
		syncdata->vel.x, syncdata->vel.y, syncdata->vel.z,
		fuel,
		syncdata->vehicle_hp,
		gear_keys,
		syncdata->udkey | (syncdata->lrkey << 16),
		syncdata->misc,
		reason
	);
	NC_mysql_tquery_nocb(buf4096a);

	missions_end_unfinished(playerid, MISSION_STATE_PAUSED);
}

void missions_on_vehicle_destroyed_or_respawned(int vehicleid)
{
	struct MISSION *mission;
	int i, playerid;

	for (i = 0; i < playercount; i++) {
		playerid = players[i];
		mission = activemission[playerid];
		if (mission && mission->vehicleid == vehicleid) {
			SendClientMessage(playerid, COL_WARN,
				WARN" Your mission vehicle has been destroyed, "
				"your mission has been aborted and you have "
				"been fined $"SETTING__MISSION_CANCEL_FEE_STR".");
			NC_DisablePlayerRaceCheckpoint(playerid);
			money_take(playerid, SETTING__MISSION_CANCEL_FEE_INT);
			missions_end_unfinished(playerid, MISSION_STATE_ABANDONED_VEHICLE_DESTROYED);
			return;
		}
	}
}

void missions_on_player_connect(int playerid)
{
	register char *indicator_states;
	int i;

	indicator_states = msp_indicators[playerid].state;
	for (i = 0; i < MAX_MISSION_INDICATORS; i++) {
		indicator_states[i] = MISSIONPOINT_INDICATOR_STATE_FREE;
	}
	missions_available_msptype_mask[playerid] = -1;
	active_msp[playerid] = NULL;
	locating_msp[playerid] = NULL;
	mission_stage[playerid] = MISSION_STAGE_NOMISSION;
	activemission[playerid] = NULL;
	paused_mission[playerid] = NULL;
	number_missions_started_stopped[playerid]++;
}

static
void missions_on_player_disconnect(int playerid, int reason)
{
	struct MISSION *miss;

	if (mission_stage[playerid] == MISSION_STAGE_FLIGHT && (miss = activemission[playerid])) {
		if (miss->flags & MISSION_FLAG_IS_PAUSED_SET_TO_IN_PROGRESS_ON_NEXT_DRIVERSYNC) {
			/*mission was already paused but player disconnected before they got control after resuming*/
			/*nothing to be done except cleanup, player will continue using the pause data from last time*/
			missions_cleanup(playerid);
		} else if (miss->flags & MISSION_FLAG_HAS_LAST_SYCDATA && loggedstatus[playerid] == LOGGED_IN) {
			missions_end_paused(playerid, reason);
		} else {
			switch (reason) {
			case 0: missions_end_unfinished(playerid, MISSION_STATE_ABANDONED_TIMEOUT); break;
			default: missions_end_unfinished(playerid, MISSION_STATE_ABANDONED_QUIT); break;
			case 2: missions_end_unfinished(playerid, MISSION_STATE_ABANDONED_KICKED); break;
			}
		}
	}
	if (paused_mission[playerid]) {
		free(paused_mission[playerid]);
		paused_mission[playerid] = NULL;
	}
}

struct MISSION_CB_DATA {
	int player_cc;
	int number_missions_started_stopped;
	int query_time;
};

/**
Starts the FLIGHT stage of the mission.

May be called from a timer.
*/
static
int missions_start_flight(void *data)
{
	struct MISSION_CB_DATA *mission_cb_data;
	int playerid;
	struct MISSION *mission;
	struct vec3 pos;

	mission_cb_data = (struct MISSION_CB_DATA*) data;
	playerid = PLAYER_CC_GETID(mission_cb_data->player_cc);
	if (!PLAYER_CC_CHECK(mission_cb_data->player_cc, playerid) ||
		mission_cb_data->number_missions_started_stopped != number_missions_started_stopped[playerid])
	{
		free(mission_cb_data);
		return TIMER_NO_REPEAT;
	}

	mission = activemission[playerid];

	if (prefs[playerid] & PREF_WORK_AUTONAV) {
		nav_navigate_to_airport(playerid, mission->vehicleid, mission->vehmodel, mission->endpoint->ap);
	}

	if (mission->missiontype & SETTING__PASSENGER_MISSIONTYPES) {
		missions_show_passenger_satisfaction_textdraw(mission->passenger_satisfaction, playerid, mission->vehicleid);
	}

	mission_stage[playerid] = MISSION_STAGE_FLIGHT;
	SetPlayerRaceCheckpointNoDir(playerid, RACE_CP_TYPE_NORMAL, &mission->endpoint->pos, MISSION_CP_RAD);
	HideGameTextForPlayer(playerid);
	TogglePlayerControllable(playerid, 1);
	GetPlayerPos(playerid, &pos);
	kneeboard_update_all(playerid, &pos);

	csprintf(buf144,
	        "UPDATE flg "
		"SET tload=UNIX_TIMESTAMP(),tlastupdate=UNIX_TIMESTAMP() "
		"WHERE id=%d",
	        mission->id);
	NC_mysql_tquery_nocb(buf144a);

	free(mission_cb_data);
	return TIMER_NO_REPEAT;
}

/**
Called when mission create query has been executed.
*/
static
void missions_querycb_create(void *data)
{
	struct MISSION_CB_DATA *mission_cb_data;
	int playerid;
	struct MISSION *mission;
	int loading_time_left;

	mission_cb_data = (struct MISSION_CB_DATA*) data;
	playerid = PLAYER_CC_GETID(mission_cb_data->player_cc);
	if (!PLAYER_CC_CHECK(mission_cb_data->player_cc, playerid) ||
		mission_cb_data->number_missions_started_stopped != number_missions_started_stopped[playerid])
	{
		free(mission_cb_data);
		return;
	}

	mission = activemission[playerid];
	mission->id = NC_cache_insert_id();

	/* flight tracker packet 1 */
	buf32[0] = 0x01594C46;
	buf32[1] = mission->id;
	buf32[2] = TRACKER_HDR_FLAG_CANARY | TRACKER_HDR_FLAG_HAS_BANK_PITCH | TRACKER_HDR_FLAG_HAS_ALTITUDE; /*flags (actually a short)*/
	NC_ssocket_send(tracker, buf32a, 10);

	loading_time_left = mission_cb_data->query_time + MISSION_LOAD_UNLOAD_TIME - time_timestamp();
	if (loading_time_left > 100) {
		timer_set(loading_time_left, missions_start_flight, mission_cb_data);
	} else {
		missions_start_flight(mission_cb_data);
	}
	/*The missions_start_flight func will free the passed mission_cb_data.*/
}

/**
Starts a mission for given player.
*/
static
void missions_start_mission(int playerid, struct MISSIONPOINT *startpoint, struct MISSIONPOINT *endpoint, int missiontype)
{
	struct MISSION_CB_DATA *cbdata;
	struct MISSION *mission;
	struct dbvehicle *veh;
	struct vec3 player_position;
	int vehicleid, i;
	float vhp, dx, dy;

	if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		SendClientMessage(playerid, COL_WARN, WARN"You must be the driver of a vehicle ");
		return;
	}

	if (!(CLASS_MSPTYPES[classid[playerid]] & missiontype)) {
		/*This should never happen, since this should only be called from the missionmap,
		and players shouldn't be able to reclass while the missionmap is opened.*/
#if DEV
		assert(((void) "wrong class", 0));
#endif
		SendClientMessage(playerid, COL_WARN, WARN"Wrong class! (/reclass)");
		return;
	}

	vehicleid = GetPlayerVehicleID(playerid);
	veh = gamevehicles[vehicleid].dbvehicle;
	if (!veh || !(vehicle_msptypes[veh->model] & missiontype)) {
		SendClientMessage(playerid, COL_WARN, WARN"Wrong vehicle type!");
		return;
	}

	for (i = 0; i < playercount; i++) {
		if ((mission = activemission[players[i]]) != NULL && mission->vehicleid == vehicleid) {
			SendClientMessage(playerid, COL_WARN, WARN"This vehicle is already used in a mission!");
			return;
		}
	}

	GetPlayerPos(playerid, &player_position);
	dx = player_position.x - startpoint->pos.x;
	dy = player_position.y - startpoint->pos.y;
	if (dx * dx + dy * dy > MISSION_CP_RAD_SQ) {
		SendClientMessage(playerid, COL_WARN, WARN"Get inside the mission point first!");
		return;
	}

	vhp = GetVehicleHealth(vehicleid);

	dx = startpoint->pos.x - endpoint->pos.x;
	dy = startpoint->pos.y - endpoint->pos.y;

	mission_stage[playerid] = MISSION_STAGE_LOAD;
	activemission[playerid] = mission = malloc(sizeof(struct MISSION));
	number_missions_started_stopped[playerid]++;
	mission->id = -1;
	mission->missiontype = missiontype;
	mission->startpoint = startpoint;
	mission->endpoint = endpoint;
	mission->distance = sqrt(dx * dx + dy * dy);
	mission->actualdistanceM = 0.0f;
	mission->previous_duration_s = 0;
	mission->passenger_satisfaction = 100;
	mission->vehmodel = veh->model;
	mission->vehicleid = vehicleid;
	mission->starttime = time(NULL);
	mission->lastvehiclehp = (short) vhp;
	mission->damagetaken = 0;
	mission->lastfuel = vehicle_fuel[vehicleid];
	mission->fuelburned = 0.0f;
	mission->weatherbonus = missions_get_initial_weatherbonus();
	mission->flags = 0;

	csprintf(buf144, "UPDATE msp SET o=o+1 WHERE i=%d", startpoint->id);
	NC_mysql_tquery_nocb(buf144a);
	csprintf(buf144, "UPDATE msp SET p=p+1 WHERE i=%d", endpoint->id);
	NC_mysql_tquery_nocb(buf144a);

	sprintf(cbuf4096_,
	        "INSERT INTO flg(player,vehicle,missiontype,fapt,tapt,fmsp,"
		"tmsp,distance,tstart,tlastupdate) "
		"VALUES(IF(%d<1,NULL,%d),%d,%d,%d,%d,%d,%d,%.4f,"
		"UNIX_TIMESTAMP(),UNIX_TIMESTAMP())",
	        userid[playerid],
	        userid[playerid],
		veh->id,
		missiontype,
		startpoint->ap->id,
		endpoint->ap->id,
		startpoint->id,
		endpoint->id,
		mission->distance);
	cbdata = malloc(sizeof(struct MISSION_CB_DATA));
	cbdata->player_cc = MK_PLAYER_CC(playerid);
	cbdata->number_missions_started_stopped = number_missions_started_stopped[playerid];
	cbdata->query_time = time_timestamp();
	common_mysql_tquery(cbuf4096_, missions_querycb_create, cbdata);

	tracker_afk_packet_sent[playerid] = 0;

	sprintf(cbuf144,
		"Flight from %s (%s) %s to %s (%s) %s",
		mission->startpoint->ap->name,
		mission->startpoint->ap->code,
		mission->startpoint->name,
		mission->endpoint->ap->name,
		mission->endpoint->ap->code,
		mission->endpoint->name);
	SendClientMessage(playerid, COL_MISSION_ALT, cbuf144);

	TogglePlayerControllable(playerid, 0);
	GameTextForPlayer(playerid, 0x800000, 3, "~p~Loading");
	kneeboard_update_all(playerid, &player_position);
	locating_msp[playerid] = NULL;
}

struct MISSION_UNLOAD_DATA {
	int player_cc;
	int number_missions_started_stopped;
	float vehiclehp;
};

/**
Called from timer callback for mission unload checkpoint.
*/
static
int missions_after_unload(void *data)
{
	struct DIALOG_INFO dialog;
	struct MISSION_UNLOAD_DATA *mission_cb_data;
	int playerid;
	struct MISSION *miss;
	float vehhp, paymp, mintime;
	int ptax, psatisfaction, pdistance, pbonus = 0, ptotal, pdamage, pcheat;
	int duration_total_s, duration_h, duration_m, extra_damage_taken;
	int i;
	char *dialoginfo;
	short missionmsg_playerids[MAX_PLAYERS];
	int num_missionmsg_playerids;

	mission_cb_data = (struct MISSION_UNLOAD_DATA*) data;
	playerid = PLAYER_CC_GETID(mission_cb_data->player_cc);
	if (!PLAYER_CC_CHECK(mission_cb_data->player_cc, playerid) ||
		mission_cb_data->number_missions_started_stopped != number_missions_started_stopped[playerid])
	{
		free(mission_cb_data);
		return TIMER_NO_REPEAT;
	}

	vehhp = mission_cb_data->vehiclehp;
	miss = activemission[playerid];

	HideGameTextForPlayer(playerid);
	TogglePlayerControllable(playerid, 1);

	pcheat = 0;
	psatisfaction = 0;

	extra_damage_taken = (int) (miss->lastvehiclehp - (short) vehhp);
	if (extra_damage_taken < 0) {
		pcheat -= 250000;
		sprintf(cbuf144,
			"flg(#%d) vhh: was: %hd now: %.0f",
			miss->id,
			miss->lastvehiclehp,
			vehhp);
		anticheat_log(playerid, AC_VEH_HP_MISSION_INCREASE, cbuf144);
	} else {
		miss->damagetaken += extra_damage_taken;
	}
	miss->lastvehiclehp = (short) vehhp;
	miss->fuelburned += miss->lastfuel - vehicle_fuel[miss->vehicleid];

	duration_total_s = miss->previous_duration_s + (int) difftime(time(NULL), miss->starttime);
	duration_m = duration_total_s % 60;
	duration_h = (duration_total_s - duration_m) / 60;

	/* don't use adistance because it also includes z changes */
	mintime = miss->distance;
	mintime /= missions_get_vehicle_maximum_speed(miss->vehmodel);
	if (duration_total_s < (int) mintime) {
		pcheat -= 250000;
		sprintf(cbuf144,
			"flg(#%d) too fast: min: %d actual: %d",
			miss->id,
			(int) mintime,
			duration_total_s);
		anticheat_log(playerid, AC_MISSION_TOOFAST, cbuf144);
	}

	paymp = missions_get_vehicle_paymp(miss->vehmodel);
	ptax = -calculate_airport_tax(miss->endpoint->ap, miss->missiontype);
	pdistance = 500 + (int) (miss->distance * 1.935f);
	pdistance = (int) (pdistance * paymp);
	if (miss->missiontype & SETTING__PASSENGER_MISSIONTYPES) {
		if (miss->passenger_satisfaction == 100) {
			psatisfaction = 500;
		} else {
			psatisfaction = miss->passenger_satisfaction - 100;
			psatisfaction *= 30;
		}
	}
	pdamage = -3 * miss->damagetaken;

	ptotal = miss->weatherbonus + psatisfaction + pdistance + pbonus;
	ptotal += ptax + pdamage + pcheat;
	money_give(playerid, ptotal);

	i = sprintf(cbuf4096,
		"%s completed flight #%d from %s (%s) to %s (%s) in %dh%02dm (%s)",
		pdata[playerid]->name,
	        miss->id,
		miss->startpoint->ap->name,
	        miss->startpoint->ap->code,
	        miss->endpoint->ap->name,
	        miss->endpoint->ap->code,
	        duration_h,
	        duration_m,
	        vehnames[miss->vehmodel - VEHICLE_MODEL_MIN]);
	echo_send_generic_message(ECHO_PACK12_FLIGHT_MESSAGE, cbuf4096);
	num_missionmsg_playerids = 0;
	for (i = 0; i < playercount; i++) {
		if (prefs[players[i]] & PREF_SHOW_MISSION_MSGS) {
			missionmsg_playerids[num_missionmsg_playerids] = players[i];
			num_missionmsg_playerids++;
		}
	}
	SendClientMessageToBatch(missionmsg_playerids, num_missionmsg_playerids, COL_MISSION, cbuf4096);

	sprintf(cbuf4096_,
		"UPDATE flg SET tunload=UNIX_TIMESTAMP(),"
		"tlastupdate=UNIX_TIMESTAMP(),duration=%d,"
		"state=%d,fuel=%f,ptax=%d,pweatherbonus=%d,psatisfaction=%d,"
		"pdistance=%d,pdamage=%d,pcheat=%d,pbonus=%d,ptotal=%d,"
		"satisfaction=%d,adistance=%f,paymp=%f,damage=%d "
		"WHERE id=%d",
		duration_total_s,
		MISSION_STATE_FINISHED,
		miss->fuelburned / vehicle_fuel_cap[miss->vehicleid],
		ptax,
		miss->weatherbonus,
		psatisfaction,
		pdistance,
		pdamage,
		pcheat,
		pbonus,
		ptotal,
		miss->passenger_satisfaction,
		miss->actualdistanceM,
		paymp,
		miss->damagetaken,
		miss->id);
	common_mysql_tquery(cbuf4096_, cb_missions_post_to_discord_after_finish_query_done, (void*) miss->id);

	dialog_init_info(&dialog);
	dialoginfo = dialog.info;
	dialoginfo += sprintf(dialoginfo,
	             "{ffffff}Flight:\t\t\t"ECOL_MISSION"#%d\n"
	             "{ffffff}Origin:\t\t\t"ECOL_MISSION"%s\n"
	             "{ffffff}Destination:\t\t"ECOL_MISSION"%s\n"
	             "{ffffff}Distance (pt to pt):\t"ECOL_MISSION"%.0fm\n"
	             "{ffffff}Distance (actual):\t"ECOL_MISSION"%.0fm\n"
	             "{ffffff}Duration:\t\t"ECOL_MISSION"%dh%02dm\n"
	             "{ffffff}Fuel Burned:\t\t"ECOL_MISSION"%.1fL\n"
	             "{ffffff}Vehicle pay category:\t"ECOL_MISSION"%.1fx\n",
	             miss->id,
	             miss->startpoint->ap->name,
	             miss->endpoint->ap->name,
	             miss->distance,
	             miss->actualdistanceM,
	             duration_h,
	             duration_m,
		     miss->fuelburned,
	             paymp);
	if (miss->missiontype & SETTING__PASSENGER_MISSIONTYPES) {
		dialoginfo += sprintf(dialoginfo,
		             "{ffffff}Passenger Satisfaction:\t"
			     ""ECOL_MISSION"%d%%\n",
		             miss->passenger_satisfaction);
	}
	*dialoginfo++ = '\n';
	*dialoginfo++ = '\n';
	if (ptax) {
		dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Airport Tax:\t\t", ptax);
	}
	if (miss->weatherbonus) {
		dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Weather bonus:\t\t", miss->weatherbonus);
	}
	dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Distance Pay:\t\t", pdistance);
	if (miss->missiontype & SETTING__PASSENGER_MISSIONTYPES) {
		if (psatisfaction > 0) {
			dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Satisfaction Bonus:\t", psatisfaction);
		} else {
			dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Satisfaction Penalty:\t", psatisfaction);
		}
	}
	if (pdamage) {
		dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Damage Penalty:\t", pdamage);
	}
	if (pcheat) {
		dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Cheat Penalty:\t\t", pcheat);
	}
	if (pbonus) {
		dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Bonus:\t\t\t", pbonus);
	}
	dialoginfo += missions_append_pay(dialoginfo, "\n\n\t{ffffff}Total Pay: ", ptotal);
	*--dialoginfo = 0;
	dialog.transactionid = DLG_TID_MISSION_OVERVIEW;
	dialog.caption = "Flight Overview";
	dialog.button1 = "Close";
	dialog_show(playerid, &dialog);

	missions_cleanup(playerid);

	free(mission_cb_data);
	return TIMER_NO_REPEAT;
}

static
void missions_start_unload(int playerid)
{
	struct MISSION_UNLOAD_DATA *cbdata;
	struct MISSION *mission;
	struct vec3 vvel;
	int vehicleid;

	if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		return;
	}

	mission = activemission[playerid];

	if (mission == NULL) {
		return;
	}

	vehicleid = GetPlayerVehicleID(playerid);
	if (vehicleid != mission->vehicleid) {
		SendClientMessage(playerid, COL_WARN, WARN"You must be in the starting vehicle to continue!");
		return;
	}

	GetVehicleVelocityUnsafe(vehicleid, &vvel);
	/*Ignoring z velocity because it would be annoying with skimmers riding heavy waves.*/
	if (vvel.x * vvel.x + vvel.y * vvel.y > MAX_SPEED_SQ) {
		SendClientMessage(playerid, COL_WARN, WARN"Stop the vehicle first!");
		return;
	}

	mission_stage[playerid] = MISSION_STAGE_UNLOAD;
	cbdata = malloc(sizeof(struct MISSION_UNLOAD_DATA));
	cbdata->number_missions_started_stopped = number_missions_started_stopped[playerid];
	cbdata->player_cc = MK_PLAYER_CC(playerid);
	cbdata->vehiclehp = GetVehicleHealth(vehicleid);

	textdraws_hide_consecutive(playerid, 1, TEXTDRAW_JOBSATISFACTION);
	GameTextForPlayer(playerid, 0x8000000, 3, "~p~Unloading");
	TogglePlayerControllable(playerid, 0); /*Needs to be after getting vehicle health, it repairs the vehicle.*/
	DisablePlayerRaceCheckpoint(playerid);
	if (prefs[playerid] & PREF_WORK_AUTONAV) {
		nav_disable(vehicleid);
	}

	timer_set(MISSION_LOAD_UNLOAD_TIME, missions_after_unload, cbdata);
}

static
void missions_update_available_msptypes(int playerid, int vehicleid)
{
	register unsigned int mask;

	missions_available_msptype_mask[playerid] = CLASS_MSPTYPES[classid[playerid]];
	if (vehicleid) {
		mask = vehicle_msptypes[GetVehicleModel(vehicleid)];
		missions_player_vehicle_msptype_mask[playerid] = mask;
		missions_available_msptype_mask[playerid] &= mask;
	} else {
		missions_player_vehicle_msptype_mask[playerid] = 0;
	}
}

/*TODO: same thing but for passenger state?*/
static
void missions_on_driver_changed_vehicle_without_state_change(int playerid, int newvehicleid)
{
	struct vec3 pos;

	missions_update_available_msptypes(playerid, newvehicleid);
	GetPlayerPos(playerid, &pos);
	missions_update_missionpoint_indicators(playerid, pos.x, pos.y, pos.z);
}

static
void missions_stoplocate(int playerid)
{
	if (locating_msp[playerid]) {
		locating_msp[playerid] = NULL;
		DisablePlayerRaceCheckpoint(playerid);
	}
}

static
void missions_on_player_state_change(int playerid, int from, int to)
{
	struct MISSION *mission;
	struct vec3 pos;
	int vehicleid;

	if (to == PLAYER_STATE_WASTED) {
		/*This basically handles on_player_death.*/
		switch (mission_stage[playerid]) {
		case MISSION_STAGE_NOMISSION:
			break;
		case MISSION_STAGE_JOBMAP:
			missions_hide_jobmap_set_stage_set_controllable(playerid);
			break;
		default:
			DisablePlayerRaceCheckpoint(playerid);
			vehicleid = GetPlayerVehicleID(playerid);
			if (vehicleid && GetVehicleHealth(vehicleid) < 250.0f) {
				missions_end_unfinished(playerid, MISSION_STATE_CRASHED);
			} else {
				missions_end_unfinished(playerid, MISSION_STATE_DIED);
			}
			break;
		}

		missions_stoplocate(playerid);
		return;
	}

	if (to == PLAYER_STATE_ONFOOT || from == PLAYER_STATE_ONFOOT) {
		vehicleid = GetPlayerVehicleID(playerid);
		if (vehicleid) {
			/*If the player entering as passenger is on a flight themselves,
			we don't want to override the displayed satisfaction value with the
			value of the mission of the vehicleid they're entering as passenger.
			(Currently not checking if the player is on a flight without satisfaction.)*/
			if (mission_stage[playerid] != MISSION_STAGE_FLIGHT) {
				mission = missions_get_mission_from_vehicleid(vehicleid);
				if (mission) {
					missions_show_passenger_satisfaction_textdraw(mission->passenger_satisfaction, playerid, 0);
				}
			}
		} else {
			/*Don't hide satisfaction text for the pilot or a passenger
			if the passenger is on a flight themselves.*/
			if (mission_stage[playerid] != MISSION_STAGE_FLIGHT) {
				missions_hide_passenger_satisfaction_textdraw(playerid, 0);
			}
		}

		/*This could be unnecessary, unless the player gets jacked or teleported.
		(They can't exit because it'd trigger the keystate change to hide the ui)*/
		if (mission_stage[playerid] == MISSION_STAGE_JOBMAP) {
			missions_hide_jobmap_set_stage_set_controllable(playerid);
		}
	}

	if (from == PLAYER_STATE_DRIVER || to == PLAYER_STATE_DRIVER ||
		from == PLAYER_STATE_PASSENGER || to == PLAYER_STATE_PASSENGER)
	{
		vehicleid = GetPlayerVehicleID(playerid);
		missions_update_available_msptypes(playerid, vehicleid);
		GetPlayerPos(playerid, &pos);
		missions_update_missionpoint_indicators(playerid, pos.x, pos.y, pos.z);
	}

	if (from == PLAYER_STATE_DRIVER &&
		(mission = activemission[playerid]) != NULL &&
		mission->vehicleid == lastvehicle_asdriver[playerid])
	{
		SetVehicleObjectiveForPlayer(lastvehicle_asdriver[playerid], playerid, 1);
		SendClientMessage(playerid, COL_WARN, WARN"Get back in your vehicle!");
	} else if (to == PLAYER_STATE_DRIVER &&
		(mission = activemission[playerid]) != NULL &&
		mission->vehicleid == GetPlayerVehicleID(playerid))
	{
		SetVehicleObjectiveForPlayer(mission->vehicleid, playerid, 0);
	}
}

void missions_on_vehicle_stream_in(int vehicleid, int forplayerid)
{
	struct MISSION *mission;

	mission = activemission[forplayerid];
	if (mission != NULL && mission->vehicleid == vehicleid) {
		SetVehicleObjectiveForPlayer(vehicleid, forplayerid, 1);
	}
}

void missions_on_weather_changed(int weather)
{
	register struct MISSION *mission;
	register int i;
	int bonusvalue;

	bonusvalue = missions_get_weatherbonus_for_weather(weather);
	i = playercount;
	while (i--) {
		mission = activemission[players[i]];
		if (mission && mission->weatherbonus < bonusvalue) {
			mission->weatherbonus = bonusvalue;
		}
	}
}

void missions_send_tracker_data(
	int playerid, int vehicleid, float hp,
	struct vec3 *vpos, struct vec3 *vvel, int engine, float pitch, float roll)
{
	struct MISSION *mission;
	unsigned char flags;
	short spd, alt, hpv, pitch10, roll10;
	float fuel;

	if ((mission = activemission[playerid]) == NULL ||
		mission->vehicleid != vehicleid ||
		(isafk[playerid] && tracker_afk_packet_sent[playerid]))
	{
		return;
	}

	tracker_afk_packet_sent[playerid] = flags = isafk[playerid] == 1;
	if (engine) {
		flags |= 2;
	}

	if (mission->flags & MISSION_FLAG_NEEDS_TRACKER_RESUME_FLAG) {
		flags |= 0x4;
		mission->flags &= ~MISSION_FLAG_NEEDS_TRACKER_RESUME_FLAG;
	}

	hpv = (short) hp;
	alt = (short) vpos->z;
	spd = (short) (VEL_TO_KTS * sqrt(vvel->x * vvel->x + vvel->y * vvel->y + vvel->z * vvel->z));
	pitch10 = (short) (pitch * 10.0f);
	roll10 = (short) (roll * 10.0f);
	fuel = vehicle_fuel[mission->vehicleid] / vehicle_fuel_cap[mission->vehicleid];

	/* flight tracker packet 2 */
	buf32[0] = 0x02594C46;
	buf32[1] = mission->id;
	cbuf32[8] = flags;
	cbuf32[9] = (char) mission->passenger_satisfaction;
	memcpy(cbuf32 + 10, &spd, 2);
	memcpy(cbuf32 + 12, &alt, 2);
	memcpy(cbuf32 + 14, &hpv, 2);
	memcpy(cbuf32 + 16, &fuel, 4);
	memcpy(cbuf32 + 20, &vpos->x, 4);
	memcpy(cbuf32 + 24, &vpos->y, 4);
	memcpy(cbuf32 + 28, &pitch10, 2);
	memcpy(cbuf32 + 30, &roll10, 2);
	NC_ssocket_send(tracker, buf32a, 32);
}

/**
To be called from the /s command handler.
*/
static
void missions_process_cancel_request_by_player(int playerid)
{
	struct MISSION *mission;

	if ((mission = activemission[playerid]) != NULL) {
		if (mission_stage[playerid] == MISSION_STAGE_LOAD || mission_stage[playerid] == MISSION_STAGE_UNLOAD) {
			NC_TogglePlayerControllable(playerid, 1);
			HideGameTextForPlayer(playerid);
		} else {
			NC_DisablePlayerRaceCheckpoint(playerid);
		}
		money_take(playerid, SETTING__MISSION_CANCEL_FEE_INT);
		missions_end_unfinished(playerid, MISSION_STATE_ABORTED);
	} else {
		SendClientMessage(playerid, COL_WARN, WARN"You're not on an active mission.");
	}
}

static
void missions_locate_closest_mission(int playerid)
{
	struct MISSIONPOINT *closest;
	struct dbvehicle *veh;
	struct vec3 pos;
	int mspindex;
	float dx, dy;
	float closest_distance_sq, distance_sq;
	int vehicleid;
	int msptypemask;

	GetPlayerPos(playerid, &pos);
	closest = NULL;
	closest_distance_sq = float_pinf;
	for (mspindex = 0; mspindex < nummissionpoints; mspindex++) {
		msptypemask = missionpoints[mspindex].type;
		if (msptypemask & missions_available_msptype_mask[playerid]) {
			dx = missionpoints[mspindex].pos.x - pos.x;
			dy = missionpoints[mspindex].pos.y - pos.y;
			distance_sq = dx * dx + dy * dy;
			if (distance_sq < closest_distance_sq) {
				closest_distance_sq = distance_sq;
				closest = &missionpoints[mspindex];
			}
		}
	}

	if (!closest) {
		SendClientMessage(playerid, COL_WARN, WARN"No mission points available for your class and vehicle combination.");
	} else {
		locating_msp[playerid] = closest;
		SetPlayerRaceCheckpointNoDir(playerid, RACE_CP_TYPE_NORMAL, &closest->pos, MISSION_CP_RAD);
		if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
			vehicleid = GetPlayerVehicleID(playerid);
			veh = gamevehicles[vehicleid].dbvehicle;
			if (veh) {
				nav_navigate_to_airport(playerid, vehicleid, veh->model, closest->ap);
			}
		}
		/*Message should have same color as the message in nav_navigate_to_airport.*/
		SendClientMessage(playerid, COL_SAMP_GREY, "Checkpoint set to closest mission point. Use /stoplocate to disable it.");
	}
}

/**
To be called from the /w command handler.
*/
static
void missions_locate_or_show_map(int playerid)
{
	struct dbvehicle *veh;
	int vehicleid;

	switch (mission_stage[playerid]) {
	case MISSION_STAGE_NOMISSION:
		if (active_msp[playerid]) {
			missions_show_jobmap_set_stage_set_controllable(playerid);
			break;
		}

		vehicleid = GetPlayerVehicleID(playerid);
		veh = gamevehicles[vehicleid].dbvehicle;
		if (!veh || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
			SendClientMessage(playerid, COL_WARN, WARN"Get in a vehicle first!");
			break;
		}

		if (vehicle_msptypes[veh->model] == 0) {
			SendClientMessage(playerid, COL_WARN, WARN"This vehicle can't do any missions!");
			break;
		}

		missions_locate_closest_mission(playerid);
		break;
	case MISSION_STAGE_FLIGHT:
		if (active_msp[playerid] == activemission[playerid]->endpoint) {
			missions_start_unload(playerid);
			break;
		}
		SetPlayerRaceCheckpointNoDir(playerid, RACE_CP_TYPE_NORMAL, &activemission[playerid]->endpoint->pos, MISSION_CP_RAD);
		SendClientMessage(playerid, COL_WARN,
			WARN"You're already working! Use /s to stop your current work first ($"SETTING__MISSION_CANCEL_FEE_STR" fee).");
		break;
	}
}

static
void missions_driversync_udkeystate_change(int playerid, short udkey)
{
	if (mission_stage[playerid] == MISSION_STAGE_JOBMAP) {
		if (udkey) {
			jobmap_move_updown(playerid, udkey);
		}
	}
}

static
void missions_driversync_keystate_change(int playerid, int oldkeys, int newkeys)
{
	register struct MISSIONPOINT *tomsp;
	struct MISSIONPOINT *frommsp;
	struct AIRPORT *selected_airport;
	unsigned int mission_type;

	if (mission_stage[playerid] == MISSION_STAGE_JOBMAP) {
		/*Since player is set to not be controllable, these use on-foot controls even though the player is in-vehicle.*/
		if (KEY_JUST_DOWN(KEY_VEHICLE_ENTER_EXIT)) {
			PlayerPlaySound(playerid, MISSION_JOBMAP_CANCEL_SOUND);
			missions_hide_jobmap_set_stage_set_controllable(playerid); /*sets controllable and mission state*/
		} else if (KEY_JUST_DOWN(KEY_SPRINT)) {
			PlayerPlaySound(playerid, MISSION_JOBMAP_ACCEPT_SOUND);
			selected_airport = jobmap_do_accept_button(playerid);
			if (selected_airport && active_msp[playerid]) {
				missions_hide_jobmap_set_stage_set_controllable(playerid);
				frommsp = active_msp[playerid];
				mission_type = missions_available_msptype_mask[playerid] & frommsp->type;
				tomsp = missions_get_random_msp(selected_airport, mission_type);
				if (tomsp) {
					missions_start_mission(playerid, frommsp, tomsp, mission_type);
				} else {
					logprintf("failed to get random msp from airport id %d on jobmap (mission type %08x)",
						selected_airport->id, mission_type);
					SendClientMessage(playerid, COL_WARN, WARN"Something went wrong, try again");
				}
			}
		}
	} else if (KEY_JUST_DOWN(KEY_YES) && active_msp[playerid]) {
		if (mission_stage[playerid] == MISSION_STAGE_NOMISSION) {
			missions_show_jobmap_set_stage_set_controllable(playerid);
		} else if (mission_stage[playerid] == MISSION_STAGE_FLIGHT && active_msp[playerid] == activemission[playerid]->endpoint) {
			missions_start_unload(playerid);
		}
	}
}

void missions_on_vehicle_refueled(int vehicleid, float refuelamount)
{
	struct MISSION *miss;
	int i;

	for (i = 0; i < playercount; i++) {
		if ((miss = activemission[players[i]]) != NULL &&
			miss->vehicleid == vehicleid)
		{
			miss->fuelburned += refuelamount;
			miss->lastfuel = vehicle_fuel[vehicleid];
			return;
		}
	}
}

void missions_on_vehicle_repaired(int vehicleid, float fixamount, float newhp)
{
	struct MISSION *miss;
	int i;

	for (i = 0; i < playercount; i++) {
		if ((miss = activemission[players[i]]) != NULL &&
			miss->vehicleid == vehicleid)
		{
			miss->damagetaken += (short) fixamount;
			miss->lastvehiclehp = (short) newhp;
			return;
		}
	}
}

static
void missions_on_driversync(int playerid, struct SYNCDATA_Driver *syncdata)
{
	register struct MISSION *mission;

	if (mission_stage[playerid] == MISSION_STAGE_FLIGHT) {
		mission = activemission[playerid];
		if (mission && mission->vehicleid == syncdata->vehicle_id) {
			mission->flags |= MISSION_FLAG_HAS_LAST_SYCDATA;
			mission->last_syncdata = *syncdata;
			if (mission->flags & MISSION_FLAG_IS_PAUSED_SET_TO_IN_PROGRESS_ON_NEXT_DRIVERSYNC) {
				mission->flags &= ~MISSION_FLAG_IS_PAUSED_SET_TO_IN_PROGRESS_ON_NEXT_DRIVERSYNC;
				csprintf(buf4096, "UPDATE flg SET state=%d WHERE id=%d", MISSION_STATE_INPROGRESS, mission->id);
				NC_mysql_tquery_nocb(buf4096a);
			}
		}
	}
}

/**
 * Sends PlayerJoin+PlayerCreate RPCs to given player.
 * This fake player will be used to set the correct aircraft position/rotation/speed
 * before the player can take over and resume the mission.
 */
static
void missions_resume_mission_create_npc(int forplayerid)
{
	struct BitStream bitstream;
	struct RPCDATA_PlayerCreate playercreate;
	struct RPCDATA_PlayerJoin playerjoin;

	playerjoin.playerid = FAKE_PLAYER_ID_RESUME_MISSION;
	playerjoin.unkAlwaysOne = 1;
	playerjoin.npc = 1;
	playerjoin.namelen = 1;
	playerjoin.name[0] = 'x';
	bitstream.ptrData = &playerjoin;
	bitstream.numberOfBitsUsed = sizeof(playerjoin) * 8;
	SAMP_SendRPCToPlayer(RPC_PlayerJoin, &bitstream, forplayerid, 2);

	memset(&playercreate, 0, sizeof(playercreate));
	playercreate.playerid = FAKE_PLAYER_ID_RESUME_MISSION;
	bitstream.ptrData = &playercreate;
	bitstream.numberOfBitsUsed = sizeof(playercreate) * 8;
	SAMP_SendRPCToPlayer(RPC_PlayerCreate, &bitstream, forplayerid, 2);
}

/**
 * Sends PlayerLeave RPC to given player.
 */
static
void missions_resume_mission_destroy_npc(int forplayerid)
{
	struct BitStream bitstream;
	struct RPCDATA_PlayerLeave playerleave;

	playerleave.playerid = FAKE_PLAYER_ID_RESUME_MISSION;
	playerleave.reason = 1; /*quit*/
	bitstream.ptrData = &playerleave;
	bitstream.numberOfBitsUsed = sizeof(playerleave) * 8;
	SAMP_SendRPCToPlayer(RPC_PlayerLeave, &bitstream, forplayerid, 2);
}

#define MISSION_RESUME_SYNCDATA_TIMEOUT 50
struct MISSION_RESUME_DATA {
	int playerid;
	int player_cc;
	int number_missions_started_stopped;
	int ms_left;
	char send_initial : 1;
	char send_onfoot : 1;
	char done : 1;
	char dialog_closed : 1;
};

static
int missions_resume_mission_send_syncdata(void *data)
{
#pragma pack(push,1)
	struct {
		/*Padding to ensure the 'short' members in 'aligned' are aligned on a 2byte address boundary.*/
		char _pad0;
		struct {
			struct {
				char packet_id;
				short playerid;
				short vehicleid;
				short lrkey;
				short udkey;
				short partial_keys;
			} structured;
			char more_data[200];
		} aligned;
	} syncdata;
#pragma pack(pop)
	struct RPCDATA_PutPlayerInVehicle ppiv;
	struct MISSION_RESUME_DATA *rd;
	struct PAUSED_MISSION *paused;
	struct SampVehicle *vehicle;
	struct BitStream bitstream;
	struct MISSION *mission;
	char text[100];
	int playerid;

	rd = data;
	playerid = rd->playerid;
	if (_cc[playerid] != rd->player_cc || number_missions_started_stopped[playerid] != rd->number_missions_started_stopped) {
		missions_resume_mission_destroy_npc(playerid);
exit:
		free(data);
		free(paused_mission[playerid]);
		paused_mission[playerid] = NULL;
		return TIMER_NO_REPEAT;
	}

	mission = activemission[playerid];
	paused = paused_mission[playerid];
	if (rd->done) {
		missions_resume_mission_destroy_npc(playerid);
		GameTextForPlayer(playerid, 2000, 3, "~n~~n~~g~You have control!");
		/*PutPlayerInVehicle is failing me, so sending the raw packet now*/
		/*This is probably not needed, but if it finally works I'm not touching it anymore*/
		/*Using samp's PutPlayerInVehicle also makes sure the vehicle is streamed in, and
		  some checkpoint shenanigans it seems like.*/
		ppiv.vehicleid = mission->vehicleid;
		ppiv.seat = 0;
		vehicle = samp_pNetGame->vehiclePool->vehicles[mission->vehicleid];
		bitstream.ptrData = &ppiv;
		bitstream.numberOfBitsUsed = sizeof(ppiv) * 8;
		SAMP_SendRPCToPlayer(RPC_PutPlayerInVehicle, &bitstream, playerid, 2);
		if (vehicle) {
			vehicle->driverplayerid = playerid; /*probably not needed but... meh*/
		}
		goto exit;
	}

	bitstream.ptrData = &syncdata.aligned;
	bitstream.numberOfBitsAllocated = sizeof(syncdata.aligned) * 8;
	if (rd->send_onfoot) {
		rd->send_onfoot = 0;
		rd->done = 1;
		/*destroying the fake player causes the vehicle to just stop in place, so send one onfoot sync packet instead*/
		bitstream.numberOfBitsUsed = 0;
		bitstream_write_bits(&bitstream, 0xCF, 8); /*onfoot sync packet*/
		bitstream_write_bits(&bitstream, FAKE_PLAYER_ID_RESUME_MISSION, 16); /*playerid*/
		bitstream_write_zero(&bitstream); /*lrkey*/
		bitstream_write_zero(&bitstream); /*udkey*/
		bitstream_write_bits(&bitstream, 0, 16); /*partial_keys*/
		bitstream_write_bytes(&bitstream, vec3_zero, 12); /*position*/
		bitstream_write_quaternion(&bitstream, 0.0f, 0.0f, 0.0f, 0.0f); /*rotation*/
		bitstream_write_bits(&bitstream, 0xFF, 8); /*player_health_armor (unsure how this val is made, but this works)*/
		bitstream_write_bits(&bitstream, 0, 8); /*weapon_id_additional_keys*/
		bitstream_write_bits(&bitstream, 0, 8); /*special_action*/
		bitstream_write_velocity(&bitstream, 0.0f, 0.0f, 0.0f);
		bitstream_write_zero(&bitstream); /*not surfing a vehicle*/
		bitstream_write_zero(&bitstream); /*no animation*/
		/*See 0x80AC4F9 (CNetGame::BroadCastPlayerSyncData+129)*/
		RakServer__Send(rakServer, &bitstream, /*prio*/1, /*rel*/7, /*stream*/1, rakPlayerID[playerid], /*broadcast*/0);
		/*send it 3x because it's important :D*/
		RakServer__Send(rakServer, &bitstream, /*prio*/1, /*rel*/7, /*stream*/1, rakPlayerID[playerid], /*broadcast*/0);
		RakServer__Send(rakServer, &bitstream, /*prio*/1, /*rel*/7, /*stream*/1, rakPlayerID[playerid], /*broadcast*/0);
		return 20; /*wait less long to exit*/
	} else {
		bitstream.numberOfBitsUsed = sizeof(syncdata.aligned.structured) * 8;
		syncdata.aligned.structured.packet_id = 0xC8; /*driver sync packet*/
		syncdata.aligned.structured.playerid = FAKE_PLAYER_ID_RESUME_MISSION;
		syncdata.aligned.structured.vehicleid = mission->vehicleid;
		syncdata.aligned.structured.lrkey = (paused->udlrkeys >> 16) & 0xFFFF;
		syncdata.aligned.structured.udkey = paused->udlrkeys & 0xFFFF;
		syncdata.aligned.structured.partial_keys = paused->gear_keys & 0xFFFF;
		bitstream_write_quaternion(&bitstream, paused->quat.w, paused->quat.x, paused->quat.y, paused->quat.z);
		bitstream_write_bytes(&bitstream, &paused->pos, 12); /*position*/
		if (rd->ms_left < MISSION_RESUME_SYNCDATA_TIMEOUT * 3 || rd->send_initial) {
			bitstream_write_velocity(&bitstream, paused->vel.x, paused->vel.y, paused->vel.z);
		} else {
			bitstream_write_velocity(&bitstream, 0.0f, 0.0f, 0.0f);
		}
		bitstream_write_bits(&bitstream, paused->hp, 16); /*vehicle_health*/
		bitstream_write_bits(&bitstream, 0xFF, 8); /*player_health_armor (unsure how this val is made, but this works)*/
		bitstream_write_bits(&bitstream, 0, 6); /*weapon_id*/
		bitstream_write_bits(&bitstream, (paused->gear_keys >> 16) & 3, 2); /*additional_keys*/
		bitstream_write_zero(&bitstream); /*siren_state*/
		if (paused->gear_keys & 0x100000) {
			bitstream_write_one(&bitstream); /*landing_gear_state*/
		} else {
			bitstream_write_zero(&bitstream); /*landing_gear_state*/
		}
		if (paused->misc) {
			bitstream_write_one(&bitstream); /*has_misc*/
			bitstream_write_bits(&bitstream, paused->misc, 32); /*has_misc*/
		} else {
			bitstream_write_zero(&bitstream); /*has_misc*/
		}
		bitstream_write_zero(&bitstream); /*has_trailer_id*/
		/*See 0x80AC4F9 (CNetGame::BroadCastPlayerSyncData+129)*/
		RakServer__Send(rakServer, &bitstream, /*prio*/1, /*rel*/7, /*stream*/1, rakPlayerID[playerid], /*broadcast*/0);
	}

	if (rd->send_initial) {
		/*
		the complete hand over time should be long enough to:
		- load the map
		- spin up the rotor if it's a heli flight
		Note that the hand over time doesn't progress while the 'paused introduction' dialog is showing,
		but the user can dismiss that dialog immediately.
		*/
		rd->send_initial = 0;
		rd->ms_left = 5000;
		return 2500; /*do a longer pause first, so the player's camera can get behind the vehicle*/
	}

	if (rd->dialog_closed) {
		rd->ms_left -= MISSION_RESUME_SYNCDATA_TIMEOUT;
		if (rd->ms_left < MISSION_RESUME_SYNCDATA_TIMEOUT) {
			rd->send_onfoot = 1;
			return 20; /*wait less long to exit after sending the last driversync*/
		}

		sprintf(text, "~n~~n~~b~Handing over control, get ready!~n~%.1fs", rd->ms_left / 1000.0f);
		GameTextForPlayer(playerid, 1000, 3, text);
	}

	return MISSION_RESUME_SYNCDATA_TIMEOUT;
}

static
void missions_cb_dlg_paused_mission_introduction(int playerid, struct DIALOG_RESPONSE response)
{
	struct MISSION_RESUME_DATA *rd;

	rd = response.data;
	rd->dialog_closed = 1;
}

static void missions_query_paused_missions(int playerid);

static
void missions_cb_dlg_continue_paused_mission(int playerid, struct DIALOG_RESPONSE response)
{
	struct MISSION_RESUME_DATA *rd;
	struct PAUSED_MISSION *paused;
	struct SampVehicle *vehicle;
	struct DIALOG_INFO dialog;
	struct MISSION *mission;
	struct vec4 pos;
	int vehicleid;

	if (paused_mission[playerid] && paused_mission[playerid]->id == (int) response.data) {
		if (response.response) {
			if (mission_stage[playerid] != MISSION_STAGE_NOMISSION || activemission[playerid]) {
				SendClientMessage(playerid, COL_WARN, WARN" You currently already are on a mission, try again later.");
			} else if (GetPlayerState(playerid) == PLAYER_STATE_WASTED) {
				SendClientMessage(playerid, COL_WARN, WARN" You can't resume a mission while dead.");
			} else {
				number_missions_started_stopped[playerid]++;
				rd = malloc(sizeof(struct MISSION_RESUME_DATA));
				rd->playerid = playerid;
				rd->player_cc = _cc[playerid];
				rd->number_missions_started_stopped = number_missions_started_stopped[playerid];
				rd->send_initial = 1;
				rd->send_onfoot = 0;
				rd->done = 0;
				rd->dialog_closed = 0;
				paused = paused_mission[playerid];
				pos.coords = paused->pos;
				pos.r = 0.0f;
				vehicleid = CreateVehicle(paused->vehmodel, &pos, 1, 1, 1000000);
				vehicle_fuel[vehicleid] = paused->fuel_percentage * vehicle_fuel_cap[vehicleid];
				mission_stage[playerid] = MISSION_STAGE_FLIGHT;
				activemission[playerid] = mission = malloc(sizeof(struct MISSION));
				mission->id = paused->id;
				mission->missiontype = paused->missiontype;
				mission->startpoint = paused->startpoint;
				mission->endpoint = paused->endpoint;
				mission->distance = paused->distance;
				mission->actualdistanceM = paused->actualdistanceM;
				mission->previous_duration_s = paused->previous_duration_s;
				mission->passenger_satisfaction = paused->passenger_satisfaction;
				mission->vehmodel = paused->vehmodel;
				mission->vehicleid = vehicleid;
				mission->starttime = time(NULL);
				mission->lastvehiclehp = (short) paused->hp;
				mission->damagetaken = paused->damage;
				mission->lastfuel = vehicle_fuel[vehicleid];
				mission->fuelburned = paused->fuel_burned_percentage * vehicle_fuel_cap[vehicleid];
				mission->weatherbonus = paused->weatherbonus;
				mission->flags = MISSION_FLAG_WAS_PAUSED;
				mission->flags |= MISSION_FLAG_NEEDS_TRACKER_RESUME_FLAG;
				mission->flags |= MISSION_FLAG_IS_PAUSED_SET_TO_IN_PROGRESS_ON_NEXT_DRIVERSYNC;
				playerpool->virtualworld[playerid] = VW_MISSION_RESUME_BASE + playerid;
				vehiclepool->virtualworld[vehicleid] = VW_MISSION_RESUME_BASE + playerid;
				vehicle = vehiclepool->vehicles[vehicleid];
				/*set vehicle stuff so that nav works during the handover period*/
				if (vehicle) {
					vehicle->vel = paused->vel;
					QuaternionToMatrix(&paused->quat, &vehicle->matrix);
				}
				nav_navigate_to_airport(playerid, mission->vehicleid, mission->vehmodel, mission->endpoint->ap);
				natives_PutPlayerInVehicle(playerid, vehicleid, /*seat*/ 0x3F);
				missions_resume_mission_create_npc(playerid);
				SetPlayerRaceCheckpointNoDir(playerid, RACE_CP_TYPE_NORMAL, &mission->endpoint->pos, MISSION_CP_RAD);
				timer_set(0, missions_resume_mission_send_syncdata, rd);
				if (mission->missiontype & SETTING__PASSENGER_MISSIONTYPES) {
					missions_show_passenger_satisfaction_textdraw(mission->passenger_satisfaction, playerid, vehicleid);
				}
				kneeboard_update_all(playerid, &paused->pos);
				dialog_init_info(&dialog);
				dialog.info =
					"You are put in a separate world with a temporary vehicle to resume this flight.\n"
					"While resuming this flight, you will not see any other vehicles or players,\n"
					"but you can still use the chat to talk with other players.\n"
					"Your aircraft will be deleted and you will respawn when this flight ends.";
				dialog.transactionid = DLG_TID_PAUSEDMISSION;
				dialog.style = DIALOG_STYLE_MSGBOX;
				dialog.caption = "Resuming paused flight";
				dialog.button1 = "Ok";
				dialog.handler.callback = missions_cb_dlg_paused_mission_introduction;
				dialog.handler.data = rd;
				dialog_show(playerid, &dialog);
			}
			return;
		} else {
			csprintf(buf4096, "UPDATE flg SET state=%d WHERE id=%d", MISSION_STATE_ABORTED, paused_mission[playerid]->id);
			NC_mysql_tquery_nocb(buf4096a);

			free(paused_mission[playerid]);
			paused_mission[playerid] = NULL;
		}
	};
	missions_query_paused_missions(playerid);
}

static
void missions_prompt_continue_paused_mission(int playerid)
{
	struct PAUSED_MISSION *paused;
	struct DIALOG_INFO dialog;
	char *info;

	paused = paused_mission[playerid];
	dialog_init_info(&dialog);
	info = dialog.info;
	info += sprintf(info, ""ECOL_DIALOG_TEXT"Welcome back!\n\n");
	info += sprintf(info, "It seems like a mission was in progress when you left:\n");
	info += sprintf(info, "Flight: "ECOL_MISSION"#%d\n", paused->id);
	info += sprintf(info, ""ECOL_DIALOG_TEXT"Aircraft: "ECOL_MISSION"%s\n", vehnames[paused->vehmodel - VEHICLE_MODEL_MIN]);
	info += sprintf(info, ""ECOL_DIALOG_TEXT"Origin: "ECOL_MISSION"%s\n", paused->startpoint->ap->name);
	info += sprintf(info, ""ECOL_DIALOG_TEXT"Destination: "ECOL_MISSION"%s\n\n", paused->endpoint->ap->name);
	info += sprintf(info, ""ECOL_DIALOG_TEXT"Do you want to continue this flight?\n");
	info += sprintf(info, "Note: aborting will result in a $"SETTING__MISSION_CANCEL_FEE_STR" fine.");
	dialog.transactionid = DLG_TID_PAUSEDMISSION;
	dialog.style = DIALOG_STYLE_MSGBOX;
	dialog.caption = "Paused flight found";
	dialog.button1 = "Continue";
	dialog.button2 = "Abort";
	dialog.handler.callback = missions_cb_dlg_continue_paused_mission;
	dialog.handler.data = (void*) paused->id;
	dialog_show(playerid, &dialog);
}

static
void missions_cb_load_paused_mission(void *data)
{
	int playerid, *field, from_msp_id, to_msp_id, missiontype, i, mission_id;
	struct MISSIONPOINT *msp, *from_msp, *to_msp;
	struct PAUSED_MISSION *paused;

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	if (paused_mission[playerid]) {
		return;
	}

	if (NC_cache_get_row_count()) {
		NC_PARS(2);
		field = nc_params + 2;
		nc_params[1] = 0;
		missiontype = (*field = 16, NC(n_cache_get_field_i));
		from_msp_id = (*field = 17, NC(n_cache_get_field_i));
		to_msp_id = (*field = 18, NC(n_cache_get_field_i));
		mission_id = (*field = 21, NC(n_cache_get_field_i));
		to_msp = NULL;
		from_msp = NULL;
		for (i = 0, msp = missionpoints; i < nummissionpoints; i++, msp++) {
			if (from_msp_id == msp->id) {
				from_msp = msp;
			}
			if (to_msp_id == msp->id) {
				to_msp = msp;
			}
		}
		if (!from_msp || !to_msp || !(from_msp->type & missiontype) || !(to_msp->type & missiontype)) {
			sprintf(cbuf4096, "UPDATE flg SET state=%d WHERE id=%d", MISSION_STATE_EXPIRED, mission_id);
			NC_mysql_tquery_nocb(buf4096a);
			missions_query_paused_missions(playerid);
			return;
		}

		NC_PARS(2);
		nc_params[1] = 0;
		paused = paused_mission[playerid] = malloc(sizeof(struct PAUSED_MISSION));
		paused->is_cancelled = 0;
		paused->vehmodel = (*field = 0, NC(n_cache_get_field_i));
		paused->pos.x = (*field = 1, NCF(n_cache_get_field_f));
		paused->pos.y = (*field = 2, NCF(n_cache_get_field_f));
		paused->pos.z = (*field = 3, NCF(n_cache_get_field_f));
		paused->quat.w = (*field = 4, NCF(n_cache_get_field_f));
		paused->quat.x = (*field = 5, NCF(n_cache_get_field_f));
		paused->quat.y = (*field = 6, NCF(n_cache_get_field_f));
		paused->quat.z = (*field = 7, NCF(n_cache_get_field_f));
		paused->vel.x = (*field = 8, NCF(n_cache_get_field_f));
		paused->vel.y = (*field = 9, NCF(n_cache_get_field_f));
		paused->vel.z = (*field = 10, NCF(n_cache_get_field_f));
		paused->fuel_percentage = (*field = 11, NCF(n_cache_get_field_f));
		paused->hp = (short) (*field = 12, NCF(n_cache_get_field_f));
		paused->gear_keys = (*field = 13, NC(n_cache_get_field_i));
		paused->udlrkeys = (*field = 14, NC(n_cache_get_field_i));
		paused->misc = (*field = 15, NC(n_cache_get_field_i));
		paused->missiontype = missiontype;
		paused->startpoint = from_msp;
		paused->endpoint = to_msp;
		paused->passenger_satisfaction = (*field = 19, NC(n_cache_get_field_i));
		paused->weatherbonus = (*field = 20, NC(n_cache_get_field_i));
		paused->id = mission_id;
		paused->damage = (*field = 22, NCF(n_cache_get_field_f));
		paused->distance = (*field = 23, NCF(n_cache_get_field_f));
		paused->actualdistanceM = (*field = 24, NCF(n_cache_get_field_f));
		paused->previous_duration_s = (*field = 25, NCF(n_cache_get_field_f));
		paused->fuel_burned_percentage = (*field = 26, NCF(n_cache_get_field_f));
		if (spawned[playerid] && mission_stage[playerid] == MISSION_STAGE_NOMISSION) {
			missions_prompt_continue_paused_mission(playerid);
		}
	}
}

static
void missions_query_paused_missions(int playerid)
{
	sprintf(cbuf4096_,
		"SELECT v.m,"
		"p.x,p.y,p.z,p.qw,p.qx,p.qy,p.qz,p.vx,p.vy,p.vz,p.fuel,p.hp,p.gear_keys,p.udlrkeys,p.misc,"
		"f.missiontype,f.fmsp,f.tmsp,f.satisfaction,f.pweatherbonus,f.id,f.damage,f.distance,f.adistance,f.duration,f.fuel "
		"FROM pfl p "
		"JOIN flg f ON p.fid=f.id "
		"JOIN veh v ON f.vehicle=v.i "
		"WHERE f.state=%d AND f.player=%d "
		"ORDER BY f.tstart ASC, p.t DESC "
		"LIMIT 1",
		MISSION_STATE_PAUSED,
		userid[playerid]
	);
	common_mysql_tquery(cbuf4096_, missions_cb_load_paused_mission, V_MK_PLAYER_CC(playerid));
}

static
void missions_on_player_spawn(int playerid, struct vec3 pos)
{
	missions_available_msptype_mask[playerid] = CLASS_MSPTYPES[classid[playerid]];
	missions_update_missionpoint_indicators(playerid, pos.x, pos.y, pos.z);
	if (paused_mission[playerid] && !paused_mission[playerid]->is_cancelled) {
		missions_prompt_continue_paused_mission(playerid);
	} else if (userid[playerid] > 0 && loggedstatus[playerid] == LOGGED_IN) {
		missions_query_paused_missions(playerid);
	}
}

void missions_update_satisfaction(int pid, int vid, float pitch, float roll)
{
	struct MISSION *miss;
	int last_satisfaction;
	float pitchlimit, rolllimit;

	if (mission_stage[pid] == MISSION_STAGE_FLIGHT &&
		(miss = activemission[pid]) != NULL &&
		(miss->missiontype & SETTING__PASSENGER_MISSIONTYPES) &&
		miss->vehicleid == vid &&
		miss->passenger_satisfaction != 0)
	{
		if (game_is_heli(miss->vehmodel)) {
			pitchlimit = 50.0f;
			rolllimit = 40.0f;
		} else {
			pitchlimit = 20.0f;
			rolllimit = 36.0f;
		}
		last_satisfaction = miss->passenger_satisfaction;

		if (pitch < 0.0f) {
			pitch = -pitch;
		}
		if (pitch > pitchlimit) {
			miss->passenger_satisfaction -= (int) ((pitch - pitchlimit) / 2.0f);
			if (miss->passenger_satisfaction < 0) {
				miss->passenger_satisfaction = 0;
			}
		}

		if (roll < 0.0f) {
			roll = -roll;
		}
		if (roll > rolllimit) {
			miss->passenger_satisfaction -= (int) ((roll - rolllimit) / 2.0f);
			if (miss->passenger_satisfaction < 0) {
				miss->passenger_satisfaction = 0;
			}
		}

		if (last_satisfaction != miss->passenger_satisfaction) {
			missions_update_passenger_satisfaction_textdraw(miss->passenger_satisfaction, pid, vid);
		}
	}
}

int missions_get_distance_to_next_cp(int playerid, struct vec3 *frompos)
{
	struct MISSION *miss;
	struct vec3 cp;

	if ((miss = activemission[playerid]) == NULL) {
		return -1;
	}

	if (mission_stage[playerid] < MISSION_STAGE_FLIGHT) {
		cp = miss->startpoint->pos;
	} else {
		cp = miss->endpoint->pos;
	}

	cp.x -= frompos->x;
	cp.y -= frompos->y;
	cp.z -= frompos->z;
	return (int) sqrt(common_vectorsize_sq(cp));
}

static
int missions_format_kneeboard_info_text(int playerid, char *dest)
{
	if (mission_stage[playerid] > MISSION_STAGE_LOAD) {
		return sprintf(dest, "~w~Flight from %s to ~r~%s~w~.",
			activemission[playerid]->startpoint->ap->name,
			activemission[playerid]->endpoint->ap->name);
	} else if (mission_stage[playerid] >= MISSION_STAGE_PRELOAD) {
		return sprintf(dest, "~w~Flight from ~r~%s~w~ to %s.",
			activemission[playerid]->startpoint->ap->name,
			activemission[playerid]->endpoint->ap->name);
	} else {
		return sprintf(dest, "%s", "~w~Not on a job, type ~y~/w~w~ to find a missionpoint.");
	}
}
