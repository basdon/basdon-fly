STATIC_ASSERT(NUM_PANEL_TEXTDRAWS == 28);

#define PANEL_NIGHT_COLORS_FROM_HR 21
#define PANEL_NIGHT_COLORS_TO_HR 6

static struct TEXTDRAW td_panel_navtxt = { "navtxt", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_bg = { "bg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_spdbgs = { "spdbgs", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_spdvalue = { "spdvalue", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_spdsmall = { "spdsmall", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_spdlarge = { "spdlarge", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_altbgs = { "altbgs", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_altvalue = { "altvalue", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_altsmall = { "altsmall", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_altlarge = { "altlarge", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_heading = { "heading", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_headingbar = { "headingbar", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_vaibar = { "vaibar", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_navdis = { "navdis", 200, 0, NULL };
static struct TEXTDRAW td_panel_navalt = { "navalt", 200, 0, NULL };
static struct TEXTDRAW td_panel_navcrs = { "navcrs", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_fl = { "fl", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_flbarbg = { "flbarbg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_flbar = { "flbar", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_hp = { "hp", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_hpbarbg = { "hpbarbg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_hpbar = { "hpbar", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_odo = { "odo", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_gear = { "gear", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_vorbar = { "vorbar", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_vorbarind = { "vorbarind", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_vaiind = { "vaiind", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_panel_ils = { "ils", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };

struct PANELCACHE {
	short altitude_v;
	short altitude_s;
	short altitude_l;
	short speed_v;
	/*No speed_s, because if _v updates, _s will (nearly) always need an update as well.*/
	short speed_l;
	short heading;
	int odo;
	unsigned char fl_blink_state : 1;
	unsigned char hp_blink_state : 1;
	/**0 down 1 up 2 transitioning*/
	unsigned char gear_state : 2;
	/**0 hidden 1 variable pos 2 left 3 right*/
	unsigned char vor_shown : 2;
	/**0 hidden 1 no signal 2 signal*/
	unsigned char ils_shown : 2;
	char fuel_percent;
	char hp_percent;
	int nav_dist;
	int nav_alt;
	short nav_crs;
	int nav_vor;
	int nav_ils;
} caches[MAX_PLAYERS];

static int col_panel_bg[2] = { 0x99000000, 0x99000A19 };
static int col_panel_fg_light[2] = { 0xFFFFFFFF, 0xFF567EAF };
static int col_panel_fg_darker[2] = { 0xFF989898, 0xFF2A3E56 };
static int col_panel_fg_dark[2] = { 0xFF585858, 0xFF3E5B7F };
/**
0 none
1 day colors
2 night colors
*/
static char shown_panel[MAX_PLAYERS];

static float vai_y_base, vai_y_dev;
static float hdg_x_base, hdg_x_dev;
static float fl_hp_boxwidth_base, fl_hp_boxwidth_dev;
static float vorbar_x_base, vorbar_x_dev;
static float vorbar_lsx_inrange, vorbar_lsx_outrange;

/**
Players that should receive panel updates.
*/
static short panelplayers[MAX_PLAYERS];
/**
Amount of players int he panelplayers array.
*/
static int numpanelplayers;

/**
@return 1 if the player was actually a panel player
*/
static
int panel_remove_panel_player(int playerid)
{
	int i = numpanelplayers;

	while (i--) {
		if (panelplayers[i] == playerid) {
			panelplayers[i]	= panelplayers[--numpanelplayers];
			shown_panel[i] = 0;
			return 1;
		}
	}
	return 0;
}

/**
@return 1 if the player should receive panel updates, ie is the panel shown?
*/
static
int panel_is_active_for(int playerid)
{
	int i;

	for (i = 0; i < numpanelplayers; i++) {
		if (panelplayers[i] == playerid) {
			return 1;
		}
	}
	return 0;
}

/**
Update heading/altitude/speed textdraws.

Altitude:
  The large values show counts of 50 above and below with a dash next to it.
  The small values show counts of 10 above and below.
  Value should have leading zeros.
  +--------+
  |   100- | large
  |    50- | large
  |+-------+
  ||   20  | small
  ||+------+
  ||| 015  | value
  ||+------+
  ||   10  | small
  |+-------+
  |     0- | large
  |   -50- | large
  +--------+

Speed:
  The large values show counts of 10 above and below with a dash next to it.
  If 0 or below, show only the dash.
  The small values show one above and one below. If 0, do not show.
  Value should have leading zeros.
  +--------+
  |    30- | large
  |    20- | large
  |+-------+
  ||    5  | small
  ||+------+
  ||| 014  | value
  ||+------+
  ||    3  | small
  |+-------+
  |    10- | large
  |      - | large
  +--------+

@param is_update when 0, ignore cache and format into the td_ variables, otherwise check cache and send TextDrawSetString RPCs.
*/
static
void panel_update_hdg_alt_spd(int playerid, int heading, int altitude, int speed, int is_update)
{
	static const char SPD_METER_DATA[] =
		"160-~n~150-~n~140-~n~130-~n~120-~n~110-~n~100-~n~_90-~n~_"
		"80-~n~_70-~n~_60-~n~_50-~n~_40-~n~_30-~n~_20-~n~_10-~n~___-"
		"~n~___-~n~___~n~";

	struct BitStream bitstream;
	UNION_FREEFORM_RPCDATA(100) rpcdata;
	int altitude_s_cacheval, altitude50, spd_l_cacheval;
	char *alt_s_buf, *spd_s_buf;

	bitstream.ptrData = &rpcdata;

	/*heading*/
	heading = 360 - heading;
	if (heading == 0) {
		heading = 360;
	}
	if (!is_update || heading != caches[playerid].heading) {
		caches[playerid].heading = heading;

#ifdef PANEL_PRINT_UPDATES
		printf("  update heading\n");
#endif

		if (is_update) {
			/*hdg*/
			rpcdata.word[0] = td_panel_heading.rpcdata->textdrawid;
			rpcdata.word[1] = sprintf((char*) &rpcdata.word[2], "%03d", heading);
			bitstream.numberOfBitsUsed = (2 + 2 + rpcdata.word[1]) * 8;
			SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);

			/*hdg meter*/
			rpcdata.word[0] = td_panel_headingbar.rpcdata->textdrawid;
			rpcdata.word[1] = sprintf((char*) &rpcdata.word[2],
				"%03d_%03d_%03d________%03d_%03d_%03d",
				((heading + 356) % 360) + 1,
				((heading + 357) % 360) + 1,
				((heading + 358) % 360) + 1,
				((heading + 0) % 360) + 1,
				((heading + 1) % 360) + 1,
				((heading + 2) % 360) + 1);
			bitstream.numberOfBitsUsed = (2 + 2 + rpcdata.word[1]) * 8;
			SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);
		} else {
			/*hdg*/
			td_panel_heading.rpcdata->text_length = sprintf(td_panel_heading.rpcdata->text, "%03d", heading);
			/*hdg meter*/
			td_panel_headingbar.rpcdata->text_length = sprintf(td_panel_headingbar.rpcdata->text,
				"%03d_%03d_%03d________%03d_%03d_%03d",
				((heading + 356) % 360) + 1,
				((heading + 357) % 360) + 1,
				((heading + 358) % 360) + 1,
				((heading + 0) % 360) + 1,
				((heading + 1) % 360) + 1,
				((heading + 2) % 360) + 1);
		}
	}

