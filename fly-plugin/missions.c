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

struct MISSION {
	int id;
	int missiontype;
	struct MISSIONPOINT *startpoint, *endpoint;
	float distance, actualdistanceM;
	int passenger_satisfaction;
	struct dbvehicle *veh;
	/**
	The vehicleid of the vehicle the mission was started with.
	*/
	int vehicleid;
	/**
	Reincarnation value of vehicleid when the mission was started.

	Use in combination with vehicleid to check if vehicle at the end is
	the same vehicle the mission was started with.
	*/
	int vehicle_reincarnation;
	time_t starttime;
	short lastvehiclehp, damagetaken;
	float lastfuel, fuelburned;
	short weatherbonus;
};

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
void cb_missions_flight_finish_query_done(void *data)
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
	struct vec3 pos;
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

	free(mission);
	activemission[playerid] = NULL;
	mission_stage[playerid] = MISSION_STAGE_NOMISSION;
	number_missions_started_stopped[playerid]++;

	GetPlayerPos(playerid, &pos);
	kneeboard_update_all(playerid, &pos);
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

	mission = activemission[playerid];
	sprintf(query,
	        "UPDATE flg "
		"SET state=%d,tlastupdate=UNIX_TIMESTAMP(),adistance=%f "
		"WHERE id=%d",
	        reason,
		mission->actualdistanceM,
	        mission->id);
	common_mysql_tquery(query, cb_missions_flight_finish_query_done, (void*) mission->id);

	missions_cleanup(playerid);
}

void missions_on_vehicle_destroyed_or_respawned(struct dbvehicle *veh)
{
	struct MISSION *mission;
	int i, playerid;

	for (i = 0; i < playercount; i++) {
		playerid = players[i];
		mission = activemission[playerid];
		if (mission != NULL && mission->veh == veh) {
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
	number_missions_started_stopped[playerid]++;
}

static
void missions_on_player_disconnect(int playerid, int reason)
{
	struct MISSION *miss;

	if ((miss = activemission[playerid]) != NULL) {
		switch (reason) {
		case 0: missions_end_unfinished(playerid, MISSION_STATE_ABANDONED_TIMEOUT); break;
		default: missions_end_unfinished(playerid, MISSION_STATE_ABANDONED_QUIT); break;
		case 2: missions_end_unfinished(playerid, MISSION_STATE_ABANDONED_KICKED); break;
		}
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
		nav_navigate_to_airport(playerid, mission->veh->spawnedvehicleid, mission->veh->model, mission->endpoint->ap);
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
	*((float*) (cbuf32 + 8)) = model_fuel_capacity(mission->veh->model);
	*((short*) (cbuf32 + 12)) = mission->veh->model;
	cbuf32[14] = pdata[playerid]->namelen;
	memset(cbuf32 + 15, 0, 24); /* don't leak random data */
	strcpy(cbuf32 + 15, pdata[playerid]->name);
	/*buf32 is len 32 * 4 so 40 is fine*/
	NC_ssocket_send(tracker, buf32a, 40);

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
		if ((mission = activemission[players[i]]) != NULL && mission->veh == veh) {
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
	mission->passenger_satisfaction = 100;
	mission->veh = veh;
	mission->vehicleid = vehicleid;
	mission->vehicle_reincarnation = gamevehicles[vehicleid].reincarnation;
	mission->starttime = time(NULL);
	mission->lastvehiclehp = (short) vhp;
	mission->damagetaken = 0;
	mission->lastfuel = veh->fuel;
	mission->fuelburned = 0.0f;
	mission->weatherbonus = missions_get_initial_weatherbonus();

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
	SendClientMessage(playerid, COL_MISSION, cbuf144);

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
	int totaltime, duration_h, duration_m, extra_damage_taken;
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
	miss->fuelburned += miss->lastfuel - miss->veh->fuel;

	totaltime = (int) difftime(time(NULL), miss->starttime);
	duration_m = totaltime % 60;
	duration_h = (totaltime - duration_m) / 60;

	/* don't use adistance because it also includes z changes */
	mintime = miss->distance;
	mintime /= missions_get_vehicle_maximum_speed(miss->veh->model);
	if (totaltime < (int) mintime) {
		pcheat -= 250000;
		sprintf(cbuf144,
			"flg(#%d) too fast: min: %d actual: %d",
			miss->id,
			(int) mintime,
			totaltime);
		anticheat_log(playerid, AC_MISSION_TOOFAST, cbuf144);
	}

	paymp = missions_get_vehicle_paymp(miss->veh->model);
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
	        vehnames[miss->veh->model - VEHICLE_MODEL_MIN]);
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
		"tlastupdate=UNIX_TIMESTAMP(),"
		"state=%d,fuel=%f,ptax=%d,pweatherbonus=%d,psatisfaction=%d,"
		"pdistance=%d,pdamage=%d,pcheat=%d,pbonus=%d,ptotal=%d,"
		"satisfaction=%d,adistance=%f,paymp=%f,damage=%d "
		"WHERE id=%d",
		MISSION_STATE_FINISHED,
		miss->fuelburned,
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
	common_mysql_tquery(cbuf4096_, cb_missions_flight_finish_query_done, (void*) miss->id);

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
	struct dbvehicle *veh;
	int vehicleid, vv;

	if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		return;
	}

	mission = activemission[playerid];

	if (mission == NULL) {
		return;
	}

	vehicleid = veh_GetPlayerVehicle(playerid, &vv, &veh);
	if (vehicleid != mission->vehicleid || vv != mission->vehicle_reincarnation || veh->id != mission->veh->id) {
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

	if ((mission = activemission[playerid]) == NULL ||
		mission->veh->spawnedvehicleid != vehicleid ||
		(isafk[playerid] && tracker_afk_packet_sent[playerid]))
	{
		return;
	}

	tracker_afk_packet_sent[playerid] = flags = isafk[playerid] == 1;
	if (engine) {
		flags |= 2;
	}

	hpv = (short) hp;
	alt = (short) vpos->z;
	spd = (short) (VEL_TO_KTS * sqrt(vvel->x * vvel->x + vvel->y * vvel->y + vvel->z * vvel->z));
	pitch10 = (short) (pitch * 10.0f);
	roll10 = (short) (roll * 10.0f);

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
		missions_end_unfinished(playerid, MISSION_STATE_DECLINED);
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
			miss->veh->spawnedvehicleid == vehicleid)
		{
			miss->fuelburned += refuelamount;
			miss->lastfuel = miss->veh->fuel;
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
			miss->veh->spawnedvehicleid == vehicleid)
		{
			miss->damagetaken += (short) fixamount;
			miss->lastvehiclehp = (short) newhp;
			return;
		}
	}
}

static
void missions_on_player_spawn(int playerid, struct vec3 pos)
{
	missions_available_msptype_mask[playerid] = CLASS_MSPTYPES[classid[playerid]];
	missions_update_missionpoint_indicators(playerid, pos.x, pos.y, pos.z);
}

void missions_update_satisfaction(int pid, int vid, float pitch, float roll)
{
	struct MISSION *miss;
	int last_satisfaction;
	float pitchlimit, rolllimit;

	if (mission_stage[pid] == MISSION_STAGE_FLIGHT &&
		(miss = activemission[pid]) != NULL &&
		(miss->missiontype & SETTING__PASSENGER_MISSIONTYPES) &&
		miss->veh->spawnedvehicleid == vid &&
		miss->passenger_satisfaction != 0)
	{
		if (game_is_heli(miss->veh->model)) {
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
