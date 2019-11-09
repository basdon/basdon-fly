
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "heartbeat.h"
#include <string.h>

/**
The session id of the currently running server.
*/
static int sessionid = -1;

void heartbeat_timed_update(AMX *amx)
{
	if (sessionid != -1) {
		sprintf(cbuf4096,
			"UPDATE heartbeat "
			"SET tlast=UNIX_TIMESTAMP() "
			"WHERE id=%d",
			sessionid);
		amx_SetUString(buf144, cbuf4096, 144);
		NC_mysql_tquery_nocb(buf144a);
	}
}

void heartbeat_create_session(AMX *amx)
{
	amx_SetUString(buf144,
		"INSERT INTO heartbeat(tstart,tlast) "
		"VALUES(UNIX_TIMESTAMP(),UNIX_TIMESTAMP())",
		144);
	NC_mysql_query_(buf144a, buf4096);
	NC_cache_insert_id_(&sessionid);
	NC_cache_delete(*buf4096);
}

void heartbeat_end_session(AMX *amx)
{
	if (sessionid != -1) {
		sprintf(cbuf4096,
			"UPDATE heartbeat "
			"SET tlast=UNIX_TIMESTAMP(),cleanexit=1 "
			"WHERE id=%d",
			sessionid);
		amx_SetUString(buf144, cbuf4096, 144);
		NC_mysql_tquery_nocb(buf144a);
	}
}