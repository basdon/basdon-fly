
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
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
