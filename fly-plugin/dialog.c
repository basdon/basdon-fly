struct DIALOG_RESPONSE {
	/**Handlers will only be notified of aborted if the DLG_OPT_NOTIFY_ABORTED option was set.
	Response variable values will be undefined.*/
	char aborted;
	/**The data that was set on the handler when the dialog was shown.*/
	void *data;
	char response;
	short listitem;
	char *inputtext;
};

#define DLG_OPT_NO_SANITIZE_INPUTTEXT 1
#define DLG_OPT_NOTIFY_ABORTED 2

struct DIALOG_HANDLER {
	/**Combination of DIALOG_OPTION_* definitions.*/
	int options;
	/**Any data that needs to be passed into DIALOG_REPONSE.data.*/
	void *data;
	void (*callback)(int playerid, struct DIALOG_RESPONSE);
};

struct DIALOG_INFO {
	/**Transaction id this dialog belongs to.
	DLG_TID_NONE can be used to not care about this dialog being overridden.*/
	int transactionid;
	/**See DIALOG_STYLE_* constants.*/
	char style;
	/**The title at the top of the dialog.
	When empty, the dialog won't show.*/
	char *caption;
	/**The text to display in the main dialog.
	Use \n to start a new line and \t to tabulate.
	Lists: Completely empty lines are removed (so ending with \n is fine), use whitespace to keep them.
	Tablists: Empty columns (like \t\t) are ignored (except at the end), use whitespace.
	          The first row commands the amount of columns*/
	char *info;
	/**button1 The text on the left button.
	Pressing it will result in a non-zero response.
	Leaving it blank will show a button with no text.*/
	char *button1;
	/**button2 The text on the right button.
	Pressing it will result in a zero response, as does pressing esc.
	Leaving it blank will hide it, but the player can still press esc and respond negatively.*/
	char *button2;
	/**Handler info*/
	struct DIALOG_HANDLER handler;
	char caption_buf[LIMIT_DIALOG_CAPTION];
	char info_buf[LIMIT_DIALOG_INFO];
	char button1_buf[LIMIT_DIALOG_BUTTON];
	char button2_buf[LIMIT_DIALOG_BUTTON];
};

static struct DIALOG_QUEUE_ENTRY {
	struct DIALOG_INFO dialog;
	struct DIALOG_QUEUE_ENTRY *next;
} *dialog_queue[MAX_PLAYERS];

static struct DIALOG_DATA {
	/**The last used dialogid. Increment before showing a dialog.*/
	unsigned char last_dialogid;
	/**To check if the next OnDialogResponse is actually a response to the last dialog
	we've showed. SAMP already does this, but we may override some certain behavior.
	Negative value means no dialog is currently shown, so no response is expected.*/
	short expected_dialogid;
	/**Current transaction id the player is in. Showing a dialog can override the
	currently shown dialog if the transaction ids are the same. If not the same,
	the dialog to show will be queued and showed later.*/
	int transactionid;
	/**Style of the last sent dialog.*/
	char style;
	/**time_timestamp() of when the last dialog was sent to the player.*/
	int last_sendtime;
	/**Handler to use for the next correct dialog response.*/
	struct DIALOG_HANDLER handler;
} active_dialog[MAX_PLAYERS];

static struct {
	struct {
		short dialog_id;
		char dialog_style;
		char caption_length_0;
		char button1_length_0;
		char button2_length_0;
	} base;
	char info_empty[10];
	int bitlength;
} dialog_empty_rpcdata;

static
void dialog_init()
{
	TRACE;
	/*Using a negative dialogid will hide any shown dialog.*/
	dialog_empty_rpcdata.base.dialog_id = -1;
	dialog_empty_rpcdata.base.dialog_style = DIALOG_STYLE_MSGBOX;
	dialog_empty_rpcdata.base.caption_length_0 = 0;
	dialog_empty_rpcdata.base.button1_length_0 = 0;
	dialog_empty_rpcdata.base.button2_length_0 = 0;
	dialog_empty_rpcdata.bitlength = sizeof(dialog_empty_rpcdata.base);
	dialog_empty_rpcdata.bitlength += EncodeString(dialog_empty_rpcdata.info_empty, "", sizeof(dialog_empty_rpcdata.info_empty));
}

