struct RADIO_MACRO {
	int hash;
	char *macro;
	char *replacement;
};

static struct RADIO_MACRO radio_macros[] = {
	{ 0, "l", "Landing" },
	{ 0, "l2", "Landed" },
	{ 0, "t", "Taking off" },
	{ 0, "t2", "Took off" },
	{ 0, "ab", "Airborn" },
	{ 0, "rf", "Refueling" },
	{ 0, "ga", "Go around" },
	{ 0, "ga2", "Going around" },
	{ 0, "tx", "Taxiing" },
	{ 0, "er", "En route" },
	{ 0, "ap", "Airport" },
	{ 0, "app", "Approach" },
	{ 0, "sf", "Short final" },
	{ 0, "d", "Destination" },
	{ 0, "r", "Roger that" },
	{ 0, "c", "Copy that" },
	{ 0, "a", "Affirmative" },
	{ 0, "n", "Negative" },
	{ 0, "mm", "Mayday, Mayday!" },
	{ 0, "fl", "Flight level" },
	{ 0, "cp", "Do you copy?" },
	{ 0, "st", "What's your status?" },
	{ 0, "ty", "Thank you" },
	{ 0, "yw", "You're welcome" },
	{ 0, "lof", "Low on fuel" },
	{ 0, "rw", "Runway" },
#if MAX_AIRPORTS != 20
#error need more lines here
#endif
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 },
/*end*/
	{ 0, 0, 0 },
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
		radio_macros[j].replacement = airports[i].name;
	}

	/*Hash macros.*/
	for (i = 0; radio_macros[i].macro; i++) {
		radio_macros[i].hash = strhashcode(radio_macros[i].macro);
	}
}

static
void radio_on_msg(int playerid, char *msg)
{
	char radiomsg[200];
	char m, *p;
	int hash;
	int macrolen;
	int mention;
	int i;

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
					if (IsPlayerConnected(mention)) {
						p += sprintf(p, "%s", pdata[mention]->name);
						goto macro_ok;
					}
					break;
				}
				mention = mention * 10 + m - '0';
				macrolen++;
				msg++;
			}
			*p = '@';
		} else if (m == '#') {
			hash = strhashcode(msg);
			macrolen = 0;
			for (;;) {
				m = *msg;
				if (m <= ' ' || macrolen == 4) {
					if (macrolen) {
						for (i = 0; radio_macros[i].macro; i++) {
							if (radio_macros[i].hash == hash &&
								!strincmp(radio_macros[i].macro, msg - macrolen, macrolen))
							{
								p += sprintf(p, "%s", radio_macros[i].replacement);
								goto macro_ok;
							}
						}
					}
					break;
				}
				macrolen++;
				msg++;
			}
			*p = '#';
		} else {
			*p = m;
			p++;
			continue;
		}
		memcpy(p + 1, msg - macrolen, macrolen);
		p += macrolen + 1;
macro_ok:
		;
	}
	*p = 0;

	SendClientMessageToAll(COL_RADIO, radiomsg);
	echo_send_generic_message(ECHO_PACK12_RADIO, radiomsg);
}
