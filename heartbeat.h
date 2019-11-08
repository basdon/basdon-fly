
/* vim: set filetype=c ts=8 noexpandtab: */

/**
Performs query to update endtime of current session. Call every 30 seconds.
*/
void heartbeat_timed_update(AMX*);
/**
Call from OnGameModeInit.
*/
void heartbeat_create_session(AMX*);
/**
Call from OnGameModeExit.
*/
void heartbeat_end_session(AMX*);