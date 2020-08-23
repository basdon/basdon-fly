/**
The session id of the currently running server.
*/
static int heartbeat_sessionid = -1;

/**
Performs query to update endtime of current session. Call every 30 seconds.
*/
static
void heartbeat_timed_update()
{
	if (heartbeat_sessionid != -1) {
		csprintf(buf144, "UPDATE heartbeat SET tlast=UNIX_TIMESTAMP() WHERE id=%d", heartbeat_sessionid);
		NC_mysql_tquery_nocb(buf144a);
	}
}

/**
Call from OnGameModeInit.
*/
static
void heartbeat_create_session()
{
	int cacheid;

	atoc(buf144,
		"INSERT INTO heartbeat(tstart,tlast) "
		"VALUES(UNIX_TIMESTAMP(),UNIX_TIMESTAMP())",
		144);
	cacheid = NC_mysql_query(buf144a);
	heartbeat_sessionid = NC_cache_insert_id();
	NC_cache_delete(cacheid);
}

/**
Call from OnGameModeExit.
*/
static
void heartbeat_end_session()
{
	if (heartbeat_sessionid != -1) {
		csprintf(buf144, "UPDATE heartbeat SET tlast=UNIX_TIMESTAMP(),cleanexit=1 WHERE id=%d", heartbeat_sessionid);
		NC_mysql_tquery_nocb(buf144a);
	}
}
