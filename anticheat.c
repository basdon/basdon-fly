
/* vim: set filetype=c ts=8 noexpandtab: */

#define _CRT_SECURE_NO_DEPRECATE
#include "common.h"
#include "vehicles.h"
#include <math.h>
#include <string.h>
#include "playerdata.h"

/* native Ac_FormatLog(playerid, loggedstatus, const message[], buf[]) */
cell AMX_NATIVE_CALL Ac_FormatLog(AMX *amx, cell *params)
{
	const int pid = params[1], loggedstatus = params[2];
	cell *addr;
	char msg[144];
	char buf[512];
	char uid[12];
	char ip[16];
	if (pdata[pid] == NULL) {
		strcpy(uid, "NULL");
		strcpy(ip, "0.0.0.0");
	} else {
		if (pdata[pid]->userid < 1) {
			strcpy(uid, "NULL");
		} else {
			sprintf(uid, "%d", pdata[pid]->userid);
		}
		strcpy(ip, pdata[pid]->ip);
	}
	amx_GetAddr(amx, params[3], &addr);
	amx_GetUString(msg, addr, sizeof(msg));
	sprintf(buf,
	        "INSERT INTO acl(t,j,u,l,e) VALUES(UNIX_TIMESTAMP(),'%s',%s,%d,'%s')",
	        ip,
	        uid,
		loggedstatus,
		msg);
	amx_GetAddr(amx, params[4], &addr);
	amx_SetUString(addr, buf, sizeof(buf));
	return 1;
}

void anticheat_log(AMX *amx, int playerid, int eventtype, char *info)
{
	char buf[512], *b = buf;

	while (*b != 0) {
		if (*b == '\'') {
			*b = 255;
		}
		b++;
	}

	b = buf;
	b += sprintf(b, "INSERT INTO acl(t,j,u,l,type,e) "
			"VALUES(UNIX_TIMESTAMP(),");
	if (pdata[playerid] == NULL) {
		/*this should not happen, but better to be safe when dealing
		with strange behaviors that trigger anticheat*/
		b += sprintf(b, "'',NULL,-1");
	} else {
		b += sprintf(b,
			"'%s',%d,%d",
			pdata[playerid]->ip,
			pdata[playerid]->userid,
			loggedstatus[playerid]);
	}
	sprintf(b, ",%d,'%s')", eventtype, info);
	amx_SetUString(buf4096, buf, sizeof(buf));
	NC_mysql_tquery_nocb(buf4096a);
}

float anticheat_NC_GetVehicleHealth(AMX *amx, int vehicleid)
{
	float hp;
	int playerid;

	nc_params[0] = 2;
	nc_params[1] = vehicleid;
	nc_params[2] = buf144a;
	NC(n_GetVehicleHealth);
	hp = *((float*) buf144);
	if (common_is_nan(hp)) {
		playerid = common_find_vehicle_driver(amx, vehicleid);
		if (playerid == INVALID_PLAYER_ID) {
			goto resethp;
		}
		anticheat_log(amx, playerid, AC_VEH_HP_NAN, "NaN vehicle hp");
	} else if (hp > 1000.0f) {
		playerid = common_find_vehicle_driver(amx, vehicleid);
		if (playerid == INVALID_PLAYER_ID) {
			goto resethp;
		}
		sprintf((char*) buf144, "vehicle hp %.4f", hp);
		anticheat_log(amx, playerid, AC_VEH_HP_HIGH, (char*) buf144);
	} else if (hp < 0.0f) {
		return 0.0f;
	} else {
		return hp;
	}
	common_crash_player(amx, playerid);
	sprintf((char*) buf4096,
		"%s[%d] was kicked by system (invalid vehicle hp)",
		pdata[playerid]->name,
		playerid);
	amx_SetUString(buf144, (char*) buf4096, 144);
	NC_SendClientMessageToAll(COL_WARN, buf144a);
	common_NC_Kick(playerid);
resethp:
	NC_SetVehicleHealth(vehicleid, 1000.0f);
	return 1000.0f;
}