	/*altitude*/
	if (altitude > 9000) {
		altitude = 9000;
	} else if (altitude < -850) {
		altitude = -850;
	}
	if (!is_update || altitude != caches[playerid].altitude_v) {
		caches[playerid].altitude_v = altitude;

#ifdef PANEL_PRINT_UPDATES
		printf("  update alt_v\n");
#endif

		/*alt_value*/
		if (is_update) {
			rpcdata.word[0] = td_panel_altvalue.rpcdata->textdrawid;
			rpcdata.word[1] = sprintf((char*) &rpcdata.word[2], "%03d", altitude);
			bitstream.numberOfBitsUsed = (2 + 2 + rpcdata.word[1]) * 8;
			SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);
		} else {
			td_panel_altvalue.rpcdata->text_length = sprintf(td_panel_altvalue.rpcdata->text, "%03d", altitude);
		}

		/*alt_small*/
		altitude_s_cacheval = (altitude / 10 + 100) % 10;
		if (altitude < 0) {
			altitude_s_cacheval += 20000;
		}
		if (!is_update || (short) altitude_s_cacheval != caches[playerid].altitude_s) {
			caches[playerid].altitude_s = (short) altitude_s_cacheval;

#ifdef PANEL_PRINT_UPDATES
			printf("  update alt_s\n");
#endif

			if (is_update) {
				alt_s_buf = (char*) &rpcdata.word[2];
			} else {
				alt_s_buf = td_panel_altsmall.rpcdata->text;
			}

			memcpy(alt_s_buf, "_00~n~~n~_00", 12);
			if (altitude >= 0) {
				alt_s_buf[1] = '0' + ((altitude + 10) / 10) % 10;
				alt_s_buf[10] = '0' + ((altitude) / 10) % 10;
			} else {
				if (altitude <= -10) {
					alt_s_buf[0] = '-';
				}
				alt_s_buf[9] = '-';
				alt_s_buf[1] = '0' + ((-altitude) / 10) % 10;
				alt_s_buf[10] = '0' + ((-altitude + 10) / 10) % 10;
			}

			if (is_update) {
				rpcdata.word[0] = td_panel_altsmall.rpcdata->textdrawid;
				rpcdata.word[1] = 12;
				bitstream.numberOfBitsUsed = (2 + 2 + 12) * 8;
				SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);
			} else {
				td_panel_altsmall.rpcdata->text_length = 12;
			}
		}

