struct NEWPWDATA {
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

struct NEWPWDATA_PLAYER_CC {
	int player_cc;
	struct NEWPWDATA *data;
};

static char *CHANGEPASS_CAPTION = "Change password";

/**
Show wizard dialog to change password.

@step 0 for old pw, 1 new pw, 2 confirm new pw
@param pw_mismatch 1 to show error that new password is wrong
*/
static
void chpw_show_dialog(int playerid, int step, int pw_mismatch)
{
	char data[512], *d = data;
	
	if (pw_mismatch) {
		d += sprintf(d, ECOL_WARN"New passwords do not match, "
				"please try again.\n\n");
	}
	d += sprintf(d, step == 0 ? ECOL_INFO : ECOL_DIALOG_TEXT);
	d += sprintf(d, "* enter your current password");
	if (step == 0) d += sprintf(d, " <<<<");
	d += sprintf(d, step == 1 ? ECOL_INFO"\n" : ECOL_DIALOG_TEXT"\n");
	d += sprintf(d, "* choose a new password");
	if (step == 1) d += sprintf(d, " <<<<");
	d += sprintf(d, step == 2 ? ECOL_INFO"\n" : ECOL_DIALOG_TEXT"\n");
	d += sprintf(d, "* confirm your new password");
	if (step == 2) d += sprintf(d, " <<<<");

#if DIALOG_CHANGEPASS_NEWPASS != DIALOG_CHANGEPASS_PREVPASS + 1
#error incorrect dialog id order
#endif
#if DIALOG_CHANGEPASS_CONFIRMPASS != DIALOG_CHANGEPASS_NEWPASS + 1
#error incorrect dialog id order
#endif

	dialog_ShowPlayerDialog(
		playerid, DIALOG_CHANGEPASS_PREVPASS + step,
		DIALOG_STYLE_PASSWORD, CHANGEPASS_CAPTION, data,
		"Next", "Cancel",
		TRANSACTION_CHANGEPASS);
}

/**
Callback for bcrypt check to see if old password matches.
*/
static
void chpw_cb_verify_old_password(void *data)
{
	int playerid;

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	HideGameTextForPlayer(playerid);
	if (NC_bcrypt_is_equal()) {
		chpw_show_dialog(playerid, 1, 0);
	} else {
		/*not showing dialog again because then it's to easy to quickly
		attempt multiple passwords*/
		/*also right now this would fail because current password is
		already free'd and it would need to be loaded again to check
		it*/
		dialog_end_transaction(playerid, TRANSACTION_CHANGEPASS);
		dialog_ShowPlayerDialog(
			playerid, DIALOG_DUMMY, DIALOG_STYLE_MSGBOX,
			CHANGEPASS_CAPTION, ECOL_WARN"Incorrect password",
			"Ok", "", -1);
	}
}

/**
Callback for query that loads the player's current password.
*/
static
void chpw_cb_current_password_loaded(void *data)
{
	int playerid, size, ta;

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	/*if dialog is dismissed, transaction will be set to aborted,
	so check for that case and clear if necessary*/
	ta = dialog_transaction[playerid];
	if (ta == TRANSACTION_CHANGEPASS_ABORTED) {
		dialog_end_transaction(playerid, ta);
		return;
	}

	if (NC_cache_get_row_count()) {
		NC_cache_get_field_str(0, 0, buf144a);
		if (pwdata[playerid] == NULL) {
			/*user hasn't responded yet, store hash*/
			size = PW_HASH_LENGTH * sizeof(cell);
			pwdata[playerid] = malloc(size);
			memcpy(pwdata[playerid], cbuf144, size);
		} else {
			/*user already responded to dialog, verify pw now*/
			atoc(buf4096, (char*) pwdata[playerid], 144);
			free(pwdata[playerid]);
			pwdata[playerid] = NULL;
			common_bcrypt_check(buf4096a, buf144a,
				chpw_cb_verify_old_password, data);
		}
	} else {
		/*this should never happen unless user account is deleted while
		user is playing*/
	}
}

/**
Callback when query that updates the password is done.
*/
static
void chpw_cb_password_updated(void *data)
{
	int playerid;

	playerid = PLAYER_CC_GETID(data);
	if (!PLAYER_CC_CHECK(data, playerid)) {
		return;
	}

	HideGameTextForPlayer(playerid);
	dialog_end_transaction(playerid, TRANSACTION_CHANGEPASS);
	SendClientMessage(playerid, COL_SUCC, SUCC"Password changed!");
}

/**
Will execute the query to update the user their password.

@param data pointer to data WHICH WILL BE FREE'D BY THIS PROC
*/
static
void chpw_update_user_password(int playerid, struct NEWPWDATA *data)
{
	void* player_cc;

	player_cc = V_MK_PLAYER_CC(playerid);
	sprintf(cbuf4096_,
		"UPDATE usr SET pw='%s' WHERE i=%d",
		data->bcrypt,
		userid[playerid]);
	free(data);
	common_mysql_tquery(cbuf4096_, chpw_cb_password_updated, player_cc);
}

/**
Calledback called when bcrypt is done hashing the new password.

Bcrypt is started after new pass dialog and is needed after confirm pass
dialog.

It can be aborted when confirm pass result differs from new pass result.
*/
static
void chpw_cb_new_password_hashed(void *data)
{
	/*cannot use pwdata here, since if the confirm step does not match,
	the user can start again and already overwrite pwdata while this still
	hasn't called*/

	struct NEWPWDATA_PLAYER_CC *pd;
	struct NEWPWDATA *d;
	int player_cc, playerid;

	pd = (struct NEWPWDATA_PLAYER_CC*) data;
	player_cc = pd->player_cc;
	d = pd->data;
	free(pd);

	playerid = PLAYER_CC_GETID(player_cc);
	if (!PLAYER_CC_CHECK(player_cc, playerid)) {
		/*pwdata has been free'd on player disconnect*/
		return;
	}

	if (d->aborted) {
		/*reference in pwdata is already removed and it needs us to
		free it*/
		free(d);
		return;
	}

	NC_bcrypt_get_hash(buf144a);
	ctoa(d->bcrypt, buf144, PW_HASH_LENGTH);

	if (d->confirmed) {
		chpw_update_user_password(playerid, d);
	} else {
		d->bcryptdone = 1;
	}
}

/**
The /changepassword command allows one to change their password.

Must only be available to registered users.
*/
static
int chpw_cmd_changepassword(struct COMMANDCONTEXT cmdctx)
{
	if (pwdata[cmdctx.playerid]) {
		return 1;
	}

	chpw_show_dialog(cmdctx.playerid, 0, 0);

	/*load current password while user is filling in dialog*/
	sprintf(cbuf4096_, "SELECT pw FROM usr WHERE i=%d", userid[cmdctx.playerid]);
	common_mysql_tquery(cbuf4096_,
		chpw_cb_current_password_loaded, V_MK_PLAYER_CC(cmdctx.playerid));
	return 1;
}

void chpw_dlg_confirm_password(int playerid, int response,
		cell inputtexta, cell *inputtext)
{
	struct NEWPWDATA *d;

	d = (struct NEWPWDATA*) pwdata[playerid];
	/*reset pwdata since we don't need it anymore and if bcrypt is still
	on-going it uses the passed data instead*/
	pwdata[playerid] = NULL;
	/* Next | Cancel */
	if (response) {
		NC_PARS(4);
		nc_params[1] = nc_params[2] = inputtexta;
		nc_params[3] = buf144a;
		nc_params[4] = PW_HASH_LENGTH;
		NC(n_SHA256_PassHash);
		if (memcmp(cbuf144, d->sha256, 256)) {
			if (d->bcryptdone) {
				free(d);
			} else {
				d->aborted = 1;
			}
			chpw_show_dialog(playerid, 1, 1);
		} else {
			B144("~b~Updating...");
			NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);
			dialog_ensure_transaction(playerid,
				TRANSACTION_CHANGEPASS);

			if (d->bcryptdone) {
				chpw_update_user_password(playerid, d);
			} else {
				d->confirmed = 1;
			}
		}
	} else {
		if (d->bcryptdone) {
			/*bcrypt was already done so we can free*/
			free(d);
		} else {
			d->aborted = 1;
		}
	}
}

