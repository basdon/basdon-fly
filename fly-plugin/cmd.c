struct COMMAND {
	int hash;
	/**This points to its aliased cmd or itself.*/
	struct COMMAND *alias_of;
	const char *cmd;
	const char *aliased_cmd;
	const int groups;
	const char *syntax;
	const char *description;
	/**
	return 0 to print the syntax error msg
	*/
	int (*handler)(struct COMMANDCONTEXT ctx);
};

/* command must prefixed by forward slash and be lower case */
static struct COMMAND cmds[] = {
#ifdef DEV
	{ 0, 0, "/*m", 0, GROUPS_ALL, CMD__M_SYNTAX, CMD__M_DESC, cmd_dev__m },
	{ 0, 0, "/cp", 0, GROUPS_ALL, CMD_CP_SYNTAX, CMD_CP_DESC, cmd_dev_cp },
	{ 0, 0, "/crashme", 0, GROUPS_ALL, CMD_CRASHME_SYNTAX, CMD_CRASHME_DESC, cmd_dev_crashme },
	{ 0, 0, "/drvc", 0, GROUPS_ALL, CMD_DRVC_SYNTAX, CMD_DRVC_DESC, cmd_dev_disableremotevehiclecollisions },
	{ 0, 0, "/fweather", 0, GROUPS_ALL, CMD_FWEATHER_SYNTAX, CMD_FWEATHER_DESC, cmd_dev_fweather },
	{ 0, 0, "/gt", 0, GROUPS_ALL, CMD_GT_SYNTAX, CMD_GT_DESC, cmd_dev_gt },
	{ 0, 0, "/jetpack", 0, GROUPS_ALL, CMD_JETPACK_SYNTAX, CMD_JETPACK_DESC, cmd_dev_jetpack },
	{ 0, 0, "/kickme", 0, GROUPS_ALL, CMD_KICKME_SYNTAX, CMD_KICKME_DESC, cmd_dev_kickme },
	{ 0, 0, "/kill", 0, GROUPS_ALL, CMD_KILL_SYNTAX, CMD_KILL_DESC, cmd_dev_kill },
	{ 0, 0, "/nweather", 0, GROUPS_ALL, CMD_NWEATHER_SYNTAX, CMD_NWEATHER_DESC, cmd_dev_nweather },
	{ 0, 0, "/owner", 0, GROUPS_ALL, CMD_OWNER_SYNTAX, CMD_OWNER_DESC, cmd_dev_owner },
	{ 0, 0, "/platform", 0, GROUPS_ALL, CMD_PLATFORM_SYNTAX, CMD_PLATFORM_DESC, cmd_dev_platform },
	{ 0, 0, "/rnw", 0, GROUPS_ALL, CMD_RNW_SYNTAX, CMD_RNW_DESC, cmd_dev_rnw },
	{ 0, 0, "/siren", 0, GROUPS_ALL, CMD_SIREN_SYNTAX, CMD_SIREN_DESC, cmd_dev_siren },
	{ 0, 0, "/sound", 0, GROUPS_ALL, CMD_SOUND_SYNTAX, CMD_SOUND_DESC, cmd_dev_sound },
	{ 0, 0, "/testparpl", 0, GROUPS_ALL, CMD_TESTPARPL_SYNTAX, CMD_TESTPARPL_DESC, cmd_dev_testparpl },
	{ 0, 0, "/timecyc", 0, GROUPS_ALL, CMD_TIMECYC_SYNTAX, CMD_TIMECYC_DESC, cmd_dev_timecyc },
	{ 0, 0, "/timex", 0, GROUPS_ALL, CMD_TIMEX_SYNTAX, CMD_TIMEX_DESC, cmd_dev_timex },
	{ 0, 0, "/tweather", 0, GROUPS_ALL, CMD_TWEATHER_SYNTAX, CMD_TWEATHER_DESC, cmd_dev_tweather },
	{ 0, 0, "/v", 0, GROUPS_ALL, CMD_V_SYNTAX, CMD_V_DESC, cmd_dev_v },
	{ 0, 0, "/vdamage", 0, GROUPS_ALL, CMD_VDAMAGE_SYNTAX, CMD_VDAMAGE_DESC, cmd_dev_vdamage },
	{ 0, 0, "/vfl", 0, GROUPS_ALL, CMD_VFL_SYNTAX, CMD_VFL_DESC, cmd_dev_vfl },
	{ 0, 0, "/vhp", 0, GROUPS_ALL, CMD_VHP_SYNTAX, CMD_VHP_DESC, cmd_dev_vhp },
	{ 0, 0, "/vhpnan", 0, GROUPS_ALL, CMD_VHPNAN_SYNTAX, CMD_VHPNAN_DESC, cmd_dev_vhpnan },
	{ 0, 0, "/vhpninf", 0, GROUPS_ALL, CMD_VHPNINF_SYNTAX, CMD_VHPNINF_DESC, cmd_dev_vhpninf },
	{ 0, 0, "/vhppinf", 0, GROUPS_ALL, CMD_VHPPINF_SYNTAX, CMD_VHPPINF_DESC, cmd_dev_vhppinf },
#endif /*DEV*/
	{ 0, 0, "//getcar", 0, GROUPS_ADMIN, CMD__GETCAR_SYNTAX, CMD__GETCAR_DESC, cmd__getcar },
	{ 0, 0, "//goto", 0, GROUPS_ADMIN, CMD__GOTO_SYNTAX, CMD__GOTO_DESC, cmd__goto },
	{ 0, 0, "//gotorel", 0, GROUPS_ADMIN, CMD__GOTOREL_SYNTAX, CMD__GOTOREL_DESC, cmd__gotorel },
	{ 0, 0, "//makeanewpermanentpublicvehiclehere", 0, GROUP_OWNER, CMD__MANPPVH_SYNTAX, CMD__MANPPVH_DESC,
		cmd__makeanewpermanentpublicvehiclehere },
	{ 0, 0, "//park", 0, GROUPS_ADMIN, CMD__PARK_SYNTAX, CMD__PARK_DESC, cmd__park },
	{ 0, 0, "//respawn", 0, GROUPS_ADMIN, CMD__RESPAWN_SYNTAX, CMD__RESPAWN_DESC, cmd__respawn },
	{ 0, 0, "//rr", 0, GROUPS_ADMIN, CMD__RR_SYNTAX, CMD__RR_DESC, cmd__rr },
	{ 0, 0, "//spray", 0, GROUPS_ADMIN, CMD__SPRAY_SYNTAX, CMD__SPRAY_DESC, cmd__spray },
	{ 0, 0, "//streamdistance", 0, GROUP_OWNER, CMD__STREAMDISTANCE_SYNTAX, CMD__STREAMDISTANCE_DESC, cmd__streamdistance },
	{ 0, 0, "//tocar", 0, GROUPS_ADMIN, CMD__TOCAR_SYNTAX, CMD__TOCAR_DESC, cmd__tocar },
	{ 0, 0, "//tomsp", 0, GROUPS_ADMIN, CMD__TOMSP_SYNTAX, CMD__TOMSP_DESC, cmd__tomsp },
	{ 0, 0, "//vehinfo", 0, GROUP_OWNER, CMD__VEHINFO_SYNTAX, CMD__VEHINFO_DESC, cmd__vehinfo },
	{ 0, 0, "/adf", 0, GROUPS_ALL, CMD_ADF_SYNTAX, CMD_ADF_DESC, cmd_adf },
	{ 0, 0, "/andro", "/androm", 0, 0, 0, 0 },
	{ 0, 0, "/androm", 0, GROUPS_ALL, CMD_ANDROM_SYNTAX, CMD_ANDROM_DESC, cmd_androm },
	{ 0, 0, "/andromada", "/androm", 0, 0, 0, 0 },
	{ 0, 0, "/at400", 0, GROUPS_ALL, CMD_AT400_SYNTAX, CMD_AT400_DESC, cmd_at400 },
	{ 0, 0, "/beacons", 0, GROUPS_ALL, CMD_BEACONS_SYNTAX, CMD_BEACONS_DESC, cmd_beacons },
	{ 0, 0, "/camera", 0, GROUPS_ALL, CMD_CAMERA_SYNTAX, CMD_CAMERA_DESC, cmd_camera },
	{ 0, 0, "/cancelmission", "/s", 0, 0, 0, 0 },
	{ 0, 0, "/cancelwork", "/s", 0, 0, 0, 0 },
	{ 0, 0, "/changepassword", 0, GROUPS_REGISTERED, CMD_CHANGEPASSWORD_SYNTAX, CMD_CHANGEPASSWORD_DESC,
		cmd_changepassword },
	{ 0, 0, "/changelog", 0, GROUPS_ALL, CMD_CHANGELOG_SYNTAX, CMD_CHANGELOG_DESC, cmd_changelog },
	{ 0, 0, "/chute", 0, GROUPS_ALL, CMD_CHUTE_SYNTAX, CMD_CHUTE_DESC, cmd_chute },
	{ 0, 0, "/close", 0, GROUPS_ALL, CMD_CLOSE_SYNTAX, CMD_CLOSE_DESC, cmd_close },
	{ 0, 0, "/cmdlist", "/cmds", 0, 0, 0, 0 },
	{ 0, 0, "/cmds", 0, GROUPS_ALL, CMD_CMDS_SYNTAX, CMD_CMDS_DESC, cmd_cmds },
	{ 0, 0, "/commandlist", "/cmds", 0, 0, 0, 0 },
	{ 0, 0, "/commands", "/cmds", 0, 0, 0, 0 },
	{ 0, 0, "/credits", 0, GROUPS_ALL, CMD_CREDITS_SYNTAX, CMD_CREDITS_DESC, cmd_credits },
	{ 0, 0, "/engine", 0, GROUPS_ALL, CMD_ENGINE_SYNTAX, CMD_ENGINE_DESC, cmd_engine },
	{ 0, 0, "/fix", "/repair", 0, 0, 0, 0 },
	{ 0, 0, "/getspray", 0, GROUPS_ALL, CMD_GETSPRAY_SYNTAX, CMD_GETSPRAY_DESC, cmd_getspray },
	{ 0, 0, "/help", 0, GROUPS_ALL, CMD_HELP_SYNTAX, CMD_HELP_DESC, cmd_help },
	{ 0, 0, "/helpaccount", 0, GROUPS_ALL, CMD_HELPACCOUNT_SYNTAX, CMD_HELPACCOUNT_DESC, cmd_helpaccount },
	{ 0, 0, "/helpadf", 0, GROUPS_ALL, CMD_HELPADF_SYNTAX, CMD_HELPADF_DESC, cmd_helpadf },
	{ 0, 0, "/helpcmd", 0, GROUPS_ALL, CMD_HELPCMD_SYNTAX, CMD_HELPCMD_DESC, cmd_helpcmd },
	{ 0, 0, "/helpcopilot", 0, GROUPS_ALL, CMD_HELPCOPILOT_SYNTAX, CMD_HELPCOPILOT_DESC, cmd_helpcopilot },
	{ 0, 0, "/helpils", 0, GROUPS_ALL, CMD_HELPILS_SYNTAX, CMD_HELPILS_DESC, cmd_helpils },
	{ 0, 0, "/helpkeys", 0, GROUPS_ALL, CMD_HELPKEYS_SYNTAX, CMD_HELPKEYS_DESC, cmd_helpkeys },
	{ 0, 0, "/helpmission", 0, GROUPS_ALL, CMD_HELPMISSION_SYNTAX, CMD_HELPMISSION_DESC, cmd_helpmission },
	{ 0, 0, "/helpmissions", "/helpmission", 0, 0, 0, 0 },
	{ 0, 0, "/helpnav", 0, GROUPS_ALL, CMD_HELPNAV_SYNTAX, CMD_HELPNAV_DESC, cmd_helpnav },
	{ 0, 0, "/helpradio", 0, GROUPS_ALL, CMD_HELPRADIO_SYNTAX, CMD_HELPRADIO_DESC, cmd_helpradio },
	{ 0, 0, "/helpvor", 0, GROUPS_ALL, CMD_HELPVOR_SYNTAX, CMD_HELPVOR_DESC, cmd_helpvor },
	{ 0, 0, "/ils", 0, GROUPS_ALL, CMD_ILS_SYNTAX, CMD_ILS_DESC, cmd_ils },
	{ 0, 0, "/info", "/help", 0, 0, 0, 0 },
	{ 0, 0, "/irc", 0, GROUPS_ALL, CMD_IRC_SYNTAX, CMD_IRC_DESC, cmd_irc },
	{ 0, 0, "/loc", 0, GROUPS_ALL, CMD_LOC_SYNTAX, CMD_LOC_DESC, cmd_loc },
	{ 0, 0, "/nearest", 0, GROUPS_ALL, CMD_NEAREST_SYNTAX, CMD_NEAREST_DESC, cmd_nearest },
	{ 0, 0, "/me", 0, GROUPS_ALL, CMD_ME_SYNTAX, CMD_ME_DESC, cmd_me },
	{ 0, 0, "/metar", 0, GROUPS_ALL, CMD_METAR_SYNTAX, CMD_METAR_DESC, cmd_metar },
	{ 0, 0, "/mission", "/w", 0, 0, 0, 0 },
	{ 0, 0, "/park", 0, GROUPS_ALL, CMD_PARK_SYNTAX, CMD_PARK_DESC, cmd_park },
	{ 0, 0, "/p", "/prefs", 0, 0, 0, 0 },
	{ 0, 0, "/pm", 0, GROUPS_ALL, CMD_PM_SYNTAX, CMD_PM_DESC, cmd_pm },
	{ 0, 0, "/prefs", 0, GROUPS_ALL, CMD_PREFS_SYNTAX, CMD_PREFS_DESC, cmd_prefs },
	{ 0, 0, "/preferences", "/prefs", 0, 0, 0, 0 },
	{ 0, 0, "/protip", 0, GROUPS_ALL, CMD_PROTIP_SYNTAX, CMD_PROTIP_DESC, cmd_protip },
	{ 0, 0, "/r", 0, GROUPS_ALL, CMD_R_SYNTAX, CMD_R_DESC, cmd_r },
	{ 0, 0, "/radiomacros", 0, GROUPS_ALL, CMD_RADIOMACROS_SYNTAX, CMD_RADIOMACROS_DESC, cmd_radiomacros },
	{ 0, 0, "/reclass", 0, GROUPS_ALL, CMD_RECLASS_SYNTAX, CMD_RECLASS_DESC, cmd_reclass },
	{ 0, 0, "/register", 0, GROUP_GUEST, CMD_REGISTER_SYNTAX, CMD_REGISTER_DESC, cmd_register },
	{ 0, 0, "/repair", 0, GROUPS_ALL, CMD_REPAIR_SYNTAX, CMD_REPAIR_DESC, cmd_repair },
	{ 0, 0, "/refuel", 0, GROUPS_ALL, CMD_REFUEL_SYNTAX, CMD_REFUEL_DESC, cmd_refuel },
	{ 0, 0, "/respawn", 0, GROUPS_ALL, CMD_RESPAWN_SYNTAX, CMD_RESPAWN_DESC, cmd_respawn },
	{ 0, 0, "/s", 0, GROUPS_ALL, CMD_S_SYNTAX, CMD_S_DESC, cmd_s },
	{ 0, 0, "/spray", 0, GROUPS_ALL, CMD_SPRAY_SYNTAX, CMD_SPRAY_DESC, cmd_spray },
	{ 0, 0, "/stoplocate", 0, GROUPS_ALL, CMD_STOPLOCATE_SYNTAX, CMD_STOPLOCATE_DESC, cmd_stoplocate },
	{ 0, 0, "/stopwork", "/s", 0, 0, 0, 0 },
	{ 0, 0, "/tickrate", 0, GROUPS_ALL, CMD_TICKRATE_SYNTAX, CMD_TICKRATE_DESC, cmd_tickrate },
	{ 0, 0, "/updates", "/changelog", 0, 0, 0, 0 },
	{ 0, 0, "/vor", 0, GROUPS_ALL, CMD_VOR_SYNTAX, CMD_VOR_DESC, cmd_vor },
	{ 0, 0, "/w", 0, GROUPS_ALL, CMD_W_SYNTAX, CMD_W_DESC, cmd_w },
	{ 0, 0, "/weather", "/metar", 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
};

/*
Check if the command in cmdtext is same as cmd (case insensensitive).
Parseidx is not written to if it didn't match.
On match, parseidx is the index right after the command, so either space or \0.
*/
static
int cmd_is(const char *cmdtext, const char *cmd, int *parseidx)
{
	int pos = 0;

nextchar:
	/* starts checking at 1 because 0 is always a forward slash */
	++pos;
	if (cmdtext[pos] == cmd[pos] || cmd[pos] == 0) {
		if (cmdtext[pos] == 0 || cmdtext[pos] == ' ') {
			*parseidx = pos;
			return 1;
		}
		goto nextchar;
	}
	if ('A' <= cmdtext[pos] && cmdtext[pos] <= 'Z' && (cmdtext[pos] | 0x20) == cmd[pos]) {
		goto nextchar;
	}
	return 0;
}

/*
Precalcs all command hashes and links aliases.
*/
static
void cmd_init()
{
	register struct COMMAND *cmd, *aliased;
	int hash;

	/*Hash, set alias to itself.*/
	cmd = cmds;
	while (cmd->cmd) {
		cmd->hash = strhashcode((char*) cmd->cmd);
		cmd->alias_of = cmd;
		cmd++;
	}

	/*Set up aliases.*/
	cmd = cmds;
	while (cmd->cmd) {
		if (cmd->aliased_cmd) {
			hash = strhashcode((char*) cmd->aliased_cmd);
			aliased = cmds;
			for (;;) {
				assert(aliased->cmd); /*Aliased command doesn't exist.*/
				if (aliased->hash == hash && !strcmp(aliased->cmd, cmd->aliased_cmd)) {
					assert(!aliased->aliased_cmd); /*Can't alias to an alias.*/
					cmd->alias_of = aliased;
					break;
				}
				aliased++;
			}
		}
		assert(cmd->alias_of->description[0]); /*Every command should have a description.*/
		cmd++;
	}
}

/**
@param cmdtext the cmdtext line, may have arguments in it
@param parseidx passed so it can be used for further parsing the cmdtext line
@param out_cmd will be set to the cmd that matched. This can be an alias of the real cmd that should be used.
@param out_real_cmd will be set to the effective cmd. This may be the same as out_cmd if out_cmd wasn't an alias.
*/
static
int cmd_get_by_name_check_permissions(
	int playerid,
	char *cmdtext,
	int *out_parseidx,
	struct COMMAND **out_cmd,
	struct COMMAND **out_real_cmd)
{
	register struct COMMAND *cmd, *real_cmd;
	int hash;

