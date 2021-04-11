static
void game_sa_init()
{
	memset(vehicleflags, 0, sizeof(vehicleflags));
#define SETFLAGS(M,F) vehicleflags[M - VEHICLE_MODEL_MIN] = F
	SETFLAGS(MODEL_LEVIATHN, HELI);
	SETFLAGS(MODEL_HUNTER, HELI);
	SETFLAGS(MODEL_SEASPAR, HELI);
	SETFLAGS(MODEL_SKIMMER, PLANE);
	SETFLAGS(MODEL_RCBARON, PLANE);
	SETFLAGS(MODEL_RCRAIDER, HELI);
	SETFLAGS(MODEL_SPARROW, HELI);
	SETFLAGS(MODEL_RUSTLER, PLANE | RETRACTABLE_GEAR);
	SETFLAGS(MODEL_MAVERICK, HELI);
	SETFLAGS(MODEL_VCNMAV, HELI);
	SETFLAGS(MODEL_POLMAV, HELI);
	SETFLAGS(MODEL_RCGOBLIN, HELI);
	SETFLAGS(MODEL_BEAGLE, PLANE);
	SETFLAGS(MODEL_CROPDUST, PLANE);
	SETFLAGS(MODEL_STUNT, PLANE);
	SETFLAGS(MODEL_SHAMAL, PLANE | RETRACTABLE_GEAR);
	SETFLAGS(MODEL_HYDRA, PLANE | RETRACTABLE_GEAR);
	SETFLAGS(MODEL_CARGOBOB, HELI);
	SETFLAGS(MODEL_NEVADA, PLANE | RETRACTABLE_GEAR);
	SETFLAGS(MODEL_RAINDANC, HELI);
	SETFLAGS(MODEL_AT400, PLANE | RETRACTABLE_GEAR);
	SETFLAGS(MODEL_ANDROM, PLANE | RETRACTABLE_GEAR);
	SETFLAGS(MODEL_DODO, PLANE);
	memset(aircraftmodelindex, -1, sizeof(aircraftmodelindex));
	aircraftmodelindex[MODEL_LEVIATHN - VEHICLE_MODEL_MIN] = 0;
	aircraftmodelindex[MODEL_HUNTER - VEHICLE_MODEL_MIN] = 1;
	aircraftmodelindex[MODEL_SEASPAR - VEHICLE_MODEL_MIN] = 2;
	aircraftmodelindex[MODEL_SKIMMER - VEHICLE_MODEL_MIN] = 3;
	aircraftmodelindex[MODEL_RCBARON - VEHICLE_MODEL_MIN] = 4;
	aircraftmodelindex[MODEL_RCRAIDER - VEHICLE_MODEL_MIN] = 5;
	aircraftmodelindex[MODEL_SPARROW - VEHICLE_MODEL_MIN] = 6;
	aircraftmodelindex[MODEL_RUSTLER - VEHICLE_MODEL_MIN] = 7;
	aircraftmodelindex[MODEL_MAVERICK - VEHICLE_MODEL_MIN] = 8;
	aircraftmodelindex[MODEL_VCNMAV - VEHICLE_MODEL_MIN] = 9;
	aircraftmodelindex[MODEL_POLMAV - VEHICLE_MODEL_MIN] = 10;
	aircraftmodelindex[MODEL_RCGOBLIN - VEHICLE_MODEL_MIN] = 11;
	aircraftmodelindex[MODEL_BEAGLE - VEHICLE_MODEL_MIN] = 12;
	aircraftmodelindex[MODEL_CROPDUST - VEHICLE_MODEL_MIN] = 13;
	aircraftmodelindex[MODEL_STUNT - VEHICLE_MODEL_MIN] = 14;
	aircraftmodelindex[MODEL_SHAMAL - VEHICLE_MODEL_MIN] = 15;
	aircraftmodelindex[MODEL_HYDRA - VEHICLE_MODEL_MIN] = 16;
	aircraftmodelindex[MODEL_CARGOBOB - VEHICLE_MODEL_MIN] = 17;
	aircraftmodelindex[MODEL_NEVADA - VEHICLE_MODEL_MIN] = 18;
	aircraftmodelindex[MODEL_RAINDANC - VEHICLE_MODEL_MIN] = 19;
	aircraftmodelindex[MODEL_AT400 - VEHICLE_MODEL_MIN] = 20;
	aircraftmodelindex[MODEL_ANDROM - VEHICLE_MODEL_MIN] = 21;
	aircraftmodelindex[MODEL_DODO - VEHICLE_MODEL_MIN] = 22;
}

/**
Check if the given vehicle model is an air vehicle (plane or helicopter).
*/
static
int game_is_air_vehicle(int model)
{
	return model && (vehicleflags[model - VEHICLE_MODEL_MIN] & (HELI | PLANE));
}

/**
Check if the given vehicle model is a helicopter.
*/
static
int game_is_heli(int model)
{
	return model && (vehicleflags[model - VEHICLE_MODEL_MIN] & HELI);
}

/**
Check if the given vehicle model is a plane.
*/
static
int game_is_plane(int model)
{
	return model && (vehicleflags[model - VEHICLE_MODEL_MIN] & PLANE);
}

/**
Gives a random 'normal' random vehicle colors combination for given model.

Data is from game's carcols.dat.

@param col1 ptr to int or NULL if primary color is not needed
@param col2 ptr to int or NULL if secondary color is not needed
*/
static
void game_random_carcol(int model, int *col1, int *col2)
{
	char amount;
	short position;

	if (VEHICLE_MODEL_MIN <= model && model <= VEHICLE_MODEL_MAX) {
		model -= VEHICLE_MODEL_MIN;
		amount = carcoldata[model].amount;
		position = carcoldata[model].position;
		if (amount > 1) {
			position += NC_random(amount) * 2;
		}
		if (col1 != NULL) {
			*col1 = carcols[position];
		}
		if (col2 != NULL) {
			*col2 = carcols[position + 1];
		}
	}
}

static
int game_get_air_vehicle_passengerseats(int model)
{
	switch (model)
	{
	case MODEL_ANDROM:
	case MODEL_AT400:
	case MODEL_BEAGLE:
	case MODEL_CARGOBOB:
	case MODEL_DODO:
	case MODEL_LEVIATHN:
	case MODEL_RAINDANC:
	case MODEL_SKIMMER:
	case MODEL_SPARROW:
	case MODEL_SEASPAR:
		return 1;
	case MODEL_MAVERICK:
	case MODEL_POLMAV:
		return 3;
	}
	return 0;
}