		/*alt_large*/
		altitude50 = altitude / 50;
		if (altitude < 0) {
			/*Adjustment is needed when altitude is negative.*/
			altitude50 -= 1;
		}
		if (!is_update || (short) altitude50 != caches[playerid].altitude_l) {
			caches[playerid].altitude_l = (short) altitude50;

#ifdef PANEL_PRINT_UPDATES
			printf("  update alt_l\n");
#endif

			if (is_update) {
				rpcdata.word[0] = td_panel_altlarge.rpcdata->textdrawid;
				rpcdata.word[1] = sprintf((char*) &rpcdata.word[2],
					"%4d-~n~%4d-~n~~n~~n~~n~~n~%4d-~n~%4d-~n~",
					(altitude50 + 2) * 50,
					(altitude50 + 1) * 50,
					(altitude50 + 0) * 50,
					(altitude50 - 1) * 50);
				bitstream.numberOfBitsUsed = (2 + 2 + rpcdata.word[1]) * 8;
				SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);
			} else {
				td_panel_altlarge.rpcdata->text_length = sprintf(td_panel_altlarge.rpcdata->text,
					"%4d-~n~%4d-~n~~n~~n~~n~~n~%4d-~n~%4d-~n~",
					(altitude50 + 2) * 50,
					(altitude50 + 1) * 50,
					(altitude50 + 0) * 50,
					(altitude50 - 1) * 50);
			}
		}
	}

	/*speed*/
	if (!is_update || speed != caches[playerid].speed_v) {
		caches[playerid].speed_v = speed;

#ifdef PANEL_PRINT_UPDATES
		printf("  update spd_v & spd_s\n");
#endif

		/*spd_value*/
		if (is_update) {
			rpcdata.word[0] = td_panel_spdvalue.rpcdata->textdrawid;
			rpcdata.word[1] = sprintf((char*) &rpcdata.word[2], "%03d", speed);
			bitstream.numberOfBitsUsed = (2 + 2 + rpcdata.word[1]) * 8;
			SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);
		} else {
			td_panel_spdvalue.rpcdata->text_length = sprintf(td_panel_spdvalue.rpcdata->text, "%03d", speed);
		}

		/*0 and 149 are important boundary checks for code below.*/
		if (0 <= speed && speed < 150) {
			/*spd_small*/
			if (is_update) {
				spd_s_buf = (char*) &rpcdata.word[2];
			} else {
				spd_s_buf = td_panel_spdsmall.rpcdata->text;
			}
			memcpy(spd_s_buf, "0~n~~n~_", 8);
			spd_s_buf[0] = '0' + ((speed + 1) % 10);
			if (speed != 0) {
				spd_s_buf[7] = '0' + ((speed + 9) % 10);
			}
			if (is_update) {
				rpcdata.word[0] = td_panel_spdsmall.rpcdata->textdrawid;
				rpcdata.word[1] = 8;
				bitstream.numberOfBitsUsed = (2 + 2 + 8) * 8;
				SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);
			} else {
				td_panel_spdsmall.rpcdata->text_length = 8;
			}

			/*spd_large*/
			spd_l_cacheval = (14 - speed / 10) * 7;
			if (spd_l_cacheval != caches[playerid].speed_l) {
				caches[playerid].speed_l = spd_l_cacheval;

#ifdef PANEL_PRINT_UPDATES
				printf("  update spd_l\n");
#endif

				if (is_update) {
					memcpy((char*) &rpcdata.word[2], SPD_METER_DATA + spd_l_cacheval, 11);
					memcpy((char*) &rpcdata.word[2] + 11, "~n~~n~~n~~n~~n~", 15);
					memcpy((char*) &rpcdata.word[2] + 26, SPD_METER_DATA + spd_l_cacheval + 14, 14);
					rpcdata.word[0] = td_panel_spdlarge.rpcdata->textdrawid;
					rpcdata.word[1] = 40;
					bitstream.numberOfBitsUsed = (2 + 2 + 40) * 8;
					SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);
				} else {
					td_panel_spdlarge.rpcdata->text_length = 40;
					memcpy(td_panel_spdlarge.rpcdata->text, SPD_METER_DATA + spd_l_cacheval, 11);
					memcpy(td_panel_spdlarge.rpcdata->text + 11, "~n~~n~~n~~n~~n~", 15);
					memcpy(td_panel_spdlarge.rpcdata->text + 26, SPD_METER_DATA + spd_l_cacheval + 14, 14);
				}
			}
		}
	}
}

/**
@param is_update when 0, ignore cache and format into the td_ variables, otherwise check cache and send TextDrawSetString RPCs.
*/
static
void panel_update_odo_fl_hp_gear(int playerid, int vehicleid, int is_update)
{
	static char GEAR_STATE_COLOR_CHAR[3] = { 'g', 'r', 'y' };

	struct BitStream bitstream;
	UNION_FREEFORM_RPCDATA(50) rpcdata;
	int odo, fl_pct, hp_pct;
	float tmp_fuel_percent, health;
	struct dbvehicle *veh;
	char fl_blink_state, hp_blink_state;
	unsigned gear_state;

	bitstream.ptrData = &rpcdata;

	veh = gamevehicles[vehicleid].dbvehicle;
	if (veh) {
		tmp_fuel_percent = 100.0f * veh->fuel / model_fuel_capacity(veh->model);
		fl_pct = (int) tmp_fuel_percent;
		/*Some rounding stuff because otherwise it'll immediately be 99%*/
		if (tmp_fuel_percent - fl_pct > .49f) {
			fl_pct++;
		}
		odo = (int) veh->odoKM;
	} else {
		fl_pct = 0;
		odo = 0;
	}

	health = anticheat_GetVehicleHealth(vehicleid) - 250.0f;
	if (health < 0.0f) {
		hp_pct = 0;
	} else {
		hp_pct = (int) (health / 7.5f);
	}

	if (!is_update || odo != caches[playerid].odo) {
		caches[playerid].odo = odo;

#ifdef PANEL_PRINT_UPDATES
		printf("  update odo\n");
#endif

		if (is_update) {
			rpcdata.word[0] = td_panel_odo.rpcdata->textdrawid;
			rpcdata.word[1] = sprintf((char*) &rpcdata.word[2], "ODO %08d", odo);
			bitstream.numberOfBitsUsed = (2 + 2 + rpcdata.word[1]) * 8;
			SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);
		} else {
			td_panel_odo.rpcdata->text_length = sprintf(td_panel_odo.rpcdata->text, "ODO %08d", odo);
		}
	}

