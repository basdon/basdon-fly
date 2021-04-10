struct COMMAND {
	int hash;
	const char *cmd;
	const int groups;
	const char *syntax;
	const char *description;
	/**
	return 0 to print the syntax error msg
	*/
	int (*handler)(struct COMMANDCONTEXT ctx);
};

/* see sharedsymbols.h for GROUPS_ definitions */
/* command must prefixed by forward slash and be lower case */
static struct COMMAND cmds[] = {
#ifdef DEV
	{ 0, "/*m", GROUPS_ALL, CMD__M_SYNTAX, CMD__M_DESC, cmd_dev__m },
	{ 0, "/cp", GROUPS_ALL, CMD_CP_SYNTAX, CMD_CP_DESC, cmd_dev_cp },
	{ 0, "/crashme", GROUPS_ALL, CMD_CRASHME_SYNTAX, CMD_CRASHME_DESC, cmd_dev_crashme },
	{ 0, "/drvc", GROUPS_ALL, CMD_DRVC_SYNTAX, CMD_DRVC_DESC, cmd_dev_disableremotevehiclecollisions },
	{ 0, "/fweather", GROUPS_ALL, CMD_FWEATHER_SYNTAX, CMD_FWEATHER_DESC, cmd_dev_fweather },
	{ 0, "/gt", GROUPS_ALL, CMD_GT_SYNTAX, CMD_GT_DESC, cmd_dev_gt },
	{ 0, "/jetpack", GROUPS_ALL, CMD_JETPACK_SYNTAX, CMD_JETPACK_DESC, cmd_dev_jetpack },
	{ 0, "/kickme", GROUPS_ALL, CMD_KICKME_SYNTAX, CMD_KICKME_DESC, cmd_dev_kickme },
	{ 0, "/kill", GROUPS_ALL, CMD_KILL_SYNTAX, CMD_KILL_DESC, cmd_dev_kill },
	{ 0, "/nweather", GROUPS_ALL, CMD_NWEATHER_SYNTAX, CMD_NWEATHER_DESC, cmd_dev_nweather },
	{ 0, "/owner", GROUPS_ALL, CMD_OWNER_SYNTAX, CMD_OWNER_DESC, cmd_dev_owner },
	{ 0, "/platform", GROUPS_ALL, CMD_PLATFORM_SYNTAX, CMD_PLATFORM_DESC, cmd_dev_platform },
	{ 0, "/sound", GROUPS_ALL, CMD_SOUND_SYNTAX, CMD_SOUND_DESC, cmd_dev_sound },
	{ 0, "/testmsgsplit", GROUPS_ALL, CMD_TESTMSGSPLIT_SYNTAX, CMD_TESTMSGSPLIT_DESC, cmd_dev_testmsgsplit },
	{ 0, "/testparpl", GROUPS_ALL, CMD_TESTPARPL_SYNTAX, CMD_TESTPARPL_DESC, cmd_dev_testparpl },
	{ 0, "/timecyc", GROUPS_ALL, CMD_TIMECYC_SYNTAX, CMD_TIMECYC_DESC, cmd_dev_timecyc },
	{ 0, "/timex", GROUPS_ALL, CMD_TIMEX_SYNTAX, CMD_TIMEX_DESC, cmd_dev_timex },
	{ 0, "/tweather", GROUPS_ALL, CMD_TWEATHER_SYNTAX, CMD_TWEATHER_DESC, cmd_dev_tweather },
	{ 0, "/v", GROUPS_ALL, CMD_V_SYNTAX, CMD_V_DESC, cmd_dev_v },
	{ 0, "/vdamage", GROUPS_ALL, CMD_VDAMAGE_SYNTAX, CMD_VDAMAGE_DESC, cmd_dev_vdamage },
	{ 0, "/vfl", GROUPS_ALL, CMD_VFL_SYNTAX, CMD_VFL_DESC, cmd_dev_vfl },
	{ 0, "/vhp", GROUPS_ALL, CMD_VHP_SYNTAX, CMD_VHP_DESC, cmd_dev_vhp },
	{ 0, "/vhpnan", GROUPS_ALL, CMD_VHPNAN_SYNTAX, CMD_VHPNAN_DESC, cmd_dev_vhpnan },
	{ 0, "/vhpninf", GROUPS_ALL, CMD_VHPNINF_SYNTAX, CMD_VHPNINF_DESC, cmd_dev_vhpninf },
	{ 0, "/vhppinf", GROUPS_ALL, CMD_VHPPINF_SYNTAX, CMD_VHPPINF_DESC, cmd_dev_vhppinf },
#endif /*DEV*/
	{ 0, "//getcar", GROUPS_ADMIN, CMD__GETCAR_SYNTAX, CMD__GETCAR_DESC, cmd__getcar },
	{ 0, "//goto", GROUPS_ADMIN, CMD__GOTO_SYNTAX, CMD__GOTO_DESC, cmd__goto },
	{ 0, "//gotorel", GROUPS_ADMIN, CMD__GOTOREL_SYNTAX, CMD__GOTOREL_DESC, cmd__gotorel },
	{ 0, "//makeanewpermanentpublicvehiclehere", GROUP_OWNER, CMD__MANPPVH_SYNTAX, CMD__MANPPVH_DESC,
		cmd__makeanewpermanentpublicvehiclehere },
	{ 0, "//respawn", GROUPS_ADMIN, CMD__RESPAWN_SYNTAX, CMD__RESPAWN_DESC, cmd__respawn },
	{ 0, "//rr", GROUPS_ADMIN, CMD__RR_SYNTAX, CMD__RR_DESC, cmd__rr },
	{ 0, "//streamdistance", GROUP_OWNER, CMD__STREAMDISTANCE_SYNTAX, CMD__STREAMDISTANCE_DESC, cmd__streamdistance },
	{ 0, "//tocar", GROUPS_ADMIN, CMD__TOCAR_SYNTAX, CMD__TOCAR_DESC, cmd__tocar },
	{ 0, "//tomsp", GROUPS_ADMIN, CMD__TOMSP_SYNTAX, CMD__TOMSP_DESC, cmd__tomsp },
	{ 0, "//vehinfo", GROUP_OWNER, CMD__VEHINFO_SYNTAX, CMD__VEHINFO_DESC, cmd__vehinfo },
	{ 0, "/adf", GROUPS_ALL, CMD_ADF_SYNTAX, CMD_ADF_DESC, cmd_adf },
	{ 0, "/androm", GROUPS_ALL, CMD_ANDROM_SYNTAX, CMD_ANDROM_DESC, cmd_androm },
	{ 0, "/andromada", GROUPS_ALL, CMD_ANDROM_SYNTAX, CMD_ANDROM_DESC,cmd_androm },
	{ 0, "/at400", GROUPS_ALL, CMD_AT400_SYNTAX, CMD_AT400_DESC, cmd_at400 },
	{ 0, "/beacons", GROUPS_ALL, CMD_BEACONS_SYNTAX, CMD_BEACONS_DESC, cmd_beacons },
	{ 0, "/camera", GROUPS_ALL, CMD_CAMERA_SYNTAX, CMD_CAMERA_DESC, cmd_camera },
	{ 0, "/cancelwork", GROUPS_ALL, CMD_S_SYNTAX, CMD_S_DESC, cmd_s },
	{ 0, "/changepassword", GROUPS_REGISTERED, CMD_CHANGEPASSWORD_SYNTAX, CMD_CHANGEPASSWORD_DESC,
		cmd_changepassword },
	{ 0, "/changelog", GROUPS_ALL, CMD_CHANGELOG_SYNTAX, CMD_CHANGELOG_DESC, cmd_changelog },
	{ 0, "/chute", GROUPS_ALL, CMD_CHUTE_SYNTAX, CMD_CHUTE_DESC, cmd_chute },
	{ 0, "/engine", GROUPS_ALL, CMD_ENGINE_SYNTAX, CMD_ENGINE_DESC, cmd_engine },
	{ 0, "/fix", GROUPS_ALL, CMD_REPAIR_DESC, CMD_REPAIR_DESC, cmd_repair },
	{ 0, "/getspray", GROUPS_ALL, CMD_GETSPRAY_SYNTAX, CMD_GETSPRAY_DESC, cmd_getspray },
	{ 0, "/helpkeys", GROUPS_ALL, CMD_HELPKEYS_SYNTAX, CMD_HELPKEYS_DESC, cmd_helpkeys },
	{ 0, "/ils", GROUPS_ALL, CMD_ILS_SYNTAX, CMD_ILS_DESC, cmd_ils },
	{ 0, "/irc", GROUPS_ALL, CMD_IRC_SYNTAX, CMD_IRC_DESC, cmd_irc },
	{ 0, "/loc", GROUPS_ALL, CMD_LOC_SYNTAX, CMD_LOC_DESC, cmd_loc },
	{ 0, "/nearest", GROUPS_ALL, CMD_NEAREST_SYNTAX, CMD_NEAREST_DESC, cmd_nearest },
	{ 0, "/me", GROUPS_ALL, CMD_ME_SYNTAX, CMD_ME_DESC, cmd_me },
	{ 0, "/metar", GROUPS_ALL, CMD_METAR_SYNTAX, CMD_METAR_DESC, cmd_metar },
	{ 0, "/mission", GROUPS_ALL, CMD_W_SYNTAX, CMD_W_DESC, cmd_w },
	{ 0, "/park", GROUPS_ALL, CMD_PARK_SYNTAX, CMD_PARK_DESC, cmd_park },
	{ 0, "/p", GROUPS_ALL, CMD_PREFS_SYNTAX, CMD_PREFS_DESC, cmd_prefs },
	{ 0, "/pm", GROUPS_ALL, CMD_PM_SYNTAX, CMD_PM_DESC, cmd_pm },
	{ 0, "/prefs", GROUPS_ALL, CMD_PREFS_SYNTAX, CMD_PREFS_DESC, cmd_prefs },
	{ 0, "/preferences", GROUPS_ALL, CMD_PREFS_SYNTAX, CMD_PREFS_DESC, cmd_prefs },
	{ 0, "/protip", GROUPS_ALL, CMD_PROTIP_SYNTAX, CMD_PROTIP_DESC, cmd_protip },
	{ 0, "/r", GROUPS_ALL, CMD_R_SYNTAX, CMD_R_DESC, cmd_r },
	{ 0, "/reclass", GROUPS_ALL, CMD_RECLASS_SYNTAX, CMD_RECLASS_DESC, cmd_reclass },
	{ 0, "/register", GROUP_GUEST, CMD_REGISTER_SYNTAX, CMD_REGISTER_DESC, cmd_register },
	{ 0, "/repair", GROUPS_ALL, CMD_REPAIR_SYNTAX, CMD_REPAIR_DESC, cmd_repair },
	{ 0, "/refuel", GROUPS_ALL, CMD_REFUEL_SYNTAX, CMD_REFUEL_DESC, cmd_refuel },
	{ 0, "/respawn", GROUPS_ALL, CMD_RESPAWN_SYNTAX, CMD_RESPAWN_DESC, cmd_respawn },
	{ 0, "/s", GROUPS_ALL, CMD_S_SYNTAX, CMD_S_DESC, cmd_s },
	{ 0, "/spray", GROUPS_ALL, CMD_SPRAY_SYNTAX, CMD_SPRAY_DESC, cmd_spray },
	{ 0, "/stoplocate", GROUPS_ALL, CMD_STOPLOCATE_SYNTAX, CMD_STOPLOCATE_DESC, cmd_stoplocate },
	{ 0, "/stopwork", GROUPS_ALL, CMD_S_SYNTAX, CMD_S_DESC, cmd_s },
	{ 0, "/tickrate", GROUPS_ALL, CMD_TICKRATE_SYNTAX, CMD_TICKRATE_DESC, cmd_tickrate },
	{ 0, "/updates", GROUPS_ALL, CMD_CHANGELOG_SYNTAX, CMD_CHANGELOG_DESC, cmd_changelog },
	{ 0, "/vor", GROUPS_ALL, CMD_VOR_SYNTAX, CMD_VOR_DESC, cmd_vor },
	{ 0, "/w", GROUPS_ALL, CMD_W_SYNTAX, CMD_W_DESC, cmd_w },
	{ 0, "/weather", GROUPS_ALL, CMD_METAR_SYNTAX, CMD_METAR_DESC, cmd_metar },
	{ 0, 0, 0, 0, 0, 0 },
};

