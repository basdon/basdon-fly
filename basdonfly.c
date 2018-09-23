
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"

logprintf_t logprintf;
extern void *pAMXFunctions;

/* various.c */
cell AMX_NATIVE_CALL Urlencode(AMX *amx, cell *params);
/* panel.c */
cell AMX_NATIVE_CALL Panel_ResetCaches(AMX *amx, cell *params);
cell AMX_NATIVE_CALL Panel_FormatAltitude(AMX *amx, cell *params);
cell AMX_NATIVE_CALL Panel_FormatSpeed(AMX *amx, cell *params);
cell AMX_NATIVE_CALL Panel_FormatHeading(AMX *amx, cell *params);
/* game_sa.c */
cell AMX_NATIVE_CALL IsAirVehicle(AMX *amx, cell *params);

cell AMX_NATIVE_CALL ValidateMaxPlayers(AMX *amx, cell *params)
{
	int maxplayers = params[1];
	if (MAX_PLAYERS != maxplayers) {
		logprintf("[FATAL] MAX_PLAYERS mistmatch: %d (plugin) vs %d (gm)", MAX_PLAYERS, maxplayers);
		return 0;
	}
	return MAX_PLAYERS;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT int PLUGIN_CALL Load(void **ppData)
{
	void game_sa_init();

	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t) ppData[PLUGIN_DATA_LOGPRINTF];

	game_sa_init();

	return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
}

AMX_NATIVE_INFO PluginNatives[] =
{
	{"ValidateMaxPlayers", ValidateMaxPlayers},
	{"Urlencode", Urlencode},
	{"Panel_ResetCaches", Panel_ResetCaches},
	{"Panel_FormatAltitude", Panel_FormatAltitude},
	{"Panel_FormatSpeed", Panel_FormatSpeed},
	{"Panel_FormatHeading", Panel_FormatHeading},
	{"IsAirVehicle", IsAirVehicle},
	{0, 0}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
	return amx_Register(amx, PluginNatives, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx)
{
	return AMX_ERR_NONE;
}

