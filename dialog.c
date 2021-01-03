static struct DIALOGDATA {
	short dialogid;
	char style;
	char caption[LIMIT_DIALOG_CAPTION];
	char info[LIMIT_DIALOG_INFO];
	char button1[LIMIT_DIALOG_BUTTON];
	char button2[LIMIT_DIALOG_BUTTON];
	short transactionid;
	struct DIALOGDATA *next;
} *dialog_queue[MAX_PLAYERS];

/**
Dialog transaction per player.
*/
static int dialog_transaction[MAX_PLAYERS];
/**
To check if the next OnDialogResponse is actually a response to the last dialog
we've showed.
*/
static int showndialog[MAX_PLAYERS];

static
void dialog_on_player_connect(int playerid)
{
	dialog_queue[playerid] = NULL;
	dialog_transaction[playerid] = TRANSACTION_NONE;
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

static
void dialog_on_player_disconnect(int playerid)
{
	struct DIALOGDATA *next;
	while (dialog_queue[playerid] != NULL) {
		next = dialog_queue[playerid]->next;
		free(dialog_queue[playerid]);
		dialog_queue[playerid] = next;
	}
}

/**
@return 0 when the response should be dropped
*/
static
int dialog_on_response(int playerid, int dialogid)
{
	int ret = 1;
	dialog_transaction[playerid] = TRANSACTION_NONE;
	if (dialogid != showndialog[playerid]) {
#ifndef DEV
		sprintf(cbuf144, "expected %d got %d", showndialog[playerid], dialogid);
		anticheat_log(playerid, AC_WRONG_DIALOGID, cbuf144);
#endif
		ret = 0;
	}
	showndialog[playerid] = 0;
	return ret;
}

/**
Ends dialog transaction for player if it matches given transactionid and no
dialogs are shown.

If the player has a different transaction going, it will not be
ended and a warning will be logged.
*/
static
void dialog_end_transaction(int playerid, int transactionid)
{
	if (dialog_transaction[playerid] && dialog_transaction[playerid] != transactionid) {
		logprintf("dialog_end_transaction: not ending %d while %d is active", transactionid, dialog_transaction[playerid]);
		return;
	}
	if (!showndialog[playerid]) {
		dialog_transaction[playerid] = TRANSACTION_NONE;
	}
}

/**
Ensures the current transactionid for given player.

If the player already has a different transaction going, it will not be
overridden and a warning will be logged.
*/
static
void dialog_ensure_transaction(int playerid, int transactionid)
{
	if (dialog_transaction[playerid] && dialog_transaction[playerid] != transactionid) {
		logprintf("dialog_ensure_transaction: ignored %d while %d is active", transactionid, dialog_transaction[playerid]);
		return;
	}
	dialog_transaction[playerid] = transactionid;
}

/**
Append a dialog to a player's dialog queue.
*/
static
void dialog_append_to_queue(
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
	
	if (dialog_queue[playerid] == NULL) {
		dialog_queue[playerid] = data;
		return;
	}
	
	prev = dialog_queue[playerid];
	while (prev->next != NULL) {
		prev = prev->next;
	}
	prev->next = data;
}

/**
Shows the player a synchronous (only one at a time) dialog box.

Uses buf4096 for info, buf64 for caption, buf32 and buf32_1 for buttons.
buf32_1 is not used if button2 is NULL.

Transactions are used to make sure open dialogs do not disappear when calling.
If a transaction is already running, the dialog to open gets added to a queue
which is being polled and will be shown once no transaction is active anymore.

The transaction is reset in OnDialogResponse, use dialog_ensure_transaction
in your dialog's response code to keep the transaction set (eg when doing a
db query and not wanting any other dialog shown while the query is executing).

DIALOG_DUMMY can be used as dialog id if no response is handled.

@param style see DIALOG_STYLE_ constants
@param playerid The ID of the player to show the dialog to
@param dialogid An ID to assign this dialog to, so responses can be processed.
                Max dialogid is 32767. Using negative values will close any
		open dialog
@param style The style of the dialog
@param caption The title at the top of the dialog.
               The length of the caption can not exceed more than 64 characters
	       before it starts to cut off
@param info The text to display in the main dialog. Use \n to start a new line
            and \t to tabulate
@param button1 The text on the left button
@param button2 The text on the right button. Leave it blank ("") to hide it
@param transactionid Transaction id this dialog belongs to. Use -1 to use the
                     dialog id as transacton id. TRANSACTION_OVERRIDE can be
		     used to override any active transactions (cases will be
		     logged to console). TRANSACTION_NONE can be used to not
		     case about it being overridden.
@returns 0 on failure, player is not connected. NOTE: also returns 1 if the
         dialog was queued.
*/
static
int dialog_ShowPlayerDialog(
	int playerid, int dialogid, int style,
	char *caption, char *info,
	char *button1, char *button2, int transactionid)
{
	if (transactionid == -1) {
		transactionid = dialogid;
	}

	if (dialog_transaction[playerid] && dialog_transaction[playerid] != transactionid) {
		if (transactionid != TRANSACTION_OVERRIDE) {
			dialog_append_to_queue(playerid, dialogid, style, caption, info, button1, button2, transactionid);
			return 0;
		}
		logprintf("dialog: overriding transaction %d for %d",
			dialog_transaction[playerid], transactionid);
	}
	dialog_transaction[playerid] = transactionid;

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

/**
Checks if any dialog is queued for the player and show it if so.
*/
static
void dialog_pop_queue(int playerid)
{
	struct DIALOGDATA *q;

	q = dialog_queue[playerid];
	if (q && !showndialog[playerid] && (!dialog_transaction[playerid] || dialog_transaction[playerid] == q->transactionid)) {
		dialog_queue[playerid] = q->next;
		dialog_ShowPlayerDialog(playerid, q->dialogid, q->style, q->caption, q->info, q->button1, q->button2, q->transactionid);
		free(q);
	}
}
