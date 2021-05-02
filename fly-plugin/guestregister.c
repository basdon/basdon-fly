struct REGISTERDATA {
	char newname[MAX_PLAYER_NAME];
	char oldname[MAX_PLAYER_NAME];
	char bcrypt[PW_HASH_LENGTH];
	char sha256[SHA256BUFSIZE];
	/**
	Flag specifying if bcrypt is done.
	*/
	char bcryptdone;
	/**
	Flag specifying if password has been confirmed by user.
	When bcrypt is done and this flag is set, it should continue.
	*/
	char confirmed;
	/**
	Flag specifying this data is aborted, to be set on canceling while
	bcryptdone is 0, so bcrypt callback can free memory.
	*/
	char aborted;
};

/**
Struct to pass to callbacks to make sure we're never dealing with NULL ptrs.
*/
struct CBDATA {
	int player_cc;
	struct REGISTERDATA *data;
};

/*TODO cleanup*/
static void guestreg_dlg_change_name(int playerid, struct DIALOG_RESPONSE response);
static void guestreg_dlg_register_firstpass(int playerid, struct DIALOG_RESPONSE response);

/**
Check if the given name is a valid name given by player.

Checks both the characters and the length.

@return non-zero if the name contains invalid characters
*/
static
int guestreg_is_name_valid(char *name)
{
	static const char *VALIDS = "x___xx____x_xxxxxxxxxx___x__x"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxx_x_x_xxxxxxxxxxxxxxxxxxxxxxxxxx";

	unsigned char c, len;

	len = 0;
	while ((c = (unsigned char) *name)) {
		if (c < 36 || 122 < c || VALIDS[c - 36] == '_') {
			return 0;
		}
		len++;
		name++;
	}
	return 2 < len && len < 21;
}

static
void guestreg_rollback_name(int playerid, struct REGISTERDATA *rd)
{
	if (natives_SetPlayerName(playerid, rd->oldname) != 1) {
		login_give_guest_name(playerid);
	}
}

/**
Shows the dialog to change username.
*/
static
void guestreg_show_dialog_namechange(int playerid, int show_invalid_name_error, int show_username_taken_error)
{
	struct DIALOG_INFO dialog;
	char *d;

	dialog_init_info(&dialog);
	d = dialog.info;
	if (show_invalid_name_error) {
		d += sprintf(d,
			ECOL_WARN"Invalid username!\n\n"
			ECOL_DIALOG_TEXT);
	}
	if (show_username_taken_error) {
		d += sprintf(d,
			ECOL_WARN"That username is already taken!\n\n"
			ECOL_DIALOG_TEXT);
	}
	sprintf(d,
		"Enter the username you want to register.\n"
		"Valid usernames are 3-21 characters long and "
		"cannot start with a @ symbol.\n"
		"Your current username is: %s",
		pdata[playerid]->name);
	dialog.transactionid = DLG_TID_GUESTREGISTER;
	dialog.style = DIALOG_STYLE_INPUT;
	dialog.caption = REGISTER_CAPTION;
	dialog.button1 = "Change name";
	dialog.button2 = "Cancel";
	dialog.handler.callback = guestreg_dlg_change_name;
	dialog_show(playerid, &dialog);
}

