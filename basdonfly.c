
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "airport.h"
#include "class.h"
#include "anticheat.h"
#include "dialog.h"
#include "echo.h"
#include "game_sa.h"
#include "heartbeat.h"
#include "login.h"
#include "maps.h"
#include "missions.h"
#include "money.h"
#include "nav.h"
#include "objects.h"
#include "panel.h"
#include "playerdata.h"
#include "playtime.h"
#include "pm.h"
#include "prefs.h"
#include "protips.h"
#include "servicepoints.h"
#include "score.h"
#include "spawn.h"
#include "timecyc.h"
#include "timer.h"
#include "vehicles.h"
#include "zones.h"
#include "time/time.h"
#include <stdio.h>
#include <string.h>

logprintf_t logprintf;
extern void *pAMXFunctions;

EXPECT_SIZE(char, 1);
EXPECT_SIZE(short, 2);
EXPECT_SIZE(int, 4);
EXPECT_SIZE(cell, 4);
EXPECT_SIZE(float, 4);

#define FORWARD(X) cell AMX_NATIVE_CALL X(AMX *amx, cell *params)

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
/* nav.c */
FORWARD(Nav_Reset);
/* playerdata.c */
FORWARD(PlayerData_FormatUpdateQuery);
FORWARD(PlayerData_SetUserId);
FORWARD(PlayerData_UpdateGroup);

int temp_afk[MAX_PLAYERS];
int _cc[MAX_PLAYERS];

#define IN_BASDONFLY
#include "basdon.c"

#ifndef PRECOMPILED_TIME
#include "time/time.c"
#endif /*PRECOMPILED_TIME*/

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT int PLUGIN_CALL Load(void **ppData)
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t) ppData[PLUGIN_DATA_LOGPRINTF];

	cmd_init();
	game_sa_init();
	login_init();
	nav_init();
	pdata_init();
	time_init();
	zones_init();

	return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
	static int count = 0;
#ifdef LOG_SLOW_TICKS
	static int tickcount = 0;
	int newtickcount;
#endif /*LOG_SLOW_TICKS*/

	/*occurs when doing gmx*/
	if (amx == NULL) {
		count = 0;
		return;
	}

	timer_tick();

	if (count++ >= 19) {
		count = 0;

#ifdef LOG_SLOW_TICKS
		newtickcount = NC_tickcount();
		if (tickcount && newtickcount - tickcount > 120) {
			logprintf("slow 20 ticks %d", newtickcount - tickcount);
		}
		tickcount = newtickcount;
#endif /*LOG_SLOW_TICKS*/

		/*timer100*/
		anticheat_decrease_flood();
		maps_process_tick();
#ifdef DEV
		dev_missions_update_closest_point();
#endif /*DEV*/
		panel_timed_update();
		playtime_check_for_afk();

		timecyc_tick();
	}
}
#undef amx

static
cell AMX_NATIVE_CALL REMOVEME_onplayerreqclassimpl(AMX *amx, cell *params)
{
	class_on_player_request_class(params[1],params[2]);
	return 1;
}

static
cell AMX_NATIVE_CALL REMOVEME_setprefs(AMX *amx, cell *params)
{
	prefs[params[1]] = params[2];
	return 1;
}

static
cell AMX_NATIVE_CALL REMOVEME_getprefs(AMX *amx, cell *params)
{
	return prefs[params[1]];
}

static
cell AMX_NATIVE_CALL REMOVEME_setloggedstatus(AMX *amx, cell *params)
{
	loggedstatus[params[1]] = params[2];
	return 1;
}

extern float playerodoKM[MAX_PLAYERS];
cell AMX_NATIVE_CALL REMOVEME_getplayerodo(AMX *amx, cell *params)
{
	return amx_ftoc(playerodoKM[params[1]]);
}

static
cell AMX_NATIVE_CALL REMOVEME_setplayermoney(AMX *amx, cell *params)
{
	money_set(params[1], params[2]);
	return 1;
}

static
cell AMX_NATIVE_CALL REMOVEME_getplayermoney(AMX *amx, cell *params)
{
	return money_get(params[1]);
}

