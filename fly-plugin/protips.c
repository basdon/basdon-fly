#define PROTIPS_INTERVAL (8 * 60000)

static const char *protips[] = {
	/*max length:*/
	/*--------------------------------------------------------------------------------------------------------------------------------------------*/
#ifndef MISSION_CANCEL_FINE
#error need MISSION_CANCE_FINE
#endif
	"Pro Tip: Don't like the aviation panel night colors? Disable it in /p(references).",
	"Pro Tip: Use /w(ork) to find a mission. /s(top) to cancel your mission ($"EQ(MISSION_CANCEL_FINE)" fee).",
	"Pro Tip: Use /nearest to get a list of all airports, sorted by proximity!",
	"Pro Tip: Enable navigation with /adf [beacon] or /vor [beacon][runway]",
	"Pro Tip: ILS can be toggled using /ils when VOR is already active.",
	"Pro Tip: Press the 'CONVERSATION - NO' key (default: n, /helpkeys) to turn off your engine and preserve fuel.",
	"Pro Tip: Confused about key bindings? Check out /helpkeys",
	"Pro Tip: Check /p(references) to tweak your personal preferences while playing on this server.",
	"Pro Tip: Send private messages to other players using /pm [id/name/part of name] [message]",
	"Pro Tip: Can't get into an AT-400? We got you covered, just type /at400 when near one. (we also have /androm)",
	"Pro Tip: Can't get into an Andromada? We got you covered, just type /androm when near one. (we also have /at400)",
	"Pro Tip: Lost? Use /respawn to respawn.",
	"Pro Tip: Always try to land on the back wheels.",
	"Pro Tip: Use /reclass to change into a different class.",
	"Pro Tip: All flights while on a mission are recorded, you can watch them on the website!",
	"Pro Tip: Want to fly as passenger in a plane without seats? Press 'FIRE' (default: lmb) while carrying no weapons to warp into the plane.",
	"Pro Tip: Want to hide the gps/kneeboard text on your screen? Check out /p(references).",
	"Pro Tip: The chat is linked to IRC, check out /irc for more info.",
	"Pro Tip: Adjust the draw distance of player name tags using /p(references).",
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
	char *protip;

	if (playercount) {
		protip = protips_get_random_protip();
		SendClientMessageToAll(COL_INFO_LIGHT, protip);
		echo_send_generic_message(ECHO_PACK12_PROTIP, protip);
	}
	return PROTIPS_INTERVAL;
}

static
void protips_init()
{
	timer_set(PROTIPS_INTERVAL, protips_timed_broadcast, NULL);
}