/*
Hashes command part of command text (case-insensitive).
End delimiter for the command part is either a zero terminator, or anything
with a value below the space character.
*/
static
int cmd_hash(const char *cmdtext)
{
	int val, pos = 0, result = 0;

	/* same as hashCode in Java (but case insensitive) */
	while (cmdtext[pos] != 0 && cmdtext[pos] > ' ') {
		val = cmdtext[pos++];
		if ('A' <= val && val <= 'Z') {
			val |= 0x20;
		}
		result = 31 * result + val;
	}
	return result;
}

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
Precalcs all command hashes.
*/
static
void cmd_init()
{
	register struct COMMAND *cmd;

	cmd = cmds;
	while (cmd->cmd) {
		cmd->hash = cmd_hash(cmd->cmd);
		cmd++;
	}
}

/**
Called from hooked commandtext response packet handler.
*/
void hook_cmd_on_cmdtext(short playerid, char *cmdtext)
{
	register struct COMMAND *cmd;
	struct COMMANDCONTEXT cmdctx;
	int hash;
	char syntaxmsg[144];

	if (!ISPLAYING(playerid)) {
		SendClientMessage(playerid, COL_WARN, WARN"Log in first.");
		return;
	}

	if (!spawned[playerid]) {
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

	hash = cmd_hash(cmdtext);

	cmd = cmds;
	while (cmd->cmd) {
		if (hash == cmd->hash &&
			(pdata[playerid]->groups & cmd->groups) &&
			cmd_is(cmdtext, cmd->cmd, &cmdctx.parseidx))
		{
			cmdctx.playerid = playerid;
			cmdctx.cmdtext = cmdtext;
			if (!cmd->handler(cmdctx)) {
				sprintf(syntaxmsg, "Syntax: %s %s", cmd->cmd, cmd->syntax);
				SendClientMessage(playerid, COL_WARN, syntaxmsg);
			}
			return;
		}
		cmd++;
	}
	SendClientMessage(playerid, -1, "SERVER: Unknown command.");
}
