
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "heartbeat.h"
#include <string.h>

/**
The session id of the currently running server.
*/
static int sessionid = -1;

void heartbeat_timed_update()
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

void heartbeat_create_session()
{
	int cacheid;

	amx_SetUString(buf144,
		"INSERT INTO heartbeat(tstart,tlast) "
		"VALUES(UNIX_TIMESTAMP(),UNIX_TIMESTAMP())",
		144);
	cacheid = NC_mysql_query(buf144a);
	sessionid = NC_cache_insert_id();
	NC_cache_delete(cacheid);
}

void heartbeat_end_session()
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