static
cell AMX_NATIVE_CALL REMOVEME_getflighttime(AMX *amx, cell *params)
{
	return score_flight_time[params[1]];
}

static
cell AMX_NATIVE_CALL REMOVEME_setflighttime(AMX *amx, cell *params)
{
	score_flight_time[params[1]] = params[2];
	return 1;
}

static
cell AMX_NATIVE_CALL REMOVEME_setsessionid(AMX *amx, cell *params)
{
	sessionid[params[1]] = params[2];
	return 1;
}

static
cell AMX_NATIVE_CALL REMOVEME_onplayerlogin(AMX *amx, cell *params)
{
	veh_spawn_player_vehicles(params[1]);
	return 1;
}

static
cell REMOVEME_setplayerodo(AMX *amx, cell *params)
{
	playerodoKM[params[1]] = amx_ctof(params[2]);
	return 1;
}

static
cell REMOVEME_setplayername(AMX *amx, cell *params)
{
	cell *addr;
	char name[MAX_PLAYER_NAME + 1];
	int playerid = params[1];
	amx_GetAddr(amx, params[2], &addr);
	amx_GetUString(name, addr, MAX_PLAYER_NAME + 1);
	return natives_SetPlayerName(playerid, name);
}

#define REGISTERNATIVE(X) {#X, X}
AMX_NATIVE_INFO PluginNatives[] =
{
	REGISTERNATIVE(REMOVEME_setplayername),
	REGISTERNATIVE(REMOVEME_setplayerodo),
	REGISTERNATIVE(REMOVEME_onplayerlogin),
	REGISTERNATIVE(REMOVEME_setsessionid),
	REGISTERNATIVE(REMOVEME_onplayerreqclassimpl),
	REGISTERNATIVE(REMOVEME_setprefs),
	REGISTERNATIVE(REMOVEME_getprefs),
	REGISTERNATIVE(REMOVEME_setloggedstatus),
	REGISTERNATIVE(REMOVEME_getplayerodo),
	REGISTERNATIVE(REMOVEME_setplayermoney),
	REGISTERNATIVE(REMOVEME_getplayermoney),
	REGISTERNATIVE(REMOVEME_getflighttime),
	REGISTERNATIVE(REMOVEME_setflighttime),
	/* anticheat.c */
	REGISTERNATIVE(Ac_FormatLog),
	/* basdon.c */
	REGISTERNATIVE(B_OnDialogResponse),
	REGISTERNATIVE(B_OnGameModeExit),
	REGISTERNATIVE(B_OnGameModeInit),
	REGISTERNATIVE(B_OnMysqlResponse),
	REGISTERNATIVE(B_OnPlayerCommandText),
	REGISTERNATIVE(B_OnPlayerConnect),
	REGISTERNATIVE(B_OnPlayerDeath),
	REGISTERNATIVE(B_OnPlayerDisconnect),
	REGISTERNATIVE(B_OnPlayerEnterRaceCP),
	REGISTERNATIVE(B_OnPlayerEnterVehicle),
	REGISTERNATIVE(B_OnPlayerKeyStateChange),
	REGISTERNATIVE(B_OnPlayerRequestClass),
	REGISTERNATIVE(B_OnPlayerRequestSpawn),
	REGISTERNATIVE(B_OnPlayerSpawn),
	REGISTERNATIVE(B_OnPlayerStateChange),
	REGISTERNATIVE(B_OnPlayerText),
	REGISTERNATIVE(B_OnPlayerUpdate),
	REGISTERNATIVE(B_OnRecv),
	REGISTERNATIVE(B_OnVehicleSpawn),
	REGISTERNATIVE(B_OnVehicleStreamIn),
	REGISTERNATIVE(B_OnVehicleStreamOut),
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
	/* nav.c */
	REGISTERNATIVE(Nav_Reset),
	/* playerdata.c */
	REGISTERNATIVE(PlayerData_FormatUpdateQuery),
	REGISTERNATIVE(PlayerData_SetUserId),
	REGISTERNATIVE(PlayerData_UpdateGroup),
	{0, 0}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
	return amx_Register(amx, PluginNatives, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *a)
{
	if (amx == a) {
		amx = NULL;
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

