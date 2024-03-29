#define PROTIPS_INTERVAL (8 * 60000)

static const char *protips[] = {
	/*max length:*/
	/*(note: this is the max lengths before it would get split.
	 * Ideally protips should only be the length of one client message, but it can be more.*/
	/*--------------------------------------------------------------------------------------------------------------------------------------------*/
	"Tip: Don't like the aviation panel night colors? Disable it in /p(references).",
	"Tip: Use /w(ork) to find a mission. /s(top) to cancel your mission ($"SETTING__MISSION_CANCEL_FEE_STR" fee).",
	"Tip: Use /nearest to get a list of all airports, sorted by proximity!",
	"Tip: Enable navigation with /adf [beacon] or /vor [beacon][runway], see /helpnav",
	"Tip: ILS can be toggled using /ils when VOR is already active, see /helpils",
	"Tip: Press the 'CONVERSATION - NO' key (default: n, /helpkeys) to turn off your engine and preserve fuel.",
	"Tip: Confused about key bindings? Check out /helpkeys",
	"Tip: Check /p(references) to tweak your personal preferences while playing on this server.",
	"Tip: Send private messages to other players using /pm [id/name/part of name] [message]",
	"Tip: Can't get into an AT-400? We got you covered, just type /at400 when near one. (we also have /androm)",
	"Tip: Can't get into an Andromada? We got you covered, just type /androm when near one. (we also have /at400)",
	"Tip: Lost? Use /respawn to respawn.",
	"Tip: Always try to land on the back wheels.",
	"Tip: Use /reclass to change into a different class.",
	"Tip: All flights while on a mission are recorded, you can view them on the website!",
	"Tip: Want to fly as passenger in a plane without seats? Press 'FIRE' (default: lmb) while carrying no weapons to warp into the plane. See /helpcopilot",
	"Tip: Want to hide the gps/kneeboard text on your screen? Check out /p(references).",
	"Tip: The chat is linked to IRC, check out /irc for more info.",
	"Tip: Adjust the draw distance of player name tags using /p(references).",
	"Tip: Check out our fancy /help!",
};
static const int numprotips = sizeof(protips)/sizeof(protips[0]);

static
char *protips_get_random_protip()
{
	return (char*) protips[NC_random(numprotips)];
}

static
int protips_timed_broadcast(void *data)
{
	if (playercount) {
		SendClientMessageToAllAndIRC(ECHO_PACK12_PROTIP, COL_INFO_LIGHT, protips_get_random_protip());
	}
	return PROTIPS_INTERVAL;
}

static
void protips_init()
{
	timer_set(PROTIPS_INTERVAL, protips_timed_broadcast, NULL);
}
