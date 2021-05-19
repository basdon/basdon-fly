struct RADIO_MACRO {
	int hash;
	char *macro;
	/**Description when {@link replacement_generator} is not null, replacement otherwise.*/
	char *replacement_or_description;
	int (*replacement_generator)(int playerid, char *str);
};

static
int radio_append_mission_origin(int playerid, char *str)
{
	struct MISSION *mission;

	mission = activemission[playerid];
	if (mission) {
		return sprintf(str, "%s (%s) %s",
			mission->startpoint->ap->name,
			mission->startpoint->ap->code,
			mission->startpoint->name
		);
	}
	return sprintf(str, "%s", "(no mission origin)");
}

static
int radio_append_mission_destination(int playerid, char *str)
{
	struct MISSION *mission;

	mission = activemission[playerid];
	if (mission) {
		return sprintf(str, "%s (%s) %s",
			mission->endpoint->ap->name,
			mission->endpoint->ap->code,
			mission->endpoint->name
		);
	}
	return sprintf(str, "%s", "(no mission destination)");
}

static struct RADIO_MACRO radio_macros[] = {
	{ 0, "l", "Landing", 0 },
	{ 0, "l2", "Landed", 0 },
	{ 0, "t", "Taking off", 0 },
	{ 0, "t2", "Took off", 0 },
	{ 0, "ab", "Airborn", 0 },
	{ 0, "rf", "Refueling", 0 },
	{ 0, "ga", "Go around", 0 },
	{ 0, "ga2", "Going around", 0 },
	{ 0, "tx", "Taxiing", 0 },
	{ 0, "er", "En route", 0 },
	{ 0, "ap", "Airport", 0 },
	{ 0, "app", "Approach", 0 },
	{ 0, "sf", "Short final", 0 },
	{ 0, "d", "Destination", 0 },
	{ 0, "r", "Roger that", 0 },
	{ 0, "c", "Copy that", 0 },
	{ 0, "a", "Affirmative", 0 },
	{ 0, "n", "Negative", 0 },
	{ 0, "mm", "Mayday, Mayday!", 0 },
	{ 0, "fl", "Flight level", 0 },
	{ 0, "cp", "Do you copy?", 0 },
	{ 0, "st", "What's your status?", 0 },
	{ 0, "ty", "Thank you", 0 },
	{ 0, "yw", "You're welcome", 0 },
	{ 0, "lof", "Low on fuel", 0 },
	{ 0, "rw", "Runway", 0 },
	{ 0, "mo", "(current mission origin)", radio_append_mission_origin },
	{ 0, "md", "(current mission destination)", radio_append_mission_destination },
#if MAX_AIRPORTS != 20
#error need more lines here
#endif
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
/*end*/
	{ 0, 0, 0, 0 },
};

static
void radio_init()
{
	int i, j;

	assert(numairports); /*Airports need to be loaded first.*/

	/*Add airport codes as macros.*/
	for (i = 0, j = 0; i < numairports; i++) {
		while (radio_macros[j].macro) {
			j++;
		}
		radio_macros[j].macro = airports[i].code;
		radio_macros[j].replacement_or_description = airports[i].name;
	}

	/*Hash macros.*/
	for (i = 0; radio_macros[i].macro; i++) {
		radio_macros[i].hash = strhashcode(radio_macros[i].macro);
	}
}

static
void radio_show_macros_dialog(int playerid)
{
	register struct RADIO_MACRO *macro;
	struct DIALOG_INFO dialog;
	char *b;

	dialog_init_info(&dialog);
	b = dialog.info;
	b += sprintf(b, "%s", "Macro\tReplacement");
	macro = radio_macros;
	while (macro->macro) {
		b += sprintf(b, "\n%%%s\t%s", macro->macro, macro->replacement_or_description);
		macro++;
	}
	dialog.transactionid = DLG_TID_RADIOMACROS;
	dialog.style = DIALOG_STYLE_TABLIST_HEADERS;
	dialog.caption = "Radio macros";
	dialog.button1 = "Ok";
	dialog_show(playerid, &dialog);
}

static
void radio_send_radio_msg(int playerid, char *msg)
{
	register struct RADIO_MACRO *macro;
	char radiomsg[200];
	char macrobuf[10];
	char m, *p;
	int hash;
	int macrolen;
	int mention;

	p = radiomsg;
	p += sprintf(p, "(RADIO:%s(%d)) ", pdata[playerid]->name, playerid);
	while ((m = *msg)) {
		msg++;
		if (m == '@') {
			macrolen = 0;
			mention = 0;
			for (;;) {
				m = *msg;
				if (m < '0' || m > '9' || macrolen > 5) {
					if (macrolen && IsPlayerConnected(mention)) {
						p += sprintf(p, "%s", pdata[mention]->name);
						goto macro_ok;
					}
					break;
				}
				mention = mention * 10 + m - '0';
				macrolen++;
				msg++;
			}
		} else if (m == '%') {
			macrolen = 0;
			for (;;) {
				m = *msg;
				if (!m || ((m < 'a' || m > 'z') && (m < 'A' || m > 'Z')) || macrolen == 4) {
					if (macrolen) {
						/*Need to copy to a zero term buf because the macro may end in , or any other symbol,
						and strhashcode would include that in the hash.*/
						memcpy(macrobuf, msg - macrolen, macrolen);
						macrobuf[macrolen] = 0;
						hash = strhashcode(macrobuf);
						macro = radio_macros;
						while (macro->macro) {
							if (macro->hash == hash &&
								!strincmp(macro->macro, msg - macrolen, macrolen))
							{
								if (macro->replacement_generator) {
									p += macro->replacement_generator(playerid, p);
								} else {
									p += sprintf(p, "%s", macro->replacement_or_description);
								}
								goto macro_ok;
							}
							macro++;
						}
					}
					break;
				}
				macrolen++;
				msg++;
			}
		} else {
			*p = m;
			p++;
			continue;
		}
		macrolen++; /*to include the prefix*/
		memcpy(p, msg - macrolen, macrolen);
		p += macrolen;
macro_ok:
		;
	}
	*p = 0;

	SendClientMessageToAll(COL_RADIO, radiomsg);
	echo_send_generic_message(ECHO_PACK12_RADIO, radiomsg);
}