	fl_blink_state = fl_pct < 20 && (time_m % 2);
	if (!is_update || fl_blink_state != caches[playerid].fl_blink_state) {
		caches[playerid].fl_blink_state = fl_blink_state;

#ifdef PANEL_PRINT_UPDATES
		printf("  update fl text\n");
#endif

		if (is_update) {
			rpcdata.word[0] = td_panel_fl.rpcdata->textdrawid;
			rpcdata.word[1] = 2;
			rpcdata.byte[4] = 'F' + ('_' - 'F') * fl_blink_state;
			rpcdata.byte[5] = 'L' + ('_' - 'L') * fl_blink_state;
			bitstream.numberOfBitsUsed = (2 + 2 + 2) * 8;
			SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);
		} else {
			td_panel_fl.rpcdata->text[0] = 'F' + ('_' - 'F') * fl_blink_state;
			td_panel_fl.rpcdata->text[1] = 'L' + ('_' - 'L') * fl_blink_state;
		}
	}

	hp_blink_state = hp_pct < 20 && (time_m % 2);
	if (!is_update || hp_blink_state != caches[playerid].hp_blink_state) {
		caches[playerid].hp_blink_state = hp_blink_state;

#ifdef PANEL_PRINT_UPDATES
		printf("  update hp text\n");
#endif

		if (is_update) {
			rpcdata.word[0] = td_panel_hp.rpcdata->textdrawid;
			rpcdata.word[1] = 2;
			rpcdata.byte[4] = 'H' + ('_' - 'H') * hp_blink_state;
			rpcdata.byte[5] = 'P' + ('_' - 'P') * hp_blink_state;
			bitstream.numberOfBitsUsed = (2 + 2 + 2) * 8;
			SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);
		} else {
			td_panel_hp.rpcdata->text[0] = 'H' + ('_' - 'H') * hp_blink_state;
			td_panel_hp.rpcdata->text[1] = 'P' + ('_' - 'P') * hp_blink_state;
		}
	}

	if (!is_update || fl_pct != caches[playerid].fuel_percent) {
		caches[playerid].fuel_percent = fl_pct;

#ifdef PANEL_PRINT_UPDATES
		printf("  update fl bar\n");
#endif

		td_panel_flbar.rpcdata->box_width = fl_hp_boxwidth_base + fl_hp_boxwidth_dev * (fl_pct / 100.0f);

		if (is_update) {
			textdraws_show(playerid, 1, &td_panel_flbar);
		}
	}

	if (!is_update || hp_pct != caches[playerid].hp_percent) {
		caches[playerid].hp_percent = hp_pct;

#ifdef PANEL_PRINT_UPDATES
		printf("  update hp bar\n");
#endif

		td_panel_hpbar.rpcdata->box_width = fl_hp_boxwidth_base + fl_hp_boxwidth_dev * (hp_pct / 100.0f);

		if (is_update) {
			textdraws_show(playerid, 1, &td_panel_hpbar);
		}
	}

	if (vehicleflags[NC_GetVehicleModel(vehicleid) - VEHICLE_MODEL_MIN] & RETRACTABLE_GEAR) {
		gear_state = vehicle_gear_state[vehicleid];
		if (time_timestamp() - vehicle_gear_change_time[vehicleid] < 1500) {
			gear_state = 2;
		}
	} else {
		gear_state = 0;
	}
	if (!is_update || gear_state != caches[playerid].gear_state) {
		caches[playerid].gear_state = gear_state;

#ifdef PANEL_PRINT_UPDATES
		printf("  update gear\n");
#endif

		if (is_update) {
			rpcdata.word[0] = td_panel_gear.rpcdata->textdrawid;
			rpcdata.word[1] = td_panel_gear.rpcdata->text_length;
			memcpy((char*) &rpcdata.byte[4], td_panel_gear.rpcdata->text, rpcdata.word[1]);
			rpcdata.byte[5] = GEAR_STATE_COLOR_CHAR[gear_state];
			bitstream.numberOfBitsUsed = (2 + 2 + rpcdata.word[1]) * 8;
			SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);
		} else {
			td_panel_gear.rpcdata->text[1] = GEAR_STATE_COLOR_CHAR[gear_state];
		}
	}
}

static
void panel_update_nav_text_active(int playerid, struct RPCDATA_ShowTextDraw *textrpc, char *format, void *format_data, int is_update)
{
	struct BitStream bitstream;
	UNION_FREEFORM_RPCDATA(50) rpcdata;

	bitstream.ptrData = &rpcdata;

	if (is_update) {
		rpcdata.word[0] = textrpc->textdrawid;
		rpcdata.word[1] = sprintf((char*) &rpcdata.word[2], format, format_data);
		bitstream.numberOfBitsUsed = (2 + 2 + rpcdata.word[1]) * 8;
		SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);
	} else {
		textrpc->text_length = sprintf(textrpc->text, format, format_data);
	}
}