static
void dialog_reset_state(int playerid)
{
	TRACE;
	active_dialog[playerid].expected_dialogid = -1;
	active_dialog[playerid].transactionid = DLG_TID_NONE;
	active_dialog[playerid].handler.options = 0;
	active_dialog[playerid].handler.callback = NULL;
}

static
void dialog_hide_all(int playerid)
{
	TRACE;
	struct BitStream bs;

	dialog_reset_state(playerid);
	active_dialog[playerid].last_sendtime = time_timestamp();

	bs.ptrData = &dialog_empty_rpcdata;
	bs.numberOfBitsUsed = dialog_empty_rpcdata.bitlength;
	SAMP_SendRPCToPlayer(RPC_ShowDialog, &bs, playerid, 2);
}

static
void dialog_send_abort_result(int playerid, struct DIALOG_HANDLER *handler)
{
	TRACE;
	struct DIALOG_RESPONSE response;

	if (handler->options & DLG_OPT_NOTIFY_ABORTED) {
		response.aborted = 1;
		response.data = handler->data;
		handler->callback(playerid, response);
	}
}

static
void dialog_on_player_connect(int playerid)
{
	TRACE;
	dialog_queue[playerid] = NULL;
	active_dialog[playerid].last_dialogid = 1;
	active_dialog[playerid].last_sendtime = 0;

	dialog_reset_state(playerid); /*This is also done in the dialog_hide_all call.*/
	dialog_hide_all(playerid);
}

static
void dialog_on_player_disconnect(int playerid)
{
	TRACE;
	register struct DIALOG_QUEUE_ENTRY *current, *next;

	dialog_send_abort_result(playerid, &active_dialog[playerid].handler);

	current = dialog_queue[playerid];
	while (current) {
		dialog_send_abort_result(playerid, &current->dialog.handler);
		next = current->next;
		free(current);
		current = next;
	}

	dialog_queue[playerid] = NULL;
	active_dialog[playerid].handler.options = 0;
	active_dialog[playerid].handler.callback = NULL;
}

static
void dialog_init_info(struct DIALOG_INFO *info)
{
	TRACE;
	info->transactionid = DLG_TID_NONE;
	info->style = DIALOG_STYLE_MSGBOX;
	info->handler.options = 0;
	info->handler.callback = NULL;
	info->info_buf[0] = 0;
	info->caption_buf[0] = 0;
	info->button1_buf[0] = 0;
	info->button2_buf[0] = 0;
	info->caption = info->caption_buf;
	info->button1 = info->button1_buf;
	info->button2 = info->button2_buf;
	info->info = info->info_buf;
}

/**
Ensures the current transactionid for given player.

If the player already has a different transaction going, it will not be
overridden and a warning will be logged.
*/
static
void dialog_ensure_transaction(int playerid, int transactionid)
{
	TRACE;
	register struct DIALOG_DATA *data;

	data = &active_dialog[playerid];
	if (data->transactionid != DLG_TID_NONE && data->transactionid != transactionid) {
		logprintf("dialog_ensure_transaction: ignored %d while %d is active", transactionid, data->transactionid);
		return;
	}
	data->transactionid = transactionid;
}

/**
Append a dialog to a player's dialog queue.
*/
static
void dialog_append_to_queue(int playerid, struct DIALOG_INFO *dialog)
{
	TRACE;
	register struct DIALOG_INFO *d;
	struct DIALOG_QUEUE_ENTRY *queue_entry, *prev;

	queue_entry = malloc(sizeof(struct DIALOG_QUEUE_ENTRY));
	queue_entry->dialog = *dialog;
	queue_entry->next = NULL;

	/*Have to fix up pointers. If they don't point to the bufs, they point to rodata which is fine.*/
	d = &queue_entry->dialog;
	if (dialog->info == dialog->info_buf) {
		d->info = d->info_buf;
	}
	if (dialog->caption == dialog->caption_buf) {
		d->caption = d->caption_buf;
	}
	if (dialog->button1 == dialog->button1_buf) {
		d->button1 = d->button1_buf;
	}
	if (dialog->button2 == dialog->button2_buf) {
		d->button2 = d->button2_buf;
	}
	
	if (dialog_queue[playerid] == NULL) {
		dialog_queue[playerid] = queue_entry;
		return;
	}
	
	prev = dialog_queue[playerid];
	while (prev->next) {
		prev = prev->next;
	}
	prev->next = queue_entry;
}

