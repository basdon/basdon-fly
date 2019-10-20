
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "dialog.h"
#include "playerdata.h"
#include "vehicles.h"
#include <string.h>

logprintf_t logprintf;
extern void *pAMXFunctions;

#define FORWARD(X) cell AMX_NATIVE_CALL X(AMX *amx, cell *params)

/* airport.c */
FORWARD(APT_FormatNearestList);
FORWARD(APT_FormatBeaconList);
FORWARD(APT_MapIndexFromListDialog);
FORWARD(APT_FormatInfo);
FORWARD(APT_FormatCodeAndName);
/* anticheat.c */
FORWARD(Ac_FormatLog);
/* commands.c */
FORWARD(Command_Is);
FORWARD(Command_GetIntParam);
FORWARD(Command_GetPlayerParam);
FORWARD(Command_GetStringParam);
/* dialog.c */
FORWARD(Dialog_EndTransaction);
FORWARD(Dialog_EnsureTransaction);
FORWARD(Dialog_ShowPlayerDialog);
/* echo.c */
FORWARD(Echo_Init);
/* game_sa.c */
FORWARD(Game_IsAirVehicle);
FORWARD(Game_IsHelicopter);
FORWARD(Game_IsPlane);
/* login.c */
FORWARD(Login_FormatAddFailedLogin);
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
FORWARD(Login_FormatUpdateFalng);
FORWARD(Login_FormatUpgradeGuestAcc);
FORWARD(Login_FreePassword);
FORWARD(Login_GetPassword);
FORWARD(Login_PasswordConfirmFree);
FORWARD(Login_PasswordConfirmStore);
FORWARD(Login_PasswordConfirmValidate);
FORWARD(Login_UsePassword);
/* missions.c */
FORWARD(Missions_AddPoint);
FORWARD(Missions_Create);
FORWARD(Missions_CreateTrackerMessage);
FORWARD(Missions_EndUnfinished);
FORWARD(Missions_EnterCheckpoint);
FORWARD(Missions_FinalizeAddPoints);
FORWARD(Missions_GetMissionNavData);
FORWARD(Missions_GetState);
FORWARD(Missions_OnVehicleRefueled);
FORWARD(Missions_OnVehicleRepaired);
FORWARD(Missions_OnWeatherChanged);
FORWARD(Missions_PostLoad);
FORWARD(Missions_PostUnload);
FORWARD(Missions_ShouldShowSatisfaction);
FORWARD(Missions_Start);
FORWARD(Missions_UpdateSatisfaction);
/* nav.c */
FORWARD(Nav_GetActiveNavType);
FORWARD(Nav_Format);
FORWARD(Nav_NavigateToMission);
FORWARD(Nav_Reset);
FORWARD(Nav_ResetCache);
FORWARD(Nav_Update);
/* panel.c */
FORWARD(Panel_FormatAltitude);
FORWARD(Panel_FormatHeading);
FORWARD(Panel_FormatSpeed);
FORWARD(Panel_ResetCaches);
/* playerdata.c */
FORWARD(PlayerData_Clear);
FORWARD(PlayerData_FormatUpdateQuery);
FORWARD(PlayerData_SetUserId);
FORWARD(PlayerData_UpdateGroup);
FORWARD(PlayerData_UpdateName);
/* playtime.c */
FORWARD(Playtime_FormatUpdateTimes);
/* timecyc.c */
FORWARD(Timecyc_GetCurrentWeatherMsg);
FORWARD(Timecyc_GetNextWeatherMsgQuery);
/* various.c */
FORWARD(Urlencode);
/* vehicles.c */
FORWARD(Veh_Add);
FORWARD(Veh_AddOdo);
FORWARD(Veh_AddServicePoint);
FORWARD(Veh_CollectSpawnedVehicles);
FORWARD(Veh_CollectPlayerVehicles);
FORWARD(Veh_ConsumeFuel);
FORWARD(Veh_Destroy);
FORWARD(Veh_FormatPanelText);
FORWARD(Veh_GetLabelToDelete);
FORWARD(Veh_GetNextUpdateQuery);
FORWARD(Veh_Init);
FORWARD(Veh_InitServicePoints);
FORWARD(Veh_IsFuelEmpty);
FORWARD(Veh_IsPlayerAllowedInVehicle);
FORWARD(Veh_OnPlayerDisconnect);
FORWARD(Veh_Refuel);
FORWARD(Veh_RegisterLabel);
FORWARD(Veh_Repair);
FORWARD(Veh_ResetPanelTextCache);
FORWARD(Veh_ShouldCreateLabel);
FORWARD(Veh_UpdateServicePointTextId);
FORWARD(Veh_UpdateServicePtsVisibility);
FORWARD(Veh_UpdateSlot);

static AMX *gamemode_amx = NULL;

#define IN_BASDONFLY
#include "basdon.c"

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT int PLUGIN_CALL Load(void **ppData)
{
	void game_sa_init(), login_init(), zones_init();
	void nav_init(), missions_init(), veh_init(), cmd_init();

	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t) ppData[PLUGIN_DATA_LOGPRINTF];

	cmd_init();
	game_sa_init();
	login_init();
	pdata_init();
	zones_init();
	nav_init();
	missions_init();
	veh_init();

	return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
}

