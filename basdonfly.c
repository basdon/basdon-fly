
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
/* playtime.c */
FORWARD(Playtime_FormatUpdateTimes);
/* game_sa.c */
FORWARD(IsAirVehicle);
FORWARD(IsHelicopter);
FORWARD(IsPlane);
/* login.c */
FORWARD(Login_FormatChangePassword);
FORWARD(Login_FormatChangePasswordBox);
FORWARD(Login_FormatCheckUserExist);
FORWARD(Login_FormatCreateSession);
FORWARD(Login_FormatCreateUser);
FORWARD(Login_FormatCreateUserSession);
FORWARD(Login_FormatGuestRegisterBox);
FORWARD(Login_FormatLoadAccountData);
FORWARD(Login_FormatOnJoinRegisterBox);
FORWARD(Login_FormatSavePlayerName);
FORWARD(Login_FormatUpgradeGuestAcc);
FORWARD(Login_FreePassword);
FORWARD(Login_GetPassword);
FORWARD(Login_UsePassword);
FORWARD(Login_PasswordConfirmFree);
FORWARD(Login_PasswordConfirmStore);
FORWARD(Login_PasswordConfirmValidate);
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
/* vehicles.c */
FORWARD(Veh_Add);
FORWARD(Veh_CollectPlayerVehicles);
FORWARD(Veh_CollectSpawnedVehicles);
FORWARD(Veh_Destroy);
FORWARD(Veh_Init);
FORWARD(Veh_GetLabelToDelete);
FORWARD(Veh_OnPlayerDisconnect);
FORWARD(Veh_RegisterLabel);
FORWARD(Veh_ShouldCreateLabel);
FORWARD(Veh_UpdateSlot);
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
	REGISTERNATIVE(Playtime_FormatUpdateTimes),
	REGISTERNATIVE(Login_FormatChangePassword),
	REGISTERNATIVE(Login_FormatChangePasswordBox),
	REGISTERNATIVE(Login_FormatCheckUserExist),
	REGISTERNATIVE(Login_FormatCreateSession),
	REGISTERNATIVE(Login_FormatCreateUser),
	REGISTERNATIVE(Login_FormatCreateUserSession),
	REGISTERNATIVE(Login_FormatGuestRegisterBox),
	REGISTERNATIVE(Login_FormatLoadAccountData),
	REGISTERNATIVE(Login_FormatOnJoinRegisterBox),
	REGISTERNATIVE(Login_FormatSavePlayerName),
	REGISTERNATIVE(Login_FormatUpgradeGuestAcc),
	REGISTERNATIVE(Login_FreePassword),
	REGISTERNATIVE(Login_GetPassword),
	REGISTERNATIVE(Login_PasswordConfirmFree),
	REGISTERNATIVE(Login_PasswordConfirmStore),
	REGISTERNATIVE(Login_PasswordConfirmValidate),
	REGISTERNATIVE(Login_UsePassword),
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
	REGISTERNATIVE(Veh_Add),
	REGISTERNATIVE(Veh_CollectSpawnedVehicles),
	REGISTERNATIVE(Veh_CollectPlayerVehicles),
	REGISTERNATIVE(Veh_Destroy),
	REGISTERNATIVE(Veh_Init),
	REGISTERNATIVE(Veh_GetLabelToDelete),
	REGISTERNATIVE(Veh_OnPlayerDisconnect),
	REGISTERNATIVE(Veh_RegisterLabel),
	REGISTERNATIVE(Veh_ShouldCreateLabel),
	REGISTERNATIVE(Veh_UpdateSlot),
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