/**
Shows the player a synchronous (only one at a time) dialog box.

Transactions are used to make sure open dialogs do not disappear when calling.
If a transaction is already running, and the passed transaction uses a different
transactionid, the dialog to open gets added to a queue which is polled when a
response is received. The dialog will be shown once no transaction is active
anymore. If the transaction of the new dialog is the same as the old dialog,
the old one will be aborted and the new one _will_ override it.

The transaction is reset in dialog_on_response
(which is before calling your response handler), use dialog_ensure_transaction
in your dialog's response code to keep the transaction set (eg when doing a
db query and not wanting any other dialog shown while the query is executing).
No special code is needed when showing a new dialog from within your dialog
response handler (no matter what transaction id it uses).
*/
static
void dialog_show(int playerid, struct DIALOG_INFO *dialog)
{
	TRACE;
	register struct DIALOG_DATA *data;
	struct BitStream bs;
	char *ptr, captionlen, button1len, button2len;
	int bytelength;

	data = &active_dialog[playerid];

#if DEV
	/*Don't use DLG_OPT_NOTIFY_ABORTED if you don't have a handler callback...*/
	assert(!(dialog->handler.options & DLG_OPT_NOTIFY_ABORTED) || dialog->handler.callback);
#endif

	if (!dialog->caption[0]) {
		logprintf("WARN: dialog_show invoked with an empty dialog caption "
				"(transactionid %d), dropping", dialog->transactionid);
		SendClientMessage(playerid, COL_WARN, WARN"Error, please notify robin");
		return;
	}

	if (data->transactionid != DLG_TID_NONE && data->transactionid != dialog->transactionid) {
		dialog_append_to_queue(playerid, dialog);
		return;
	}

	dialog_send_abort_result(playerid, &data->handler);

	data->expected_dialogid = ++data->last_dialogid;
	data->transactionid = dialog->transactionid;
	data->style = dialog->style;
	data->last_sendtime = time_timestamp();
	data->handler = dialog->handler;

	captionlen = strlen(dialog->caption);
	button1len = strlen(dialog->button1);
	button2len = strlen(dialog->button2);
	bytelength = 2 + 1 + 1 + captionlen + 1 + button1len + 1 + button2len;
	bs.numberOfBitsUsed = bytelength * 8;
	ptr = bs.ptrData = alloca(bytelength + LIMIT_DIALOG_INFO + /*extra for EncodeString*/100);

	*((short*) ptr) = data->expected_dialogid;
	ptr += 2;
	*ptr = dialog->style;
	ptr += 1;
	*ptr = captionlen;
	ptr += 1;
	memcpy(ptr, dialog->caption, captionlen);
	ptr += captionlen;
	*ptr = button1len;
	ptr += 1;
	memcpy(ptr, dialog->button1, button1len);
	ptr += button1len;
	*ptr = button2len;
	ptr += 1;
	memcpy(ptr, dialog->button2, button2len);
	ptr += button2len;
	bs.numberOfBitsUsed += EncodeString(ptr, dialog->info, LIMIT_DIALOG_INFO);
	SAMP_SendRPCToPlayer(RPC_ShowDialog, &bs, playerid, 2);
}

/**
Checks if any dialog is queued for the player, show it if and:
- no dialog is shown currently,
- player's dialog transaction is none or matches the next queued dialog.
*/
static
void dialog_pop_queue(int playerid)
{
	TRACE;
	register struct DIALOG_DATA *data;
	register struct DIALOG_INFO *dialog;
	struct DIALOG_QUEUE_ENTRY *peeked;

	peeked = dialog_queue[playerid];
	if (peeked) {
		data = &active_dialog[playerid];
		if (data->expected_dialogid < 0) {
			dialog = &peeked->dialog;
			if (data->transactionid == DLG_TID_NONE || data->transactionid == dialog->transactionid) {
				dialog_queue[playerid] = peeked->next;
				dialog_show(playerid, dialog);
				free(peeked);
			}
		}
	}
}