static
void panel_update_nav_text_inactive(int playerid, struct RPCDATA_ShowTextDraw *textrpc, int is_update)
{
	struct BitStream bitstream;
	UNION_FREEFORM_RPCDATA(50) rpcdata;

	bitstream.ptrData = &rpcdata;

	if (is_update) {
		rpcdata.word[0] = textrpc->textdrawid;
		rpcdata.word[1] = 1;
		rpcdata.byte[4] = '-';
		bitstream.numberOfBitsUsed = (2 + 2 + 1) * 8;
		SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);
	} else {
		textrpc->text_length = 1;
		textrpc->text[0] = '-';
	}
}

/**
@param is_update when 0, ignore cache and format into the td_ variables, otherwise check cache and send TextDrawSetString RPCs.
*/
static
void panel_update_nav(int playerid, int vehicleid, int is_update)
{
	#define ILS_TEXT_LEN 124

	static const unsigned char ILS_Z_OFFSETS[] = { 8, 15, 22, 29, 61, 93, 100, 107, 114 };

	struct BitStream bitstream;
	UNION_FREEFORM_RPCDATA(ILS_TEXT_LEN) rpcdata;
	struct NAVDATA *navdata;
	int new_dist, new_alt, new_crs;
	char dist_buf[30];
	char vorbar_text[30];
	float vorbar_lsx;
	float vorbar_x;
	unsigned char vorbar_pos;
	short vorbar_textlen;
	int vorcachevalue;
	int vor_inrange;
	int ilscachevalue;
	unsigned char ilstype_new;
	char ils_buf[ILS_TEXT_LEN + 2];
	short ils_len;

	bitstream.ptrData = &rpcdata;
	navdata = nav[vehicleid];

	/*dist*/
	if (navdata) {
		new_dist = (float) navdata->dist;
		if (new_dist > 1000) {
			new_dist = new_dist / 100 * 100;
		}
	} else {
		new_dist = 2000000000;
	}
	if (!is_update || caches[playerid].nav_dist != new_dist) {
		caches[playerid].nav_dist = new_dist;

#ifdef PANEL_PRINT_UPDATES
		printf("  update nav_dist\n");
#endif

		if (navdata) {
			if (new_dist >= 1000) {
				sprintf(dist_buf, "%.1fKm", (float) new_dist / 1000.0f);
			} else {
				sprintf(dist_buf, "%d", new_dist);
			}
			panel_update_nav_text_active(playerid, td_panel_navdis.rpcdata, dist_buf, NULL, is_update);
		} else {
			panel_update_nav_text_inactive(playerid, td_panel_navdis.rpcdata, is_update);
		}
	}

	/*alt*/
	if (navdata) {
		new_alt = navdata->alt;
	} else {
		new_alt = 2000000000;
	}
	if (!is_update || caches[playerid].nav_alt != new_alt) {
		caches[playerid].nav_alt = new_alt;

#ifdef PANEL_PRINT_UPDATES
		printf("  update nav_alt\n");
#endif

		if (navdata) {
			panel_update_nav_text_active(playerid, td_panel_navalt.rpcdata, "%d", (void*) new_alt, is_update);
		} else {
			panel_update_nav_text_inactive(playerid, td_panel_navalt.rpcdata, is_update);
		}
	}

	/*crs*/
	if (navdata) {
		new_crs = navdata->crs;
	} else {
		new_crs = 20000;
	}
	if (!is_update || caches[playerid].nav_crs != new_crs) {
		caches[playerid].nav_crs = new_crs;

#ifdef PANEL_PRINT_UPDATES
		printf("  update nav_crs\n");
#endif

		if (navdata) {
			/*The | is a degree symbol in the right textdraw font.*/
			panel_update_nav_text_active(playerid, td_panel_navcrs.rpcdata, "%d|", (void*) new_crs, is_update);
		} else {
			panel_update_nav_text_inactive(playerid, td_panel_navcrs.rpcdata, is_update);
		}
	}

	/*vorbar*/
	if (navdata && navdata->vorvalue != INVALID_VOR_VALUE) {
		if (-50 < navdata->vorvalue && navdata->vorvalue < 50) {
			vorcachevalue = navdata->vorvalue + 2000000;
			vor_inrange = 1;
		} else {
			if (navdata->vorvalue < 0) {
				vorcachevalue = (navdata->vorvalue + 50) / -15 + 1;
			} else {
				vorcachevalue = (navdata->vorvalue - 50) / 15 + 1;
			}
			vor_inrange = 0;
		}

		if (caches[playerid].nav_vor != vorcachevalue || !caches[playerid].vor_shown) {
			caches[playerid].nav_vor = vorcachevalue;

#ifdef VOR_PRINT_UPDATES
			printf("  update nav_vor\n");
#endif

			if (vor_inrange) {
				vorbar_textlen = 1;
				vorbar_text[0] = 'i';
				vorbar_x = vorbar_x_base - (float) navdata->vorvalue / 50.0f * vorbar_x_dev;
				vorbar_lsx = vorbar_lsx_inrange;
				vorbar_pos = 1;
			} else {
				vorbar_textlen = sprintf(vorbar_text, "%d", vorcachevalue);
				if (navdata->vorvalue < 0) {
					vorbar_x = vorbar_x_base + vorbar_x_dev;
					vorbar_pos = 2;
				} else {
					vorbar_x = vorbar_x_base - vorbar_x_dev;
					vorbar_pos = 3;
				}
				vorbar_lsx = vorbar_lsx_outrange;
			}

			if (vorbar_pos == 1 || caches[playerid].vor_shown != vorbar_pos) {
#ifdef VOR_PRINT_UPDATES
				printf("    reshow vor\n");
#endif
				td_panel_vorbarind.rpcdata->text_length = vorbar_textlen;
				memcpy(td_panel_vorbarind.rpcdata->text, vorbar_text, vorbar_textlen);
				td_panel_vorbarind.rpcdata->x = vorbar_x;
				td_panel_vorbarind.rpcdata->font_width = vorbar_lsx;
				textdraws_show(playerid, 1, &td_panel_vorbarind);
			} else {
#ifdef VOR_PRINT_UPDATES
				printf("    textonly vor\n");
#endif
				rpcdata.word[0] = td_panel_vorbarind.rpcdata->textdrawid;
				rpcdata.word[1] = vorbar_textlen;
				memcpy(&rpcdata.word[2], vorbar_text, vorbar_textlen);
				bitstream.numberOfBitsUsed = (2 + 2 + vorbar_textlen) * 8;
				SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);
			}
			caches[playerid].vor_shown = vorbar_pos;
		}
	} else {
		if (caches[playerid].vor_shown) {
			caches[playerid].vor_shown = 0;
			textdraws_hide_consecutive(playerid, 1, td_panel_vorbarind.rpcdata->textdrawid);
#ifdef VOR_PRINT_UPDATES
				printf("    hide vor\n");
#endif
		}
	}

	/*ils*/
	if (navdata && navdata->ils) {
		ilscachevalue = ((unsigned char) navdata->ilsx << 8) | (unsigned char) navdata->ilsz;
		if (caches[playerid].nav_ils != ilscachevalue || !caches[playerid].ils_shown) {
			caches[playerid].nav_ils = ilscachevalue;

#ifdef ILS_PRINT_UPDATES
			printf("  update ils\n");
#endif

			if (navdata->ilsx == INVALID_ILS_VALUE) {
				ilstype_new = 1;
				td_panel_ils.rpcdata->flags |= TEXTDRAW_FLAG_PROPORTIONAL;
				ils_len = 19;
				strcpy(ils_buf, "~r~~h~no ILS signal");
			} else {
				ilstype_new = 2;
				td_panel_ils.rpcdata->flags &= ~TEXTDRAW_FLAG_PROPORTIONAL;
				ils_len = ILS_TEXT_LEN;
				strcpy(ils_buf,
					"~r~_~n~"
					"~w~X~n~"
					"~w~X~n~"
					"~w~X~n~"
					"~w~X~n~"
					"~r~_ "
					"~w~X ~w~X ~w~X ~w~X ~w~X ~w~X ~w~X ~w~X ~w~X"
					" ~r~_~n~"
					"~w~X~n~"
					"~w~X~n~"
					"~w~X~n~"
					"~w~X~n~"
					"~r~_");

				if (navdata->ilsx < 0) {
					ils_buf[38] = '0' + (-navdata->ilsx);
				} else if (navdata->ilsx >= ILS_SIZE) {
					ils_buf[88] = '0' + (navdata->ilsx - ILS_SIZE + 1);
				} else {
					ils_buf[41 + 5 * navdata->ilsx] = 'r';
				}

				if (navdata->ilsz < 0) {
					ils_buf[3] = '0' + (-navdata->ilsz);
				} else if (navdata->ilsz >= ILS_SIZE) {
					ils_buf[123] = '0' + (navdata->ilsz - ILS_SIZE + 1);
				} else {
					ils_buf[ILS_Z_OFFSETS[navdata->ilsz]] = 'r';
				}
			}

			if (caches[playerid].ils_shown != ilstype_new) {
#ifdef ILS_PRINT_UPDATES
				printf("    reshow ils %ld\n", time_timestamp());
#endif
				td_panel_ils.rpcdata->text_length = ils_len;
				memcpy(td_panel_ils.rpcdata->text, ils_buf, ils_len);
				textdraws_show(playerid, 1, &td_panel_ils);
			} else {
#ifdef ILS_PRINT_UPDATES
				printf("    textonly ils %ld\n", time_timestamp());
#endif
				rpcdata.word[0] = td_panel_ils.rpcdata->textdrawid;
				rpcdata.word[1] = ils_len;
				memcpy(&rpcdata.word[2], ils_buf, ils_len);
				bitstream.numberOfBitsUsed = (2 + 2 + ils_len) * 8;
				SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream, playerid, 2);
			}
			caches[playerid].ils_shown = ilstype_new;
		}
	} else {
		if (caches[playerid].ils_shown) {
			caches[playerid].ils_shown = 0;
			textdraws_hide_consecutive(playerid, 1, td_panel_ils.rpcdata->textdrawid);
#ifdef ILS_PRINT_UPDATES
				printf("    hide ils\n");
#endif
		}
	}
}

