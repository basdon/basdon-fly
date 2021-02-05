struct REGISTERDATA {
	char newname[MAX_PLAYER_NAME];
	char oldname[MAX_PLAYER_NAME];
	char bcrypt[PW_HASH_LENGTH];
	cell sha256[PW_HASH_LENGTH];
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
void guestreg_show_dialog_namechange(int playerid,
	int show_invalid_name_error, int show_username_taken_error)
{
	char data[512], *d = data;

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

	dialog_ShowPlayerDialog(playerid,
		DIALOG_GUESTREGISTER_CHANGENAME, DIALOG_STYLE_INPUT,
		(char*) REGISTER_CAPTION, data,
		"Change name", "Cancel",
		TRANSACTION_GUESTREGISTER);
}

/**
Show dialog to enter or confirm password to complete guest registration.

@param step 0 for enter password or 1 for confirm password
*/
static
void guestreg_show_dialog_password(int playerid, int step, int pw_mismatch)
{
	char data[512], *d = data;

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

#if DIALOG_GUESTREGISTER_CONFIRMPASS != DIALOG_GUESTREGISTER_FIRSTPASS + 1
#error incorrect dialog id order
#endif

	dialog_ShowPlayerDialog(playerid,
		DIALOG_GUESTREGISTER_FIRSTPASS + step, DIALOG_STYLE_PASSWORD,
		(char*) REGISTER_CAPTION, data,
		"Next", "Cancel",
		TRANSACTION_GUESTREGISTER);
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
	dialog_end_transaction(playerid, TRANSACTION_GUESTREGISTER);
	SendClientMessage(playerid, COL_SUCC, SUCC"Your account has been registered and your stats are saved, welcome!");
	sprintf(msg144, "Guest %s[%d] just registered their account, welcome!", pdata[playerid]->name, playerid);
	SendClientMessageToAll(COL_JOIN, msg144);
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
		guestreg_show_dialog_password(playerid, 0, 0);
		return;
	}

	SendClientMessage(playerid, COL_WARN, WARN"Failed to change your name.");
	dialog_end_transaction(playerid, TRANSACTION_GUESTREGISTER);
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

/**
The /register command allows guests to register their account.
*/
static
int guestreg_cmd_register(struct COMMANDCONTEXT cmdctx)
{
	if (sessionid[cmdctx.playerid] == -1 || userid[cmdctx.playerid] == -1) {
		SendClientMessage(cmdctx.playerid, COL_WARN,
			WARN"Your guest session is not linked to a guest account. Please reconnect if you want to register.");
	} else {
		guestreg_show_dialog_namechange(cmdctx.playerid, 0, 0);
	}
	return 1;
}

void guestreg_dlg_change_name(int playerid, int response, char *inputtext)
{
	struct REGISTERDATA *rd;
	struct CBDATA *cd;

	/* Change name | Cancel */
	if (!response) {
		return;
	}

	if (!guestreg_is_name_valid(inputtext)) {
		guestreg_show_dialog_namechange(playerid, 1, 0);
		return;
	}

	B144("~b~Checking username...");
	NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);

	dialog_ensure_transaction(playerid, TRANSACTION_GUESTREGISTER);

	rd = malloc(sizeof(struct REGISTERDATA));
	strcpy(rd->oldname, pdata[playerid]->name);
	strcpy(rd->newname, inputtext);
	rd->aborted = 0;
	rd->bcryptdone = 0;
	rd->confirmed = 0;
	pwdata[playerid] = rd;

	cd = malloc(sizeof(struct CBDATA));
	cd->player_cc = MK_PLAYER_CC(playerid);
	cd->data = rd;

	sprintf(cbuf4096_, "SELECT i FROM usr WHERE name='%s'", inputtext);
	common_mysql_tquery(cbuf4096_,	guestreg_cb_check_username_exists, cd);
}

void guestreg_dlg_register_firstpass(int playerid,
	int response, cell inputtexta, cell *inputtext)
{
	struct REGISTERDATA *rd;
	struct CBDATA *cd;

	rd = (struct REGISTERDATA*) pwdata[playerid];
	/* Next | Cancel */
	if (response) {
		NC_PARS(4);
		nc_params[1] = nc_params[2] = inputtexta;
		nc_params[3] = buf144a;
		nc_params[4] = PW_HASH_LENGTH;
		NC(n_SHA256_PassHash);
		memcpy(rd->sha256, buf144, 256);

		cd = malloc(sizeof(struct CBDATA));
		cd->player_cc = MK_PLAYER_CC(playerid);
		cd->data = rd;
		common_bcrypt_hash(inputtexta, guestreg_cb_password_hashed, cd);

		guestreg_show_dialog_password(playerid, 1, 0);
	} else {
		guestreg_rollback_name(playerid, rd);
		free(pwdata[playerid]);
		pwdata[playerid] = NULL;
	}
}

void guestreg_dlg_register_confirmpass(int playerid,
	int response, cell inputtexta, cell *inputtext)
{
	/*TODO duplicate code (changepassword)*/
	struct REGISTERDATA *rd, *oldrd;

	rd = (struct REGISTERDATA*) pwdata[playerid];
	pwdata[playerid] = NULL;
	/* Next | Cancel */
	if (response) {
		NC_PARS(4);
		nc_params[1] = nc_params[2] = inputtexta;
		nc_params[3] = buf144a;
		nc_params[4] = PW_HASH_LENGTH;
		NC(n_SHA256_PassHash);
		if (memcmp(cbuf144, rd->sha256, 256)) {
			oldrd = rd;
			rd = malloc(sizeof(struct REGISTERDATA));
			memcpy(rd, oldrd, sizeof(struct REGISTERDATA));
			if (oldrd->bcryptdone) {
				free(oldrd);
			} else {
				oldrd->aborted = 1;
			}
			pwdata[playerid] = rd;
			guestreg_show_dialog_password(playerid, 0, 1);
		} else {
			B144("~b~Registering...");
			NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);
			dialog_ensure_transaction(playerid,
				TRANSACTION_GUESTREGISTER);

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