/**
Show dialog to enter or confirm password to complete guest registration.

@param step 0 for enter password or 1 for confirm password
*/
static
void guestreg_show_dialog_password(int playerid, int step, int pw_mismatch, void (*cb)(int playerid, struct DIALOG_RESPONSE))
{
	struct DIALOG_INFO dialog;
	char *d;

	dialog_init_info(&dialog);
	d = dialog.info;
	if (pw_mismatch) {
		d += sprintf(d,
			ECOL_WARN"Passwords do not match!\n\n");
	}
	d += sprintf(d, step == 0 ? ECOL_INFO : ECOL_DIALOG_TEXT);
	d += sprintf(d, "* choose a password");
	if (step == 0) d += sprintf(d, " <<<<");
	d += sprintf(d, step == 1 ? ECOL_INFO"\n" : ECOL_DIALOG_TEXT"\n");
	d += sprintf(d, "* confirm your password");
	if (step == 1) d += sprintf(d, " <<<<");
	dialog.transactionid = DLG_TID_GUESTREGISTER;
	dialog.style = DIALOG_STYLE_PASSWORD;
	dialog.caption = REGISTER_CAPTION;
	dialog.button1 = "Next";
	dialog.button2 = "Cancel";
	dialog.handler.options = DLG_OPT_NO_SANITIZE_INPUTTEXT;
	dialog.handler.callback = cb;
	dialog_show(playerid, &dialog);
}

/**
Callback when the register query executed.
*/
static
void guestreg_cb_registered(void *data)
{
	int playerid;
	char msg144[144];

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	HideGameTextForPlayer(playerid);
	dialog_end_transaction(playerid, DLG_TID_GUESTREGISTER);
	SendClientMessage(playerid, COL_SUCC, SUCC"Your account has been registered and your stats are saved, welcome!");
	sprintf(msg144, "Guest %s[%d] just registered their account, welcome!", pdata[playerid]->name, playerid);
	SendClientMessageToAllAndIRC(ECHO_PACK12_LOGIN, COL_JOIN, msg144);
}

/**
Execute the register query.

Frees given register data.
*/
static
void guestreg_do_register(int playerid, struct REGISTERDATA *rd)
{
	pdata[playerid]->groups &= ~GROUP_GUEST;
	pdata[playerid]->groups |= GROUP_MEMBER;

	sprintf(cbuf4096_,
		"UPDATE usr SET pw='%s',name='%s',groups=%d WHERE i=%d",
	        rd->bcrypt,
		pdata[playerid]->name,
		pdata[playerid]->groups,
	        userid[playerid]);
	free(rd);
	common_mysql_tquery(cbuf4096_,
		guestreg_cb_registered, V_MK_PLAYER_CC(playerid));
}

/**
Callback for username exist check query.
*/
static
void guestreg_cb_check_username_exists(void *data)
{
	struct REGISTERDATA *d;
	int player_cc, playerid;

	d = ((struct CBDATA*) data)->data;
	player_cc = ((struct CBDATA*) data)->player_cc;
	free(data);

	playerid = PLAYER_CC_GETID(player_cc);
	if (!PLAYER_CC_CHECK(player_cc, playerid)) {
		return;
	}

	HideGameTextForPlayer(playerid);

	if (NC_cache_get_row_count()) {
		guestreg_show_dialog_namechange(playerid, 0, 1);
		return;
	}

	if (natives_SetPlayerName(playerid, d->newname) == 1) {
		guestreg_show_dialog_password(playerid, 0, 0, guestreg_dlg_register_firstpass);
		return;
	}

	SendClientMessage(playerid, COL_WARN, WARN"Failed to change your name.");
	dialog_end_transaction(playerid, DLG_TID_GUESTREGISTER);
}

/**
Callback for bcrypt hasing hasspaword after user fills in password first time.
*/
static
void guestreg_cb_password_hashed(void *data)
{
	/*TODO duplicate code (changepassword.c)*/
	struct REGISTERDATA *rd;
	int player_cc, playerid;

	rd = ((struct CBDATA*) data)->data;
	player_cc = ((struct CBDATA*) data)->player_cc;
	free(data);

	playerid = PLAYER_CC_GETID(player_cc);
	if (!PLAYER_CC_CHECK(player_cc, playerid)) {
		return;
	}
	if (rd->aborted) {
		/*reference in pwdata is already removed and it needs us to
		free it*/
		free(rd);
		return;
	}

	NC_bcrypt_get_hash(buf144a);
	ctoa(rd->bcrypt, buf144, PW_HASH_LENGTH);

	if (rd->confirmed) {
		guestreg_do_register(playerid, rd);
	} else {
		rd->bcryptdone = 1;
	}
}

