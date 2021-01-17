struct COPILOT_ENTER_EXIT_OFFSET {
	char distance;
	char angle;
	char dz;
};

static struct COPILOT_ENTER_EXIT_OFFSET copilot_enter_exit_offset[MAX_PLAYERS];

static
void copilot_handle_onfoot_fire(int playerid, struct vec3 playerpos)
{
	struct vec3 vehiclepos, enter_exit_offset;
	int vehicleid, vehiclemodel, found_vehicle, number_passengerseats, seat;
	float dx, dy, dz, shortest_distance, tmpdistance;
	float enter_exit_angle;

	found_vehicle = 0;
	shortest_distance = float_pinf;
	for (vehicleid = NC_GetVehiclePoolSize(); vehicleid >= 0; vehicleid--) {
		if (!IsVehicleStreamedIn(vehicleid, playerid)) {
			continue;
		}

		vehiclemodel = GetVehicleModel(vehicleid);
		if (!game_is_air_vehicle(vehiclemodel)) {
			continue;
		}

		/*Should check veh_is_player_allowed_in_vehicle for co-piloting??*/

		GetVehiclePosUnsafe(vehicleid, &vehiclepos);
		dx = playerpos.x - vehiclepos.x;
		dy = playerpos.y - vehiclepos.y;
		dz = playerpos.z - vehiclepos.z;
		tmpdistance = dx * dx + dy * dy + dz * dz;
		if (tmpdistance < shortest_distance) {
			shortest_distance = tmpdistance;
			found_vehicle = vehicleid;
			enter_exit_offset.x = dx;
			enter_exit_offset.y = dy;
			enter_exit_offset.z = dz;
		}
	}

	if (found_vehicle && shortest_distance < 20.0f * 20.0f) {
		enter_exit_angle = (float) atan2(enter_exit_offset.y, enter_exit_offset.x);
		if (enter_exit_angle > M_PI) {
			enter_exit_angle -= M_2PI;
		}
		copilot_enter_exit_offset[playerid].angle = (char) (enter_exit_angle * 39.0f);
		copilot_enter_exit_offset[playerid].distance = (char) ((float) sqrt(shortest_distance) * 5.0f);
		copilot_enter_exit_offset[playerid].dz = (char) (enter_exit_offset.z * 5.0f);

		number_passengerseats = game_get_air_vehicle_passengerseats(vehiclemodel);
		for (seat = 1; seat < 0x40; seat++) {
			if (GetPlayerInVehicleSeat(found_vehicle, seat) == INVALID_PLAYER_ID) {
				natives_PutPlayerInVehicle(playerid, found_vehicle, seat);
				if (seat > number_passengerseats) {
					GameTextForPlayer(playerid, 5000, 3,
						"~w~entered as co-pilot~n~"
						"press ~b~~k~~VEHICLE_BRAKE~~w~ to exit~n~"
						"do not press ~r~~k~~VEHICLE_ENTER_EXIT~~w~~n~or your game will crash");
				}
				return;
			}
		}
	}
}

static
void copilot_handler_passenger_brake(int playerid, int vehicleid)
{
	int vehiclemodel, number_passengerseats;
	float enter_exit_angle, enter_exit_distance;
	struct vec3 pos;

	vehiclemodel = GetVehicleModel(vehicleid);
	number_passengerseats = game_get_air_vehicle_passengerseats(vehiclemodel);
	if (GetPlayerVehicleSeat(playerid) > number_passengerseats) {
		enter_exit_angle = (float) copilot_enter_exit_offset[playerid].angle / 39.0f;
		enter_exit_distance = (float) copilot_enter_exit_offset[playerid].distance / 5.0f;
		GetVehiclePosUnsafe(vehicleid, &pos);
		pos.x += (float) cos(enter_exit_angle) * enter_exit_distance;
		pos.y += (float) sin(enter_exit_angle) * enter_exit_distance;
		pos.z += copilot_enter_exit_offset[playerid].dz / 5.0f;
		SetPlayerPosRaw(playerid, &pos);
	}
}
