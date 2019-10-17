
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"

void common_tp_player(AMX *amx, int pid, float x, float y, float z, float r)
{
	nc_params[0] = 4;
	nc_params[1] = pid;
	*((float*) (nc_params + 2)) = x;
	*((float*) (nc_params + 3)) = y;
	*((float*) (nc_params + 4)) = z;
	NC(n_SetPlayerPos);
	nc_params[0] = 2;
	*((float*) (nc_params + 2)) = r;
	NC(n_SetPlayerFacingAngle);
	nc_params[0] = 1;
	NC(n_SetCameraBehindPlayer);
}