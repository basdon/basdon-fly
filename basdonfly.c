
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"

logprintf_t logprintf;
extern void *pAMXFunctions;

#define FORWARD(X) cell AMX_NATIVE_CALL X(AMX *amx, cell *params)

/* airport.c */
FORWARD(APT_Init);
FORWARD(APT_Destroy);
FORWARD(APT_Add);
FORWARD(APT_AddRunway);
/* commands.c */
FORWARD(CommandHash);
FORWARD(IsCommand);
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
/* game_sa.c */
FORWARD(IsAirVehicle);
/* login.c */
FORWARD(ResetPasswordConfirmData);
FORWARD(SetPasswordConfirmData);
FORWARD(ValidatePasswordConfirmData);
FORWARD(SetLoginData);
FORWARD(ClearLoginData);
FORWARD(UpdateLoginData);
FORWARD(FormatLoginApiRegister);
FORWARD(FormatLoginApiLogin);
FORWARD(FormatLoginApiGuestRegister);
FORWARD(FormatLoginApiCheckChangePass);
FORWARD(FormatLoginApiUserExistsGuest);

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
	void game_sa_init(), login_init(), dialog_init();

	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t) ppData[PLUGIN_DATA_LOGPRINTF];

	game_sa_init();
	login_init();
	dialog_init();

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
	REGISTERNATIVE(CommandHash),
	REGISTERNATIVE(IsCommand),
	REGISTERNATIVE(QueueDialog),
	REGISTERNATIVE(DropDialogQueue),
	REGISTERNATIVE(HasDialogsInQueue),
	REGISTERNATIVE(PopDialogQueue),
	REGISTERNATIVE(SetLoginData),
	REGISTERNATIVE(ClearLoginData),
	REGISTERNATIVE(UpdateLoginData),
	REGISTERNATIVE(FormatLoginApiRegister),
	REGISTERNATIVE(FormatLoginApiLogin),
	REGISTERNATIVE(FormatLoginApiGuestRegister),
	REGISTERNATIVE(FormatLoginApiCheckChangePass),
	REGISTERNATIVE(FormatLoginApiUserExistsGuest),
	REGISTERNATIVE(APT_Init),
	REGISTERNATIVE(APT_Destroy),
	REGISTERNATIVE(APT_Add),
	REGISTERNATIVE(APT_AddRunway),
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

