
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"

void common_tp_player(AMX *amx, int pid, struct vec3 pos, float r)
{
	natives_NC_SetPlayerPos(amx, pid, pos);
	nc_params[0] = 2;
	nc_params[1] = pid;
	*((float*) (nc_params + 2)) = r;
	NC(n_SetPlayerFacingAngle);
	nc_params[0] = 1;
	NC(n_SetCameraBehindPlayer);
}