	hash = strhashcode(cmdtext);
	cmd = cmds;
	while (cmd->cmd) {
		if (hash == cmd->hash && cmd_is(cmdtext, cmd->cmd, out_parseidx)) {
			real_cmd = cmd->alias_of;
			if (pdata[playerid]->groups & real_cmd->groups) {
				*out_cmd = cmd;
				*out_real_cmd = real_cmd;
				return 1;
			}
			break;
		}
		cmd++;
	}
	return 0;
}

/**
Called from hooked commandtext response packet handler.
*/
void hook_cmd_on_cmdtext(short playerid, char *cmdtext)
{
	struct COMMAND *cmd, *real_cmd;
	struct COMMANDCONTEXT cmdctx;
	char syntaxmsg[144];

	if (anticheat_on_player_command(playerid)) {
		return;
	}

	if (!ISPLAYING(playerid)) {
		SendClientMessage(playerid, COL_WARN, WARN"Log in first.");
		return;
	}

	if (!spawned[playerid]) {
#if DEV
		if (!strcmp(cmdtext, "/spawn")) {
			SpawnPlayer(playerid);
			spawned[playerid] = 1;
			return;
		}
#endif
		SendClientMessage(playerid, COL_WARN, WARN"You can't use commands when not spawned.");
		return;
	}

	common_mysql_escape_string(cmdtext, cbuf144, 144 * sizeof(cell));
	csprintf(buf4096,
		"INSERT INTO cmdlog(player,loggedstatus,stamp,cmd) "
		"VALUES(IF(%d<1,NULL,%d),%d,UNIX_TIMESTAMP(),'%s')",
		userid[playerid],
		userid[playerid],
		loggedstatus[playerid],
		cbuf144);
	NC_mysql_tquery_nocb(buf4096a);

	if (cmd_get_by_name_check_permissions(playerid, cmdtext, &cmdctx.parseidx, &cmd, &real_cmd)) {
		cmdctx.playerid = playerid;
		cmdctx.cmdtext = cmdtext;
		if (real_cmd->handler(cmdctx) == CMD_SYNTAX_ERR) {
			sprintf(syntaxmsg, "Syntax: %s %s", cmd->cmd, real_cmd->syntax);
			SendClientMessage(playerid, COL_WARN, syntaxmsg);
		}
	} else {
		SendClientMessage(playerid, -1, "SERVER: Unknown command, check /cmds");
	}
}

/**
Prints a cmd's syntax and description for a player.

@param cmdname should start with the forward slash
*/
static
void cmd_show_help_for(int playerid, char *cmdname)
{
	struct COMMAND *cmd, *real_cmd;
	int unused_parseidx;
	char msg[144];

	if (cmd_get_by_name_check_permissions(playerid, cmdname, &unused_parseidx, &cmd, &real_cmd)) {
		if (real_cmd == cmd) {
			sprintf(msg, INFO"%s: %s", real_cmd->cmd, real_cmd->description);
		} else {
			sprintf(msg, INFO"%s (alias of %s): %s", cmd->cmd, real_cmd->cmd, real_cmd->description);
		}
		SendClientMessage(playerid, COL_INFO, msg);
		sprintf(msg, INFO"Syntax: %s %s", real_cmd->cmd, real_cmd->syntax);
		SendClientMessage(playerid, COL_INFO, msg);
	} else {
		SendClientMessage(playerid, COL_WARN, WARN"That command doesn't exist or is not available to you, check /cmds");
	}
}

#define CMDLIST_NUM_ENTRIES_PER_PAGE 17

STATIC_ASSERT(CMDLIST_NUM_ENTRIES_PER_PAGE * 150 < LIMIT_DIALOG_INFO);

struct CMDLIST_DATA {
	short page;
	unsigned char num_entries;
	char has_next_page_entry;
	struct COMMAND *entries[CMDLIST_NUM_ENTRIES_PER_PAGE];
};

static void cmd_show_cmdlist(int playerid, struct CMDLIST_DATA *data);

static
void cmd_engage_cmdlist(int playerid)
{
	cmd_show_cmdlist(playerid, NULL);
}

static
void cmd_cb_dlg_cmdlist_detail(int playerid, struct DIALOG_RESPONSE response)
{
	if (!response.aborted && response.response) {
		cmd_show_cmdlist(playerid, response.data);
	} else {
		free(response.data);
	}
}

static
void cmd_cb_dlg_cmdlist(int playerid, struct DIALOG_RESPONSE response)
{
	register struct COMMAND *cmd, *alias;
	struct DIALOG_INFO *dialog;
	struct CMDLIST_DATA *data;
	char *b, have_alias_already;

	if (response.aborted || !response.response) {
		free(response.data);
		return;
	}

	data = response.data;
	if (data->page) {
		response.listitem--;
	}
	if (response.listitem < 0) {
		data->page--;
		data->num_entries = 0;
		cmd_show_cmdlist(playerid, data);
	} else if (response.listitem >= CMDLIST_NUM_ENTRIES_PER_PAGE) {
		data->page++;
		data->num_entries = 0;
		cmd_show_cmdlist(playerid, data);
	} else if (response.listitem < data->num_entries) {
		dialog = alloca(sizeof(struct DIALOG_INFO));
		dialog_init_info(dialog);
		b = dialog->info;
		cmd = data->entries[response.listitem];
		/*Not showing alias here (like /helpcmd would) because it's put below.*/
		b += sprintf(b, "%s: %s\nSyntax: %s %s",
			cmd->cmd, cmd->alias_of->description, cmd->cmd, cmd->alias_of->syntax);
		cmd = cmd->alias_of;
		have_alias_already = 0;
		alias = cmds;
		while (alias->cmd) {
			if (alias->alias_of == cmd && alias != cmd) {
				if (!have_alias_already) {
					b += sprintf(b, "\n\nAliases of %s:", cmd->cmd);
					have_alias_already = 1;
				}
				b += sprintf(b, "\n\t%s", alias->cmd);
			}
			alias++;
		}
		dialog->transactionid = DLG_TID_CMDLIST;
		dialog->caption = "Command details";
		dialog->button1 = "Back";
		dialog->button2 = "Close";
		dialog->handler.options = DLG_OPT_NOTIFY_ABORTED;
		dialog->handler.data = data;
		dialog->handler.callback = cmd_cb_dlg_cmdlist_detail;
		dialog_show(playerid, dialog);
	}
}

static
void cmd_show_cmdlist(int playerid, struct CMDLIST_DATA *data)
{
	register struct COMMAND *cmd;
	struct DIALOG_INFO dialog;
	char *b;
	int first_cmd_idx, idx;

	idx = 0;
	if (!data) {
		data = malloc(sizeof(struct CMDLIST_DATA));
		data->page = 0;
		data->num_entries = 0;
	}
	if (!data->num_entries) {
		first_cmd_idx = data->page * CMDLIST_NUM_ENTRIES_PER_PAGE;
		data->has_next_page_entry = 0;
		cmd = cmds;
		while (cmd->cmd) {
			if (pdata[playerid]->groups & cmd->alias_of->groups) {
				if (idx > first_cmd_idx) {
					if (data->num_entries == CMDLIST_NUM_ENTRIES_PER_PAGE) {
						data->has_next_page_entry = 1;
						break;
					}
					data->entries[data->num_entries++] = cmd;
				}
				idx++;
			}
			cmd++;
		}
	}

	dialog_init_info(&dialog);
	b = dialog.info;
	if (data->page) {
		b += sprintf(b, " \t"ECOL_SAMP_GREY"< previous page\n");
	}
	for (idx = 0; idx < data->num_entries; idx++) {
		cmd = data->entries[idx];
		if (cmd->alias_of == cmd) {
			b += sprintf(b, "%s\t%s\n", cmd->cmd, cmd->description);
		} else {
			b += sprintf(b, "%s\t%s (alias of %s)\n", cmd->cmd, cmd->alias_of->description, cmd->alias_of->cmd);
		}
	}
	if (data->has_next_page_entry) {
		sprintf(b, " \t"ECOL_SAMP_GREY"next page >\n");
	}
	dialog.transactionid = DLG_TID_CMDLIST;
	dialog.style = DIALOG_STYLE_TABLIST;
	sprintf(dialog.caption, "Command list page %d", data->page + 1);
	dialog.button1 = "Select";
	dialog.button2 = "Close";
	dialog.handler.data = data;
	dialog.handler.options = DLG_OPT_NOTIFY_ABORTED;
	dialog.handler.callback = cmd_cb_dlg_cmdlist;
	dialog_show(playerid, &dialog);
}
