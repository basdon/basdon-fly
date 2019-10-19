
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"

void common_tp_player(AMX *amx, int playerid, struct vec3 pos, float r)
{
	natives_NC_SetPlayerPos(amx, playerid, pos);
	nc_params[0] = 2;
	nc_params[1] = playerid;
	*((float*) (nc_params + 2)) = r;
	NC(n_SetPlayerFacingAngle);
	nc_params[0] = 1;
	NC(n_SetCameraBehindPlayer);
}

void common_hide_gametext_for_player(AMX *amx, int playerid)
{
	*buf32 = '_';
	*(buf32 + 1) = 0;
	NC_GameTextForPlayer(playerid, buf32a, 2, 3);
}