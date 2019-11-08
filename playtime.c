
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include <string.h>

/* native Playtime_FormatUpdateTimes(userid, sessionid, playtimetoadd, isdisconnect, buf[]) */
cell AMX_NATIVE_CALL Playtime_FormatUpdateTimes(AMX *amx, cell *params)
{
	int userid = params[1], sessionid = params[2], playtimetoadd = params[3];
	int isdisconnect = params[4];
	int position;
	char data[512];
	cell *addr, *adjustedaddr;
	if (userid == -1) {
		return 0;
	}
	amx_GetAddr(amx, params[5], &addr);
	position = 2;
	adjustedaddr = addr + position;
	*addr = 0;
	*(addr + 1) = 0;
	position += 1 + sprintf(data,
		"UPDATE usr SET lastseengame=UNIX_TIMESTAMP(),onlinetime=onlinetime+%d,"
		"playtime=playtime+%d WHERE i=%d",
		isdisconnect ? 0 : 30,
		playtimetoadd,
		userid);
	amx_SetUString(adjustedaddr, data, sizeof(data));
	if (sessionid == -1) {
		return 1;
	}
	*addr = position;
	adjustedaddr = addr + position;
	position += 1 + sprintf(data, "UPDATE ses SET e=UNIX_TIMESTAMP() WHERE i=%d", sessionid);
	amx_SetUString(adjustedaddr, data, sizeof(data));
	if (!isdisconnect) {
		return 1;
	}
	*(addr + 1) = position;
	adjustedaddr = addr + position;
	sprintf(data,
		"UPDATE usr SET onlinetime=(SELECT SUM(e-s) FROM ses WHERE u=usr.i) WHERE i=%d",
		userid);
	amx_SetUString(adjustedaddr, data, sizeof(data));
	return 1;
}
