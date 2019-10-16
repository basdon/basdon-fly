
/* vim: set filetype=c ts=8 noexpandtab: */

#define _CRT_SECURE_NO_DEPRECATE
#include "common.h"
#include "dialog.h"
#include <string.h>

static struct DIALOGDATA {
	short dialogid;
	char style;
	char caption[LIMIT_DIALOG_CAPTION];
	char info[LIMIT_DIALOG_INFO];
	char button1[LIMIT_DIALOG_BUTTON];
	char button2[LIMIT_DIALOG_BUTTON];
	short transactionid;
	struct DIALOGDATA *next;
} *dialogqueue[MAX_PLAYERS];

static int transaction[MAX_PLAYERS];
/**
To check if the next OnDialogResponse is actually a response to the last dialog
we've showed.
*/
static int showndialog[MAX_PLAYERS];

void dialog_on_player_connect(AMX *amx, int playerid)
{
	dialogqueue[playerid] = NULL;
	transaction[playerid] = 0;
	showndialog[playerid] = 0;
	/*to hide any open dialogs, send with any negative id*/
	nc_params[0] = 7;
	nc_params[1] = playerid;
	nc_params[2] = -1;
	nc_params[3] = DIALOG_STYLE_MSGBOX;
	nc_params[4] = emptystringa;
	nc_params[5] = emptystringa;
	nc_params[6] = emptystringa;
	nc_params[7] = emptystringa;
	NC(n_ShowPlayerDialog);
}

void dialog_on_player_disconnect(AMX *amx, int playerid)
{
	struct DIALOGDATA *next;
	while (dialogqueue[playerid] != NULL) {
		next = dialogqueue[playerid]->next;
		free(dialogqueue[playerid]);
		dialogqueue[playerid] = next;
	}
}

/**
@return 0 when the response should be dropped
*/
int dialog_on_response(AMX *amx, int playerid, int dialogid)
{
	int ret = 1;
	transaction[playerid] = TRANSACTION_NONE;
	if (dialogid != showndialog[playerid]) {
		// TODO: aclog
		//format buf144, sizeof(buf144), "unexpected dialog response "
		//"%d expected %d", dialogid, showndialog[playerid]
		//ac_log playerid, buf144
		ret = 0;
	}
	showndialog[playerid] = 0;
	return ret;
}

void dialog_end_transaction(int playerid, int transactionid)
{
	if (transaction[playerid] != transactionid) {
		logprintf(
			"dialog_end_transaction: not ending %d while %d "
			"is active", transactionid, transaction[playerid]);
		return;
	}
	transaction[playerid] = transactionid;
}

void dialog_ensure_transaction(int playerid, int transactionid)
{
	if (transaction[playerid] && transaction[playerid] != transactionid) {
		logprintf(
			"dialog_ensure_transaction: ignored %d while %d "
			"is active", transactionid, transaction[playerid]);
		return;
	}
	transaction[playerid] = transactionid;
}

void dialog_queue(
	int playerid, int dialogid, int style,
	char *caption, char *info,
	char *button1, char *button2, int transactionid)
{
	struct DIALOGDATA *data, *prev;

	data = malloc(sizeof(struct DIALOGDATA));
	data->dialogid = dialogid;
	data->style = style;
	strcpy(data->caption, caption);
	strcpy(data->info, info);
	strcpy(data->button1, button1);
	strcpy(data->button2, button2);
	data->transactionid = transactionid;
	data->next = NULL;
	
	if (dialogqueue[playerid] == NULL) {
		dialogqueue[playerid] = data;
		return;
	}
	
	prev = dialogqueue[playerid];
	while (prev->next != NULL) {
		prev = prev->next;
	}
	prev->next = data;
}

int dialog_NC_ShowPlayerDialog(
	AMX *amx, int playerid, int dialogid, int style,
	char *caption, char *info,
	char *button1, char *button2, int transactionid)
{
	if (transactionid == -1) {
		transactionid = dialogid;
	}

	if (transaction[playerid] && transaction[playerid] != transactionid) {
		if (transactionid != TRANSACTION_OVERRIDE) {
			dialog_queue(
				playerid, dialogid, style, caption, info,
				button1, button2, transactionid);
			return 0;
		}
		logprintf("dialog: overriding transaction %d for %d",
			transaction[playerid], transactionid);
	}
	transaction[playerid] = transactionid;

	showndialog[playerid] = dialogid;
	amx_SetUString(buf64, caption, LIMIT_DIALOG_CAPTION);
	amx_SetUString(buf4096, info, LIMIT_DIALOG_INFO);
	amx_SetUString(buf32, button1, LIMIT_DIALOG_BUTTON);
	nc_params[0] = 7;
	nc_params[1] = playerid;
	nc_params[2] = dialogid;
	nc_params[3] = style;
	nc_params[4] = buf64a;
	nc_params[5] = buf4096a;
	nc_params[6] = buf32a;
	if (button2) {
		amx_SetUString(buf32_1, button2, LIMIT_DIALOG_BUTTON);
		nc_params[7] = buf32_1a;
	} else {
		nc_params[7] = emptystringa;
	}
	NC(n_ShowPlayerDialog);
	return nc_result;
}

void dialog_pop_queue(AMX *amx)
{
	struct DIALOGDATA *q;
	int playerid, i;

	for (i = 0; i < playercount; i++) {
		playerid = players[i];
		if ((q = dialogqueue[playerid]) != NULL) {
			dialogqueue[playerid] = q->next;
			dialog_NC_ShowPlayerDialog(
				amx, playerid, q->dialogid, q->style,
				q->caption, q->info, q->button1, q->button2,
				q->transactionid);
			free(q);
		}
	}
}

/**to be removed*/
cell AMX_NATIVE_CALL Dialog_ShowPlayerDialog(AMX *amx, cell *params)
{
	char *b4096, b64[64], b32[32], b32_1[32];
	cell *addr;
	b4096 = malloc(4096);
	amx_GetAddr(amx, params[4], &addr);
	amx_GetUString(b64, addr, 64);
	amx_GetAddr(amx, params[5], &addr);
	amx_GetUString(b4096, addr, 4096);
	amx_GetAddr(amx, params[6], &addr);
	amx_GetUString(b32, addr, 32);
	amx_GetAddr(amx, params[7], &addr);
	amx_GetUString(b32_1, addr, 32);
	*addr = dialog_NC_ShowPlayerDialog(
		amx, params[1], params[2], params[3],
		b64, b4096, b32, b32_1,
		params[0] < 8 ? -1 : params[8]);
	free(b4096);
	return *addr;
}

/**to be removed*/
cell AMX_NATIVE_CALL Dialog_EnsureTransaction(AMX *amx, cell *params)
{
	dialog_ensure_transaction(params[1], params[2]);
	return 1;
}

/**to be removed*/
cell AMX_NATIVE_CALL Dialog_EndTransaction(AMX *amx, cell *params)
{
	dialog_end_transaction(params[1], params[2]);
	return 1;
}