static
void panel_timed_update()
{
	struct vec3 vpos;
	int playerid, vehicleid, n = numpanelplayers;
	float x, y, z;
	int heading, altitude, speed;

	while (n--) {
#ifdef PANEL_PRINT_UPDATES
		printf("panel_timed_update\n");
#endif
		playerid = panelplayers[n];

		vehicleid = NC_GetPlayerVehicleID(playerid);

		common_GetVehiclePos(vehicleid, &vpos);
		kneeboard_update_distance(playerid, &vpos);
		altitude = (int) vpos.z;
		NC_GetVehicleZAngle(vehicleid, buf144a);
		heading = (int) *fbuf144;
		NC_GetVehicleVelocity(vehicleid, buf32a, buf64a, buf144a);
		x = *fbuf32;
		y = *fbuf64;
		z = *fbuf144;
		speed = (int) (VEL_TO_KTS * (float) sqrt(x * x + y * y + z * z));
		if (NC_GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
			nav_update(vehicleid, &vpos, heading);
		}

		panel_update_hdg_alt_spd(playerid, heading, altitude, speed, 1);
		panel_update_odo_fl_hp_gear(playerid, vehicleid, 1);
		panel_update_nav(playerid, vehicleid, 1);

		/*vai*/
		z *= VEL_TO_KFPM * -14.5f;
		if (z > vai_y_dev) {
			z = vai_y_dev;
		} else if (z < -vai_y_dev) {
			z = -vai_y_dev;
		}
		z += vai_y_base;
		td_panel_vaiind.rpcdata->y = z;
		textdraws_show(playerid, 1, &td_panel_vaiind);
	}
}

