
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"

static unsigned int airvehicles[] = {
	0x02020000, 0x10008000, 0x01801023, 0x01838022, 0x02100000, 0x00020008, 0x00000003
};

cell AMX_NATIVE_CALL IsAirVehicle(AMX *amx, cell *params)
{
	int model = params[1] - 400;
	return model != -400 && ((airvehicles[model / 32] >> (model & 31)) & 1);
}

