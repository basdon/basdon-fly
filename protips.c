
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "missions.h"
#include <string.h>

static const char *protips[] = {
	/*max length:*/
	/*--------------------------------------------------------------------*/
	/*--------------------------------------------------------------------*/
#ifndef MISSION_CANCEL_FINE
#error need MISSION_CANCE_FINE
#endif
	"Pro Tip: Use /w(ork) to start a random mission. /s(top) to cancel "
		"your mission ($"EQ(MISSION_CANCEL_FINE)" fine).",
	"Pro Tip: Use /nearest to get a list of all airports, sorted by "
		"proximity!",
	"Pro Tip: Enable navigation with /adf [beacon] or "
		"/vor [beacon][runway]",
	"Pro Tip: ILS can be toggled using /ils when VOR is already active.",
	"Pro Tip: Press the 'CONVERSATION - NO' key (default: n, /helpkeys) "
		"to turn off your engine and preserve fuel.",
	"Pro Tip: Confused about key bindings? Check out /helpkeys",
	"Pro Tip: Check /p(references) to tweak your personal preferences "
		"while playing on this server.",
	"Pro Tip: Use /autow to toggle automatically getting a new mission "
		"after finishing one (see also /p).",
	"Pro Tip: Send private messages to other players using "
		"/pm [id/name/part of name] [message]",
	"Pro Tip: Can't get into an AT-400? We got you covered, just type "
		"/at400 when near one.",
	"Pro Tip: Lost? Use /respawn to respawn.",
	"Pro Tip: Always try to land on the back wheels."
};
static const int numprotips = sizeof(protips)/sizeof(protips[0]);

void protips_timed_broadcast()
{
	static int delay = 0;

	if (delay++ == 8) {
		delay = 0;
		amx_SetUString(buf144, protips[NC_random(numprotips)], 144);
		NC_SendClientMessageToAll(COL_INFO_LIGHT, buf144a);
	}
}

int protips_cmd_protip(CMDPARAMS)
{
	amx_SetUString(buf144, protips[NC_random(numprotips)], 144);
	NC_SendClientMessage(playerid, COL_INFO_LIGHT, buf144a);
	return 1;
}