static int time_h = 0, time_m = 0;

#define amx gamemode_amx
PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
	void echo_init(AMX*);
	void dev_missions_update_closest_point(AMX*);
	void dialog_pop_queue(AMX*);
	void maps_process_tick(AMX*);
	void protips_timed_broadcast(AMX*);
	void zones_update_for_all(AMX*);

	static int count = 0;
#ifdef LOG_SLOW_TICKS
	static int tickcount = 0;
#endif /*LOG_SLOW_TICKS*/
	static int lasttime = 0;

	/*occurs when doing gmx*/
	if (gamemode_amx == NULL) {
		count = 0;
		lasttime = 0;
		return;
	}

	if (count++ >= 19) {
		count = 0;

#ifdef LOG_SLOW_TICKS
		NC_tickcount();
		if (tickcount && nc_result - tickcount > 120) {
			logprintf("slow 20 ticks %d", nc_result - tickcount);
		}
		tickcount = nc_result;
#endif /*LOG_SLOW_TICKS*/

		/*timer100*/
		maps_process_tick(amx);
#ifdef DEV
		dev_missions_update_closest_point(amx);
#endif /*DEV*/

		NC_gettime(buf32a, buf32_1a, buf144a);
		if (lasttime != nc_result) {
			lasttime = nc_result;
			if (time_m++ == 30) {
				goto timer30s;
			} else if (time_m >= 60) {
				time_m = 0;
				if (time_h++ >= 24) {
					time_h = 0;
				}
				/*timer1m*/
				echo_init(amx);
				protips_timed_broadcast(amx);
timer30s:			/*timer30s*/
				;
			}
			/*timer1000*/
			zones_update_for_all(amx);
			/*TODO: on 5s?*/
			dialog_pop_queue(amx);
		}
	}
}
#undef amx

cell AMX_NATIVE_CALL REMOVEME_isspawned(AMX *amx, cell *params)
{
	return spawned[params[1]];
}

cell AMX_NATIVE_CALL REMOVEME_onplayerreqclassimpl(AMX *amx, cell *params)
{
	void class_on_player_request_class(AMX*,int,int);
	class_on_player_request_class(amx,params[1],params[2]);
	return 1;
}

cell AMX_NATIVE_CALL REMOVEME_setprefs(AMX *amx, cell *params)
{
	prefs[params[1]] = params[2];
	return 1;
}

cell AMX_NATIVE_CALL REMOVEME_getprefs(AMX *amx, cell *params)
{
	return prefs[params[1]];
}

