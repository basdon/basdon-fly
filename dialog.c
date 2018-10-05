
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include <string.h>

static struct dialogdata {
	short dialogid;
	char style;
	char caption[LIMIT_DIALOG_CAPTION];
	char text[LIMIT_DIALOG_TEXT];
	char button1[LIMIT_DIALOG_BUTTON];
	char button2[LIMIT_DIALOG_BUTTON];
	short transactionid;
	struct dialogdata *next;
} *dialogqueue[MAX_PLAYERS];

void dialog_init()
{
	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		dialogqueue[i] = NULL;
	}
}

/* native QueueDialog(playerid, dialogid, style, caption[], info[], button1[], button2[], transactionid) */
cell AMX_NATIVE_CALL QueueDialog(AMX *amx, cell *params)
{
	int playerid = params[1];
	struct dialogdata *data, *prev;
	cell *inaddr = NULL;

	data = malloc(sizeof(struct dialogdata));
	data->dialogid = params[2];
	data->style = params[3];
	amx_GetAddr(amx, params[4], &inaddr);
	amx_GetUString(data->caption, inaddr, LIMIT_DIALOG_CAPTION);
	amx_GetAddr(amx, params[5], &inaddr);
	amx_GetUString(data->text, inaddr, LIMIT_DIALOG_TEXT);
	amx_GetAddr(amx, params[6], &inaddr);
	amx_GetUString(data->button1, inaddr, LIMIT_DIALOG_BUTTON);
	amx_GetAddr(amx, params[7], &inaddr);
	amx_GetUString(data->button2, inaddr, LIMIT_DIALOG_BUTTON);
	data->transactionid = params[8];
	data->next = NULL;
	
	if (dialogqueue[playerid] == NULL) {
		dialogqueue[playerid] = data;
		return 1;
	}
	
	prev = dialogqueue[playerid];
	while (prev->next != NULL) {
		prev = prev->next;
	}
	prev->next = data;

	return 1;
}

cell AMX_NATIVE_CALL DropDialogQueue(AMX *amx, cell *params)
{
	struct dialogdata *next;
	int playerid = params[1];
	while (dialogqueue[playerid] != NULL) {
		next = dialogqueue[playerid]->next;
		free(dialogqueue[playerid]);
		dialogqueue[playerid] = next;
	}
	return 1;
}

cell AMX_NATIVE_CALL IsDialogQueueEmpty(AMX *amx, cell *params)
{
	return dialogqueue[params[1]] == NULL;
}