static
void guestreg_dlg_change_name(int playerid, struct DIALOG_RESPONSE response)
{
	struct REGISTERDATA *rd;
	struct CBDATA *cd;

	/* Change name | Cancel */
	if (!response.response) {
		return;
	}

	if (!guestreg_is_name_valid(response.inputtext)) {
		guestreg_show_dialog_namechange(playerid, 1, 0);
		return;
	}

	GameTextForPlayer(playerid, 0x800000, 3, "~b~Checking username...");

	dialog_ensure_transaction(playerid, DLG_TID_GUESTREGISTER);

	rd = malloc(sizeof(struct REGISTERDATA));
	strcpy(rd->oldname, pdata[playerid]->name);
	strcpy(rd->newname, response.inputtext);
	rd->aborted = 0;
	rd->bcryptdone = 0;
	rd->confirmed = 0;
	pwdata[playerid] = rd;

	cd = malloc(sizeof(struct CBDATA));
	cd->player_cc = MK_PLAYER_CC(playerid);
	cd->data = rd;

	sprintf(cbuf4096_, "SELECT i FROM usr WHERE name='%s'", response.inputtext);
	common_mysql_tquery(cbuf4096_, guestreg_cb_check_username_exists, cd);
}

static
void guestreg_dlg_register_confirmpass(int playerid, struct DIALOG_RESPONSE response)
{
	/*TODO duplicate code (changepassword)*/
	struct REGISTERDATA *rd, *oldrd;
	char tmp_hash_buf[SHA256BUFSIZE];

	rd = (struct REGISTERDATA*) pwdata[playerid];
	pwdata[playerid] = NULL;
	/* Next | Cancel */
	if (response.response) {
		SAMP_SHA256(tmp_hash_buf, response.inputtext);
		if (memcmp(tmp_hash_buf, rd->sha256, SHA256BUFSIZE)) {
			oldrd = rd;
			rd = malloc(sizeof(struct REGISTERDATA));
			memcpy(rd, oldrd, sizeof(struct REGISTERDATA));
			if (oldrd->bcryptdone) {
				free(oldrd);
			} else {
				oldrd->aborted = 1;
			}
			pwdata[playerid] = rd;
			guestreg_show_dialog_password(playerid, 0, 1, guestreg_dlg_register_firstpass);
		} else {
			GameTextForPlayer(playerid, 0x800000, 3, "~b~Registering...");
			dialog_ensure_transaction(playerid, DLG_TID_GUESTREGISTER);

			if (rd->bcryptdone) {
				guestreg_do_register(playerid, rd);
			} else {
				rd->confirmed = 1;
			}
		}
	} else {
		guestreg_rollback_name(playerid, rd);
		if (rd->bcryptdone) {
			free(rd);
		} else {
			rd->aborted = 1;
		}
	}
}

static
void guestreg_dlg_register_firstpass(int playerid, struct DIALOG_RESPONSE response)
{
	struct REGISTERDATA *rd;
	struct CBDATA *cd;

	rd = (struct REGISTERDATA*) pwdata[playerid];
	/* Next | Cancel */
	if (response.response) {
		SAMP_SHA256(rd->sha256, response.inputtext);

		cd = malloc(sizeof(struct CBDATA));
		cd->player_cc = MK_PLAYER_CC(playerid);
		cd->data = rd;
		atoc(buf144, response.inputtext, 144);
		common_bcrypt_hash(buf144a, guestreg_cb_password_hashed, cd);

		guestreg_show_dialog_password(playerid, 1, 0, guestreg_dlg_register_confirmpass);
	} else {
		guestreg_rollback_name(playerid, rd);
		free(pwdata[playerid]);
		pwdata[playerid] = NULL;
	}
}
