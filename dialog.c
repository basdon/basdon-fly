
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "anticheat.h"
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

int dialog_current_transaction(int playerid)
{
	return transaction[playerid];
}

void dialog_on_player_connect(int playerid)
{
	dialogqueue[playerid] = NULL;
	transaction[playerid] = 0;
	showndialog[playerid] = 0;
	/*to hide any open dialogs, send with any negative id*/
	NC_PARS(7);
	nc_params[1] = playerid;
	nc_params[2] = -1;
	nc_params[3] = DIALOG_STYLE_MSGBOX;
	nc_params[4] = emptystringa;
	nc_params[5] = emptystringa;
	nc_params[6] = emptystringa;
	nc_params[7] = emptystringa;
	NC(n_ShowPlayerDialog_);
}

void dialog_on_player_disconnect(int playerid)
{
	struct DIALOGDATA *next;
	while (dialogqueue[playerid] != NULL) {
		next = dialogqueue[playerid]->next;
		free(dialogqueue[playerid]);
		dialogqueue[playerid] = next;
	}
}

int dialog_on_response(int playerid, int dialogid)
{
	int ret = 1;
	transaction[playerid] = TRANSACTION_NONE;
	if (dialogid != showndialog[playerid]) {
#ifndef DEV
		sprintf(cbuf144,
			"expected %d got %d",
			showndialog[playerid],
			dialogid);
		anticheat_log(playerid, AC_WRONG_DIALOGID, cbuf144);
#endif
		ret = 0;
	}
	showndialog[playerid] = 0;
	return ret;
}

void dialog_end_transaction(int playerid, int transactionid)
{
	if (transaction[playerid] && transaction[playerid] != transactionid) {
		logprintf(
			"dialog_end_transaction: not ending %d while %d "
			"is active", transactionid, transaction[playerid]);
		return;
	}
	if (!showndialog[playerid]) {
		transaction[playerid] = TRANSACTION_NONE;
	}
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

int dialog_ShowPlayerDialog(
	int playerid, int dialogid, int style,
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
	atoc(buf64, caption, LIMIT_DIALOG_CAPTION);
	atoc(buf4096, info, LIMIT_DIALOG_INFO);
	atoc(buf32, button1, LIMIT_DIALOG_BUTTON);
	NC_PARS(7);
	nc_params[1] = playerid;
	nc_params[2] = dialogid;
	nc_params[3] = style;
	nc_params[4] = buf64a;
	nc_params[5] = buf4096a;
	nc_params[6] = buf32a;
	if (button2) {
		atoc(buf32_1, button2, LIMIT_DIALOG_BUTTON);
		nc_params[7] = buf32_1a;
	} else {
		nc_params[7] = emptystringa;
	}
	return NC(n_ShowPlayerDialog_);
}

void dialog_pop_queue(int playerid)
{
	struct DIALOGDATA *q;

	if ((q = dialogqueue[playerid]) != NULL &&
		!showndialog[playerid] && (!transaction[playerid] ||
			transaction[playerid] == q->transactionid))
	{
		dialogqueue[playerid] = q->next;
		dialog_ShowPlayerDialog(
			playerid, q->dialogid, q->style,
			q->caption, q->info, q->button1, q->button2,
			q->transactionid);
		free(q);
	}
}