/**
Call when navigation properties of a vehicles was changed, so the text can update.

Sure, it's updated on the timed panel update, but it's nice if it could be updated immediately.
*/
static
void panel_nav_updated(int vehicleid)
{
	int playerid, i;

	for (i = 0; i < numpanelplayers; i++) {
		playerid = panelplayers[i];
		if (NC_GetPlayerVehicleID(playerid) == vehicleid) {
			panel_update_nav(playerid, vehicleid, 1);
		}
	}
}

static
void panel_reshow(int playerid)
{
	struct vec3 vpos;
	int vehicleid;
	float x, y, z;
	int heading, altitude, speed;
	char new_shown_panel;
	unsigned char color;

	new_shown_panel = 1;
	if ((prefs[playerid] & PREF_PANEL_NIGHTCOLORS) &&
		(time_h < PANEL_NIGHT_COLORS_TO_HR || PANEL_NIGHT_COLORS_FROM_HR <= time_h))
	{
		new_shown_panel = 2;
	}
	if (shown_panel[playerid] == new_shown_panel) {
		return;
	}
	shown_panel[playerid] = new_shown_panel;
	color = new_shown_panel - 1;

	td_panel_bg.rpcdata->box_color = col_panel_bg[color];
	td_panel_spdbgs.rpcdata->box_color = col_panel_bg[color];
	td_panel_altbgs.rpcdata->box_color = col_panel_bg[color];
	td_panel_navtxt.rpcdata->font_color = col_panel_fg_light[color];
	td_panel_spdvalue.rpcdata->font_color = col_panel_fg_light[color];
	td_panel_spdsmall.rpcdata->font_color = col_panel_fg_dark[color];
	td_panel_spdlarge.rpcdata->font_color = col_panel_fg_darker[color];
	td_panel_altvalue.rpcdata->font_color = col_panel_fg_light[color];
	td_panel_altsmall.rpcdata->font_color = col_panel_fg_dark[color];
	td_panel_altlarge.rpcdata->font_color = col_panel_fg_darker[color];
	td_panel_heading.rpcdata->font_color = col_panel_fg_light[color];
	td_panel_headingbar.rpcdata->font_color = col_panel_fg_darker[color];
	td_panel_odo.rpcdata->font_color = col_panel_fg_light[color];
	td_panel_fl.rpcdata->font_color = col_panel_fg_light[color];
	td_panel_hp.rpcdata->font_color = col_panel_fg_light[color];
	td_panel_vorbar.rpcdata->font_color = col_panel_fg_light[color];
	td_panel_vaibar.rpcdata->font_color = col_panel_fg_light[color];

	vehicleid = NC_GetPlayerVehicleID(playerid);

	common_GetVehiclePos(vehicleid, &vpos);
	kneeboard_update_distance(playerid, &vpos);
	altitude = (int) vpos.z;
	NC_GetVehicleZAngle(vehicleid, buf144a);
	heading = (int) *fbuf144;
	NC_GetVehicleVelocity(vehicleid, buf32a, buf64a, buf144a);
	x = *fbuf32;
	y = *fbuf64;
	z = *fbuf144;
	speed = (int) (VEL_TO_KTS * (float) sqrt(x * x + y * y + z * z));
	panel_update_hdg_alt_spd(playerid, heading, altitude, speed, 0);
	panel_update_odo_fl_hp_gear(playerid, vehicleid, 0);
	panel_update_nav(playerid, vehicleid, 0);

#if NUM_PANEL_TEXTDRAWS - 3 != 25
#error
#endif
	/*-3 because vorbarind/vaiindic/ils are often recreated.*/
	textdraws_show(playerid, NUM_PANEL_TEXTDRAWS - 3,
		&td_panel_navtxt, &td_panel_bg, &td_panel_spdbgs,
		&td_panel_spdvalue, &td_panel_spdsmall, &td_panel_spdlarge,
		&td_panel_altbgs, &td_panel_altvalue, &td_panel_altsmall,
		&td_panel_altlarge, &td_panel_heading, &td_panel_headingbar,
		&td_panel_vaibar, &td_panel_navdis, &td_panel_navalt,
		&td_panel_navcrs, &td_panel_vorbar, &td_panel_fl,
		&td_panel_flbarbg, &td_panel_flbar, &td_panel_hp,
		&td_panel_hpbarbg, &td_panel_hpbar, &td_panel_odo,
		&td_panel_gear);
}