static
void dialog_sanitize_inputtext_log_when_dirty(int playerid, char *raw, char *sanitized)
{
	TRACE;
	char is_inputtext_dirty;
	char ac_info[LIMIT_DIALOG_INPUTTEXT * 3 + 100];
	char *in_pos;
	register char c;

	is_inputtext_dirty = 0;
	in_pos = raw;
	for (;;) {
		c = *in_pos;
		if (c < ' ') {
			if (c == 0) {
				*sanitized = 0;
				break;
			}
			is_inputtext_dirty = 1;
			c = '#';
		}
		*sanitized = c;
		in_pos++;
		sanitized++;
	}

	if (is_inputtext_dirty) {
		sprintf(ac_info, "raw: %s", raw);
		anticheat_log(playerid, AC_DIALOG_SMELLY_INPUT, ac_info);
		in_pos = ac_info + sprintf(ac_info, "bytes:");
		while ((c = *raw)) {
			raw++;
			in_pos += sprintf(in_pos, " %02X", ((unsigned int) c) & 0xFF);
		}
		anticheat_log(playerid, AC_DIALOG_SMELLY_INPUT, ac_info);
	}
}

/**
Called from hooked dialog response packet handler.
*/
void hook_dialog_on_response(short playerid, short dialogid, char resp, short listitem, char *raw_inputtext)
{
	TRACE;
	struct DIALOG_RESPONSE response;
	struct DIALOG_DATA active_dlg;
	int diff_last_sent, len;
	char sanitized_inputtext[LIMIT_DIALOG_INPUTTEXT + 1];
	char logbuf[100];

	/*Copying and resetting needs to be done first thing.*/
	active_dlg = active_dialog[playerid];
	dialog_reset_state(playerid);

	if (anticheat_flood(playerid, AC_FLOOD_AMOUNT_DIALOG)) {
		/*If they're flooding, the result of the popped one might get lost as well... oh well, their problem.*/
		dialog_send_abort_result(playerid, &active_dlg.handler);
		goto abort_result;
	}

	if (dialogid != active_dlg.expected_dialogid) {
		/*SAMP also checks last sent dialogid, but we overwrote that check.*/
		diff_last_sent = time_timestamp() - active_dlg.last_sendtime;
		sprintf(logbuf, "expected %d got %d (last dialog sent %dms ago)", active_dlg.expected_dialogid, dialogid, diff_last_sent);
		anticheat_log(playerid, AC_WRONG_DIALOGID, logbuf);
		goto abort_result;
	}

	if (active_dlg.handler.callback) {
		response.aborted = 0;
		response.response = resp;
		response.data = active_dlg.handler.data;
		/**TODO: Could check if listitem is within bounds (by 'parsing' the sent info), but meh.*/
		response.listitem = listitem;
		/*It seems like some unofficial mobile client sends the selected list item text back as inputtext,
		so only check inputtext when the dialog actually requested input and set to emptystring otherwise.*/
		if (active_dlg.style == DIALOG_STYLE_INPUT || active_dlg.style == DIALOG_STYLE_PASSWORD) {
			len = strlen(raw_inputtext);
			if (len > LIMIT_DIALOG_INPUTTEXT) {
				sprintf(logbuf, "length %d > "EQ(LIMIT_DIALOG_INPUTTEXT), len);
				anticheat_log(playerid, AC_DIALOG_LONG_INPUT, logbuf);
				goto abort_result;
			}
			/*TODO should this always be no_sanitize when style is password?*/
			if (active_dlg.handler.options & DLG_OPT_NO_SANITIZE_INPUTTEXT) {
				response.inputtext = raw_inputtext;
			} else {
				dialog_sanitize_inputtext_log_when_dirty(playerid, raw_inputtext, sanitized_inputtext);
				response.inputtext = sanitized_inputtext;
			}
		} else {
			response.inputtext = "";
		}
		active_dlg.handler.callback(playerid, response);
	}
	/*Don't set any state here, the response handler could've done anything and any code here might interfere with that.*/

	goto ret;
abort_result:
	dialog_send_abort_result(playerid, &active_dlg.handler);
ret:
	dialog_pop_queue(playerid);
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
	TRACE;
	register struct DIALOG_DATA *data;

	data = &active_dialog[playerid];
	if (data->transactionid != DLG_TID_NONE && data->transactionid != transactionid) {
		logprintf("dialog_end_transaction: not ending %d while %d is active", transactionid, data->transactionid);
		return;
	}
	if (data->expected_dialogid < 0) {
		data->transactionid = DLG_TID_NONE;
		dialog_pop_queue(playerid);
	}
}

static
int dialog_get_transaction(int playerid)
{
	TRACE;
	return active_dialog[playerid].transactionid;
}
