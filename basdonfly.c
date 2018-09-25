
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"

logprintf_t logprintf;
extern void *pAMXFunctions;

#define FORWARD(X) cell AMX_NATIVE_CALL X(AMX *amx, cell *params)

/* various.c */
FORWARD(Urlencode);
/* panel.c */
FORWARD(Panel_ResetCaches);
FORWARD(Panel_FormatAltitude);
FORWARD(Panel_FormatSpeed);
FORWARD(Panel_FormatHeading);
/* game_sa.c */
FORWARD(IsAirVehicle);
/* login.c */
FORWARD(ResetPasswordConfirmData);
FORWARD(SetPasswordConfirmData);
FORWARD(ValidatePasswordConfirmData);

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
	void game_sa_init(), login_init();

	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t) ppData[PLUGIN_DATA_LOGPRINTF];

	game_sa_init();
	login_init();

	return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
}

#define REGISTERNATIVE(X) {#X, X}
AMX_NATIVE_INFO PluginNatives[] =
{
	REGISTERNATIVE(ValidateMaxPlayers),
	REGISTERNATIVE(Urlencode),
	REGISTERNATIVE(Panel_ResetCaches),
	REGISTERNATIVE(Panel_FormatAltitude),
	REGISTERNATIVE(Panel_FormatSpeed),
	REGISTERNATIVE(Panel_FormatHeading),
	REGISTERNATIVE(IsAirVehicle),
	REGISTERNATIVE(ResetPasswordConfirmData),
	REGISTERNATIVE(SetPasswordConfirmData),
	REGISTERNATIVE(ValidatePasswordConfirmData),
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

