
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "playerdata.h"

logprintf_t logprintf;
extern void *pAMXFunctions;

#define FORWARD(X) cell AMX_NATIVE_CALL X(AMX *amx, cell *params)

/* airport.c */
FORWARD(APT_Init);
FORWARD(APT_Destroy);
FORWARD(APT_Add);
FORWARD(APT_AddRunway);
FORWARD(APT_FormatNearestList);
FORWARD(APT_FormatBeaconList);
FORWARD(APT_MapIndexFromListDialog);
FORWARD(APT_FormatInfo);
FORWARD(APT_FormatCodeAndName);
/* commands.c */
FORWARD(CommandHash);
FORWARD(IsCommand);
FORWARD(Params_GetPlayer);
FORWARD(Params_GetString);
/* dialog.c */
FORWARD(QueueDialog);
FORWARD(DropDialogQueue);
FORWARD(HasDialogsInQueue);
FORWARD(PopDialogQueue);
/* various.c */
FORWARD(Urlencode);
/* panel.c */
FORWARD(Panel_ResetCaches);
FORWARD(Panel_FormatAltitude);
FORWARD(Panel_FormatSpeed);
FORWARD(Panel_FormatHeading);
/* playerdata.c */
FORWARD(PlayerData_Init);
FORWARD(PlayerData_Clear);
FORWARD(PlayerData_SetUserId);
FORWARD(PlayerData_UpdateName);
/* game_sa.c */
FORWARD(IsAirVehicle);
FORWARD(IsHelicopter);
FORWARD(IsPlane);
/* login.c */
FORWARD(ResetPasswordConfirmData);
FORWARD(SetPasswordConfirmData);
FORWARD(ValidatePasswordConfirmData);
FORWARD(FormatLoginApiRegister);
FORWARD(FormatLoginApiGuestRegister);
FORWARD(FormatLoginApiCheckChangePass);
FORWARD(FormatLoginApiUserExistsGuest);
FORWARD(Login_FormatCheckUserExist);
FORWARD(Login_FormatCreateUserSession);
FORWARD(Login_FormatGuestRegisterBox);
FORWARD(Login_FormatLoadAccountData);
FORWARD(Login_FormatUpgradeGuestAcc);
FORWARD(Login_UsePassword);
FORWARD(Login_GetPassword);
/* nav.c */
FORWARD(Nav_Reset);
FORWARD(Nav_EnableADF);
FORWARD(Nav_EnableVOR);
FORWARD(Nav_ToggleILS);
FORWARD(Nav_Format);
FORWARD(Nav_Update);
FORWARD(Nav_GetActiveNavType);
FORWARD(Nav_ResetCache);
/* timecyc.c */
FORWARD(Timecyc_GetCurrentWeatherMsg);
FORWARD(Timecyc_GetNextWeatherMsgQuery);
/* zones.c */
FORWARD(Zones_InvalidateForPlayer);
FORWARD(Zones_UpdateForPlayer);
FORWARD(Zones_FormatForPlayer);
FORWARD(Zones_FormatLoc);

cell AMX_NATIVE_CALL ValidateMaxPlayers(AMX *amx, cell *params)
{
	if (MAX_PLAYERS != params[1]) {
		logprintf("[FATAL] MAX_PLAYERS mistmatch: %d (plugin) vs %d (gm)", MAX_PLAYERS, params[1]);
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
	void game_sa_init(), login_init(), dialog_init(), zones_init(), nav_init();

	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t) ppData[PLUGIN_DATA_LOGPRINTF];

	game_sa_init();
	login_init();
	dialog_init();
	pdata_init();
	zones_init();
	nav_init();

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
	REGISTERNATIVE(IsHelicopter),
	REGISTERNATIVE(IsPlane),
	REGISTERNATIVE(ResetPasswordConfirmData),
	REGISTERNATIVE(SetPasswordConfirmData),
	REGISTERNATIVE(ValidatePasswordConfirmData),
	REGISTERNATIVE(CommandHash),
	REGISTERNATIVE(IsCommand),
	REGISTERNATIVE(Params_GetPlayer),
	REGISTERNATIVE(Params_GetString),
	REGISTERNATIVE(QueueDialog),
	REGISTERNATIVE(DropDialogQueue),
	REGISTERNATIVE(HasDialogsInQueue),
	REGISTERNATIVE(PopDialogQueue),
	REGISTERNATIVE(PlayerData_Init),
	REGISTERNATIVE(PlayerData_Clear),
	REGISTERNATIVE(PlayerData_SetUserId),
	REGISTERNATIVE(PlayerData_UpdateName),
	REGISTERNATIVE(FormatLoginApiRegister),
	REGISTERNATIVE(FormatLoginApiGuestRegister),
	REGISTERNATIVE(FormatLoginApiCheckChangePass),
	REGISTERNATIVE(FormatLoginApiUserExistsGuest),
	REGISTERNATIVE(Login_FormatCheckUserExist),
	REGISTERNATIVE(Login_FormatCreateUserSession),
	REGISTERNATIVE(Login_FormatGuestRegisterBox),
	REGISTERNATIVE(Login_FormatLoadAccountData),
	REGISTERNATIVE(Login_FormatUpgradeGuestAcc),
	REGISTERNATIVE(Login_UsePassword),
	REGISTERNATIVE(Login_GetPassword),
	REGISTERNATIVE(APT_Init),
	REGISTERNATIVE(APT_Destroy),
	REGISTERNATIVE(APT_Add),
	REGISTERNATIVE(APT_AddRunway),
	REGISTERNATIVE(APT_FormatNearestList),
	REGISTERNATIVE(APT_FormatBeaconList),
	REGISTERNATIVE(APT_MapIndexFromListDialog),
	REGISTERNATIVE(APT_FormatInfo),
	REGISTERNATIVE(APT_FormatCodeAndName),
	REGISTERNATIVE(Nav_Reset),
	REGISTERNATIVE(Nav_EnableADF),
	REGISTERNATIVE(Nav_EnableVOR),
	REGISTERNATIVE(Nav_ToggleILS),
	REGISTERNATIVE(Nav_Format),
	REGISTERNATIVE(Nav_Update),
	REGISTERNATIVE(Nav_GetActiveNavType),
	REGISTERNATIVE(Nav_ResetCache),
	REGISTERNATIVE(Timecyc_GetCurrentWeatherMsg),
	REGISTERNATIVE(Timecyc_GetNextWeatherMsgQuery),
	REGISTERNATIVE(Zones_InvalidateForPlayer),
	REGISTERNATIVE(Zones_UpdateForPlayer),
	REGISTERNATIVE(Zones_FormatForPlayer),
	REGISTERNATIVE(Zones_FormatLoc),
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