void chpw_dlg_new_password(int playerid, int response,
		cell inputtexta, cell *inputtext)
{
	struct NEWPWDATA_PLAYER_CC *pd;
	struct NEWPWDATA *d;

	/* Next | Cancel */
	if (response) {
		NC_PARS(4);
		nc_params[1] = nc_params[2] = inputtexta;
		nc_params[3] = buf144a;
		nc_params[4] = PW_HASH_LENGTH;
		NC(n_SHA256_PassHash);
		d = malloc(sizeof(struct NEWPWDATA));
		d->aborted = 0;
		d->confirmed = 0;
		d->bcryptdone = 0;
		memcpy(d->sha256, buf144, 256);
		pwdata[playerid] = d;

		pd = malloc(sizeof(struct NEWPWDATA_PLAYER_CC));
		pd->player_cc = MK_PLAYER_CC(playerid);
		pd->data = d;
		common_bcrypt_hash(inputtexta, chpw_cb_new_password_hashed, pd);

		chpw_show_dialog(playerid, 2, 0);
	}
	/*nothing to cleanup on cancel*/
}

void chpw_dlg_previous_password(int playerid, int response,
		cell inputtexta, cell *inputtext)
{
	int size;

	/* Next | Cancel */
	if (response) {
		dialog_ensure_transaction(playerid, TRANSACTION_CHANGEPASS);
		B144("~b~Validating...");
		NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);

		if (pwdata[playerid] == NULL) {
			/*means the password hasn't loaded yet,
			save actual password so password load can check.*/
			/*TODO: I don't like this temp storing of
			plain pw (but is is unpacked though)*/
			/*129 size since it's straight from inputtext*/
			size = 129 * sizeof(cell);
			pwdata[playerid] = malloc(size);
			memcpy(pwdata[playerid], inputtext, size);
		} else {
			/*pw was loaded already, check it now*/
			memcpy(cbuf144, pwdata[playerid],
				PW_HASH_LENGTH * sizeof(cell));
			free(pwdata[playerid]);
			pwdata[playerid] = NULL;
			common_bcrypt_check(inputtexta, buf144a,
				chpw_cb_verify_old_password,
				V_MK_PLAYER_CC(playerid));
		}
	} else {
		if (pwdata[playerid] == NULL) {
			/*means the password hasn't loaded yet, set the
			transaction to aborted changepass so when it's loaded
			it knows it needs to be discarded*/
			/*no other dialogs can be shown while this is ungoing,
			but this is a non-issue*/
			dialog_ensure_transaction(playerid,
				TRANSACTION_CHANGEPASS_ABORTED);
		} else {
			/*pw was already loaded, just free and be done*/
			free(pwdata[playerid]);
			pwdata[playerid] = NULL;
		}
	}
}

void chpw_on_player_connect(int playerid)
{
	pwdata[playerid] = NULL;
}

void chpw_on_player_disconnect(int playerid)
{
	if (pwdata[playerid] != NULL) {
		free(pwdata[playerid]);
		pwdata[playerid] = NULL;
	}
}
