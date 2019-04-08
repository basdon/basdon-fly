
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include <string.h>
#include "game_sa.h"

unsigned int vehicleflags[MODEL_TOTAL];

void game_sa_init()
{
	memset(vehicleflags, 0, sizeof(vehicleflags));
#define SETFLAGS(M,F) vehicleflags[M - 400] = F
	SETFLAGS(MODEL_LEVIATHN, HELI);
	SETFLAGS(MODEL_HUNTER, HELI);
	SETFLAGS(MODEL_SEASPAR, HELI);
	SETFLAGS(MODEL_SKIMMER, PLANE);
	SETFLAGS(MODEL_RCBARON, PLANE);
	SETFLAGS(MODEL_RCRAIDER, HELI);
	SETFLAGS(MODEL_SPARROW, HELI);
	SETFLAGS(MODEL_RUSTLER, PLANE);
	SETFLAGS(MODEL_MAVERICK, HELI);
	SETFLAGS(MODEL_VCNMAV, HELI);
	SETFLAGS(MODEL_POLMAV, HELI);
	SETFLAGS(MODEL_RCGOBLIN, HELI);
	SETFLAGS(MODEL_BEAGLE, PLANE);
	SETFLAGS(MODEL_CROPDUST, PLANE);
	SETFLAGS(MODEL_STUNT, PLANE);
	SETFLAGS(MODEL_SHAMAL, PLANE);
	SETFLAGS(MODEL_HYDRA, PLANE);
	SETFLAGS(MODEL_CARGOBOB, HELI);
	SETFLAGS(MODEL_NEVADA, PLANE);
	SETFLAGS(MODEL_RAINDANC, HELI);
	SETFLAGS(MODEL_AT400, PLANE);
	SETFLAGS(MODEL_ANDROM, PLANE);
	SETFLAGS(MODEL_DODO, PLANE);
}

/* native Game_IsAirVehicle(model) */
cell AMX_NATIVE_CALL Game_IsAirVehicle(AMX *amx, cell *params)
{
	int model = params[1] - 400;
	return model != -400 && vehicleflags[model] & (HELI | PLANE);
}

/* native Game_IsHelicopter(model) */
cell AMX_NATIVE_CALL Game_IsHelicopter(AMX *amx, cell *params)
{
	int model = params[1] - 400;
	return model != -400 && vehicleflags[model] & HELI;
}

/* native Game_IsPlane(model) */
cell AMX_NATIVE_CALL Game_IsPlane(AMX *amx, cell *params)
{
	int model = params[1] - 400;
	return model != -400 && vehicleflags[model] & PLANE;
}