#define REGISTERNATIVE(X) {#X, X}
AMX_NATIVE_INFO PluginNatives[] =
{
	REGISTERNATIVE(REMOVEME_isspawned),
	REGISTERNATIVE(REMOVEME_onplayerreqclassimpl),
	REGISTERNATIVE(REMOVEME_setprefs),
	REGISTERNATIVE(REMOVEME_getprefs),
	/* airport.c */
	REGISTERNATIVE(APT_FormatNearestList),
	REGISTERNATIVE(APT_FormatBeaconList),
	REGISTERNATIVE(APT_MapIndexFromListDialog),
	REGISTERNATIVE(APT_FormatInfo),
	REGISTERNATIVE(APT_FormatCodeAndName),
	/* anticheat.c */
	REGISTERNATIVE(Ac_FormatLog),
	/* basdon.c */
	REGISTERNATIVE(B_OnDialogResponse),
	REGISTERNATIVE(B_OnGameModeExit),
	REGISTERNATIVE(B_OnGameModeInit),
	REGISTERNATIVE(B_OnPlayerCommandText),
	REGISTERNATIVE(B_OnPlayerConnect),
	REGISTERNATIVE(B_OnPlayerDeath),
	REGISTERNATIVE(B_OnPlayerDisconnect),
	REGISTERNATIVE(B_OnPlayerRequestClass),
	REGISTERNATIVE(B_OnPlayerRequestSpawn),
	REGISTERNATIVE(B_OnPlayerSpawn),
	REGISTERNATIVE(B_OnPlayerText),
	REGISTERNATIVE(B_OnRecv),
	REGISTERNATIVE(B_OnVehicleSpawn),
	REGISTERNATIVE(B_Validate),
	/* commands.c */
	REGISTERNATIVE(Command_Is),
	REGISTERNATIVE(Command_GetIntParam),
	REGISTERNATIVE(Command_GetPlayerParam),
	REGISTERNATIVE(Command_GetStringParam),
	/* dialog.c */
	REGISTERNATIVE(Dialog_EndTransaction),
	REGISTERNATIVE(Dialog_EnsureTransaction),
	REGISTERNATIVE(Dialog_ShowPlayerDialog),
	/* game_sa.c */
	REGISTERNATIVE(Game_IsAirVehicle),
	REGISTERNATIVE(Game_IsHelicopter),
	REGISTERNATIVE(Game_IsPlane),
	/* login.c */
	REGISTERNATIVE(Login_FormatAddFailedLogin),
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
	REGISTERNATIVE(Login_FormatUpdateFalng),
	REGISTERNATIVE(Login_FormatUpgradeGuestAcc),
	REGISTERNATIVE(Login_FreePassword),
	REGISTERNATIVE(Login_GetPassword),
	REGISTERNATIVE(Login_PasswordConfirmFree),
	REGISTERNATIVE(Login_PasswordConfirmStore),
	REGISTERNATIVE(Login_PasswordConfirmValidate),
	REGISTERNATIVE(Login_UsePassword),
	/* missions.c */
	REGISTERNATIVE(Missions_AddPoint),
	REGISTERNATIVE(Missions_Create),
	REGISTERNATIVE(Missions_CreateTrackerMessage),
	REGISTERNATIVE(Missions_EndUnfinished),
	REGISTERNATIVE(Missions_EnterCheckpoint),
	REGISTERNATIVE(Missions_FinalizeAddPoints),
	REGISTERNATIVE(Missions_GetMissionNavData),
	REGISTERNATIVE(Missions_GetState),
	REGISTERNATIVE(Missions_OnVehicleRefueled),
	REGISTERNATIVE(Missions_OnVehicleRepaired),
	REGISTERNATIVE(Missions_OnWeatherChanged),
	REGISTERNATIVE(Missions_PostLoad),
	REGISTERNATIVE(Missions_PostUnload),
	REGISTERNATIVE(Missions_ShouldShowSatisfaction),
	REGISTERNATIVE(Missions_Start),
	REGISTERNATIVE(Missions_UpdateSatisfaction),
	/* nav.c */
	REGISTERNATIVE(Nav_GetActiveNavType),
	REGISTERNATIVE(Nav_Format),
	REGISTERNATIVE(Nav_NavigateToMission),
	REGISTERNATIVE(Nav_Reset),
	REGISTERNATIVE(Nav_ResetCache),
	REGISTERNATIVE(Nav_Update),
	/* panel.c */
	REGISTERNATIVE(Panel_FormatAltitude),
	REGISTERNATIVE(Panel_FormatHeading),
	REGISTERNATIVE(Panel_FormatSpeed),
	REGISTERNATIVE(Panel_ResetCaches),
	/* playerdata.c */
	REGISTERNATIVE(PlayerData_Clear),
	REGISTERNATIVE(PlayerData_FormatUpdateQuery),
	REGISTERNATIVE(PlayerData_SetUserId),
	REGISTERNATIVE(PlayerData_UpdateGroup),
	REGISTERNATIVE(PlayerData_UpdateName),
	/* playtime.c */
	REGISTERNATIVE(Playtime_FormatUpdateTimes),
	/* timecyc.c */
	REGISTERNATIVE(Timecyc_GetCurrentWeatherMsg),
	REGISTERNATIVE(Timecyc_GetNextWeatherMsgQuery),
	/* various.c */
	REGISTERNATIVE(Urlencode),
	/* vehicles.c */
	REGISTERNATIVE(Veh_Add),
	REGISTERNATIVE(Veh_AddOdo),
	REGISTERNATIVE(Veh_AddServicePoint),
	REGISTERNATIVE(Veh_CollectSpawnedVehicles),
	REGISTERNATIVE(Veh_CollectPlayerVehicles),
	REGISTERNATIVE(Veh_ConsumeFuel),
	REGISTERNATIVE(Veh_Destroy),
	REGISTERNATIVE(Veh_FormatPanelText),
	REGISTERNATIVE(Veh_GetLabelToDelete),
	REGISTERNATIVE(Veh_GetNextUpdateQuery),
	REGISTERNATIVE(Veh_Init),
	REGISTERNATIVE(Veh_InitServicePoints),
	REGISTERNATIVE(Veh_IsFuelEmpty),
	REGISTERNATIVE(Veh_IsPlayerAllowedInVehicle),
	REGISTERNATIVE(Veh_OnPlayerDisconnect),
	REGISTERNATIVE(Veh_Refuel),
	REGISTERNATIVE(Veh_RegisterLabel),
	REGISTERNATIVE(Veh_Repair),
	REGISTERNATIVE(Veh_ResetPanelTextCache),
	REGISTERNATIVE(Veh_ShouldCreateLabel),
	REGISTERNATIVE(Veh_UpdateServicePointTextId),
	REGISTERNATIVE(Veh_UpdateServicePtsVisibility),
	REGISTERNATIVE(Veh_UpdateSlot),
	{0, 0}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
	return amx_Register(amx, PluginNatives, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx)
{
	if (amx == gamemode_amx) {
		gamemode_amx = NULL;
		/*Most likely doing gmx for update, so the server needs
		to restart (exit and start) to copy and load the new plugin.*/
		buf32[0] = 'e'; buf32[1] = 'x'; buf32[2] = 'i'; buf32[3] = 't';
		buf32[4] = 0;
		NC_SendRconCommand(buf32a);
		/*TODO this actually crashes the server somewhere. Although the
		result is the same, it would be nice if it didn't crash :)*/
	}
	return AMX_ERR_NONE;
}