void panel_on_player_state_change(int playerid, int from, int to)
{
	struct vec3 vpos;
	int vehicleid, heading;

	if (to == PLAYER_STATE_DRIVER || to == PLAYER_STATE_PASSENGER) {
		vehicleid = NC_GetPlayerVehicleID(playerid);
		if (!vehicleid) {
			return;
		}
		if (!game_is_air_vehicle(NC_GetVehicleModel(vehicleid))) {
			return;
		}

		if (to == PLAYER_STATE_DRIVER) {
			NC_GetVehicleZAngle(vehicleid, buf144a);
			heading = (int) *fbuf144;
			common_GetVehiclePos(vehicleid, &vpos);
			nav_update(vehicleid, &vpos, heading);
		}

		panelplayers[numpanelplayers++] = playerid;
		panel_reshow(playerid);
	} else if (panel_remove_panel_player(playerid)) {
		textdraws_hide_consecutive(playerid, NUM_PANEL_TEXTDRAWS, TEXTDRAW_PANEL_BASE);
		caches[playerid].vor_shown = 0;
		caches[playerid].ils_shown = 0;
	}
}

static
void panel_day_night_changed()
{
	int i;

	for (i = 0; i < numpanelplayers; i++) {
		panel_reshow(panelplayers[i]);
	}
}

void panel_on_player_was_afk(int playerid)
{
	int vehicleid, vehiclemodel;
	vehicleid = NC_GetPlayerVehicleID(playerid);
	if (vehicleid) {
		vehiclemodel = NC_GetVehicleModel(vehicleid);
		if (game_is_air_vehicle(vehiclemodel)) {
			panelplayers[numpanelplayers++] = playerid;
		}
	}
}

void panel_on_player_connect(int playerid)
{
	memset(&caches[playerid], 0xFF, sizeof(struct PANELCACHE));
	caches[playerid].vor_shown = 0;
	caches[playerid].ils_shown = 0;
}

static
void panel_init()
{
	struct TEXTDRAW tmp_td_panel_vaiindmax = { "vaiindmax(up)", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
	struct TEXTDRAW tmp_td_panel_vorbarindmax = { "vorbarindmax(left)", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };

	textdraws_load_from_file("panel", TEXTDRAW_PANEL_BASE, NUM_PANEL_TEXTDRAWS + 2,
		&td_panel_navtxt, &td_panel_bg, &td_panel_spdbgs, &td_panel_spdvalue, &td_panel_spdsmall,
		&td_panel_spdlarge, &td_panel_altbgs, &td_panel_altvalue, &td_panel_altsmall, &td_panel_altlarge,
		&td_panel_heading, &td_panel_headingbar, &td_panel_vaibar, &td_panel_navdis, &td_panel_navalt,
		&td_panel_navcrs, &td_panel_vorbar, &td_panel_fl,
		&td_panel_flbarbg, &td_panel_flbar, &td_panel_hp, &td_panel_hpbarbg, &td_panel_hpbar,
		&td_panel_odo, &td_panel_gear,
		/*These three need to be last since they are shown separately.*/
		&td_panel_vorbarind, &td_panel_vaiind, &td_panel_ils,
		/*These really need to be last, since they're just temporarily loaded to use their coordinates as measurement.*/
		&tmp_td_panel_vaiindmax, &tmp_td_panel_vorbarindmax);

	vai_y_base = td_panel_vaiind.rpcdata->y;
	vai_y_dev = td_panel_vaiind.rpcdata->y - tmp_td_panel_vaiindmax.rpcdata->y;
	hdg_x_base = td_panel_vorbarind.rpcdata->x;
	hdg_x_dev = tmp_td_panel_vorbarindmax.rpcdata->x - td_panel_vorbarind.rpcdata->x;

	fl_hp_boxwidth_base = td_panel_hpbar.rpcdata->box_width;
	fl_hp_boxwidth_dev = td_panel_hpbarbg.rpcdata->box_width - fl_hp_boxwidth_base;

	vorbar_lsx_inrange = td_panel_vorbarind.rpcdata->font_width;
	vorbar_lsx_outrange = tmp_td_panel_vorbarindmax.rpcdata->font_width;
	vorbar_x_base = td_panel_vorbarind.rpcdata->x;
	vorbar_x_dev = td_panel_vorbarind.rpcdata->x - tmp_td_panel_vorbarindmax.rpcdata->x;

	free(tmp_td_panel_vaiindmax.rpcdata);
	free(tmp_td_panel_vorbarindmax.rpcdata);
}
