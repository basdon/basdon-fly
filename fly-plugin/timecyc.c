/*

viewdistance can be /2 by night

VDIS viewdistance
CC   cloud coverage
FOGY foggyness
FOGD fogdensity
RAIN rain
SAND sandstorm
WIND wind
HH   heathaze
WAVE waveyness

+-----+-------+-------+-------+-------+-------+-------+-------+-------+-------+
| id  | VDIS  | CC    | FOGY  | FOGD  | RAIN  | SAND  | WIND  | HH    | WAVE  |
+-----+-------+-------+-------+-------+-------+-------+-------+-------+-------+
|  0  | LA Extra Sunny                                                        |
      | 800   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 1.0   | 0.3   |
|  1  | LA Sunny                                                              |
|     | 800   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.25  | 1.0   | 0.55  |
|  2  | LA Extra Sunny Smog                                                   |
|     | 800   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 1.0   | 0.3   |
|  3  | LA Sunny Smog                                                         |
|     | 800   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.2   | 1.0   | 0.5   |
|  4  | LA Cloudy                                                             |
|     | 700   | 1.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.7   | 0.0   | 1.0   |
|                                                                             |
|  5  | SF Sunny                                                              |
|     | 455   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.25  | 0.0   | 0.55  |
|  6  | SF Extra Sunny                                                        |
|     | 455   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.3   |
|  7  | SF Cloudy                                                             |
|     | 1150  | 1.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.7   | 0.0   | 1.0   |
|  8  | SF Rainy                                                              |
|     | 650   | 1.0   | 0.0   | 0.18  | 1.0   | 0.0   | 1.0   | 0.0   | 1.0   |
|  9  | SF Foggy                                                              |
|     | 250   | 1.0   | 1.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.3   |
|                                                                             |
| 10  | LV Sunny                                                              |
|     | 1000  | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.2   | 0.0   | 0.5   |
| 11  | LV Extra Sunny                                                        |
|     | 1000  | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 1.0   | 0.3   |
| 12  | LV Cloudy                                                             |
|     | 1000  | 1.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.4   | 0.0   | 0.7   |
|                                                                             |
| 13  | CS Extra Sunny                                                        |
|     | 1500  | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.3   |
| 14  | CS Sunny                                                              |
|     | 1500  | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.3   | 0.0   | 0.6   |
| 15  | CS Cloudy                                                             |
|     | 1150  | 1.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.7   | 0.0   | 1.0   |
| 16  | CS Rainy                                                              |
|     | 650   | 1.0   | 0.0   | 0.2   | 1.0   | 0.0   | 1.0   | 0.0   | 1.0   |
|                                                                             |
| 17  | DE Extra Sunny                                                        |
|     | 1500  | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 1.0   | 0.3   |
| 18  | DE Sunny                                                              |
|     | 1500  | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.3   | 1.0   | 0.6   |
| 19  | DE Sandstorm                                                          |
|     | 150   | 1.0   | 1.0   | 0.0   | 0.0   | 1.0   | 1.5   | 0.0   | 1.0   |
|                                                                             |
| 20  | Underwater                                                            |
|     | 300   | 1.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.3   |
+-----+-----------------------+-------+-------+-------+-------+-------+-------+

0: clear skies
1: light clouds
2: clear skies
3: light clouds
4: overcast
5: light clouds
6: clear skies
7: overcast
8: thunderstorms
9: thick fog
10: light clouds
11: clear skies
12: overcast
13: clear skies
14: light clouds
15: overcast
16: thunderstorms
17: clear skies
18: light clouds
19: sandstorms
20: green smog (don't use this - water looks lime toxic green when using 16 bit color depth)

default:
6 28.5% clear
6 28.5% light clouds
4 19.0% overcast
2 9.5% thunderstorms
2 9.5% thick fog
1 4.7% sandstorms

weighted:
9 25.7%
9 25.7%
8 22.8%
6 17.1%
2  5.7%
1  2.8%
35

changed 2020 Jan 27:
9 26.4%
9 26.4%
9 26.4%
5 14.7%
1  2.9%
1  2.9%
34

changed 2026 Jun 20:
9 29.0%
9 29.0%
9 29.0%
4 12.9%
0  0.0%
0  0.0%
31

wind * 34kts

 // from sa-mp forums: https://forum.sa-mp.com/showthread.php?t=285077
 0: Sunny Skies |[Visibility]:High |[Winds]:NA |[Clouds]:Moderate
 1: Moderate Sunny Skies [Visibility]:High [Winds]:NA [Clouds]:Moderate
 2: Sunny Skies [Visibility]:High [Winds]:NA [Clouds]:Moderate
 3: Sunny Skies [Visibility]:High [Winds]:NA [Clouds]:Moderate
 4: Sunny Skies |[Visibility]:High |[Winds]:NA |[Clouds]:Moderate
 5: Sunny Skies |[Visibility]:High |[Winds]:NA |[Clouds]:Moderate
 6:
 7: Sunny Skies |[Visibility]:High |[Winds]:NA |[Clouds]:Moderate
 8: Wet Rainy Weather |[Visibility]:Medium |[Winds]:Moderate |[Clouds]:Heavy
 9: Thick Fog |[Visibility]:Low |[Winds]:Moderate |[Clouds]:Heavy
10: Moderate Sunny Skies |[Visibility]:High |[Winds]:NA |[Clouds]:Moderate
11: HeatWave |[Visibility]:High |[Winds]:NA |[Clouds]:Moderate
12: Hazy/Dull Weather |[Visibility]:Moderate |[Winds]:NA |[Clouds]:High
13: Hazy/Dull Weather |[Visibility]:Moderate |[Winds]:NA |[Clouds]:High
14: Hazy/Dull Weather |[Visibility]:Moderate |[Winds]:NA |[Clouds]:High
15: Heavy RainStorm |[Visibility]:Low |[Winds]:Very High |[Clouds]: very Thick
16: Scorching Hot Bright Weather |[Visibility]:High |[Winds]:NA |[Clouds]:Low
17: Scorching Hot Bright Weather |[Visibility]:High |[Winds]:NA |[Clouds]:Low
18: Scorching Hot Bright Weather |[Visibility]:High |[Winds]:NA |[Clouds]:Low
19: Sand Storm |[Visibility]:Very-Low |[Winds]: High Speed |[Clouds]:Heavy
20: Toxic Green Smog |[Visibility]:Low |[Winds]:Moderate |[Clouds]:Heavy

weather table 0x8D5EB0

CS | 13, 13, 13, 13, 13, 13, 13, 13, 14, 15,  9,  9, 15,
     14, 13, 14, 14, 13, 13, 13, 13, 14, 14, 13, 13
LA |  2,  2,  0,  0,  0,  2,  2,  2,  3,  0,  3,  2,  0,
      0,  3,  3,  3,  4,  4,  0,  0,  1,  1,  0,  2
SF |  5,  5,  5,  5,  5,  7,  7,  9,  9,  7,  5,  5,  5,
      5,  5,  7,  7,  7,  8,  8,  7,  7,  5,  5,  5
LV | 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
     11, 11, 11, 11, 11, 11, 10, 10, 10, 10, 10, 11
DE | 17, 17, 17, 19, 19, 17, 17, 17, 17, 17, 17, 17, 17,
     17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 17

CS | 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14,
     14, 14, 15,  9,  9, 15, 16, 16, 16, 16, 15, 14
LA |  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,
      3,  3,  2,  2,  3,  3,  0,  0, 16,  0,  4,  1
SF |  5,  6,  6,  5,  7,  9,  9,  7,  5,  5,  7,  5,  5,
      6,  6,  5,  5,  5,  7,  7,  8,  8,  7,  7,  9
LV | 11, 11, 11, 11, 11, 11, 11, 11, 10, 11, 11, 11, 11,
     10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11
DE | 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 19,
     19, 19, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17

CS | 14, 14, 14, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13
LA |  1,  1,  1,  0,  0,  2,  2,  2,  2,  2,  2,  2,  2,  2
SF |  9,  5,  5,  6,  6,  6,  5,  5,  6,  7,  7,  6,  6,  5
LV | 11, 11, 11, 11, 11, 11, 10, 10, 12, 12, 10, 12, 10, 10
DE | 18, 18, 18, 17, 17, 17, 17, 17, 17, 17, 19, 19, 19, 17

*/

#define WEATHER_INITIAL 255

#define NEXT_WEATHER_POSSIBILITIES (31)

static const char weather_mapping[NEXT_WEATHER_POSSIBILITIES] = {
	/* 9x clear */
	WEATHER_LA_EXTRASUNNY, /* VD  800 */
	WEATHER_LA_EXTRASUNNYSMOG, /* VD  800 */
	WEATHER_SF_EXTRASUNNY, /* VD  455 */
	WEATHER_LV_EXTRASUNNY, /* VD 1000 */
	WEATHER_CS_EXTRASUNNY, /* VD 1500 */
	WEATHER_DE_EXTRASUNNY, /* VD 1500 */

	WEATHER_LA_EXTRASUNNY, /* VD  800 */
	WEATHER_SF_EXTRASUNNY, /* VD  455 */
	WEATHER_LV_EXTRASUNNY, /* VD 1000 */

	/* 9x light clouds */
	WEATHER_LA_SUNNY, /* VD  800 */
	WEATHER_LA_SUNNYSMOG, /* VD  800 */
	WEATHER_SF_SUNNY, /* VD  455 */
	WEATHER_LV_SUNNY, /* VD 1000 */
	WEATHER_CS_SUNNY, /* VD 1500 */
	WEATHER_DE_SUNNY, /* VD 1500 */

	WEATHER_LA_SUNNY, /* VD  800 */
	WEATHER_SF_SUNNY, /* VD  455 */
	WEATHER_LV_SUNNY, /* VD 1000 */

	/* 9x overcast */
	WEATHER_LA_CLOUDY,
	WEATHER_LA_CLOUDY,
	WEATHER_SF_CLOUDY,
	WEATHER_LV_CLOUDY,
	WEATHER_CS_CLOUDY,

	WEATHER_LA_CLOUDY,
	WEATHER_SF_CLOUDY,
	WEATHER_LV_CLOUDY,
	WEATHER_CS_CLOUDY,

	/* 4x thunderstorms */
	WEATHER_SF_RAINY,
	WEATHER_CS_RAINY,

	WEATHER_SF_RAINY,
	WEATHER_CS_RAINY,
};

static const char
	WTH_DESC_CLEAR[] = "clear skies",
	WTH_DESC_LIGHT_CLOUDS[] = "light clouds",
	WTH_DESC_OVERCAST[] = "overcast",
	WTH_DESC_THUNDERSTORMS[] = "thunderstorms",
	WTH_DESC_THICK_FOG[] = "thick fog",
	WTH_DESC_SANDSTORMS[] = "sandstorms";

static const char *weather_descriptions[WEATHERS] = {
	WTH_DESC_CLEAR,
	WTH_DESC_LIGHT_CLOUDS,
	WTH_DESC_CLEAR,
	WTH_DESC_LIGHT_CLOUDS,
	WTH_DESC_OVERCAST,
	WTH_DESC_LIGHT_CLOUDS,
	WTH_DESC_CLEAR,
	WTH_DESC_OVERCAST,
	WTH_DESC_THUNDERSTORMS,
	WTH_DESC_THICK_FOG,
	WTH_DESC_LIGHT_CLOUDS,
	WTH_DESC_CLEAR,
	WTH_DESC_OVERCAST,
	WTH_DESC_CLEAR,
	WTH_DESC_LIGHT_CLOUDS,
	WTH_DESC_OVERCAST,
	WTH_DESC_THUNDERSTORMS,
	WTH_DESC_CLEAR,
	WTH_DESC_LIGHT_CLOUDS,
	WTH_DESC_SANDSTORMS,
};

static const float weather_winds[WEATHERS] = {
	0.0f, 0.25f, 0.0f, 0.2f, 0.7f, 0.25f, 0.0f, 0.7f, 1.0f, 0.0f, 0.2f,
	0.0f, 0.4f, 0.0f, 0.3f, 0.7f, 1.0f, 0.0f, 0.3f, 1.5f
};

#define WIND_MULTIPLIER 34.0f

static const char scales[] = "very high\0moderate\0very low";

#define SCALE_VERYHIGH scales
#define SCALE_HIGH scales + 5
#define SCALE_MODERATE scales + 10
#define SCALE_VERYLOW scales + 19
#define SCALE_LOW scales + 24

static const char *weather_visibilities[WEATHERS] = {
	/*    0 -  454 very low */
	/*  455 -  650 low */
	/*  651 -  700 moderate */
	/*  701 - 1000 high */
	/* 1001 - 1500 very high */
	SCALE_HIGH,
	SCALE_HIGH,
	SCALE_HIGH,
	SCALE_HIGH,
	SCALE_MODERATE,
	SCALE_LOW,
	SCALE_LOW,
	SCALE_VERYHIGH,
	SCALE_LOW,
	SCALE_VERYLOW,
	SCALE_HIGH,
	SCALE_HIGH,
	SCALE_HIGH,
	SCALE_VERYHIGH,
	SCALE_VERYHIGH,
	SCALE_VERYHIGH,
	SCALE_LOW,
	SCALE_VERYHIGH,
	SCALE_VERYHIGH,
	SCALE_VERYLOW,
};

static const char *weather_waves[WEATHERS] = {
	/*   0 - 0.3 low */
	/* 0.4 - 0.6 moderate */
	/* 0.7 - 1.0 high */
	SCALE_VERYLOW,
	SCALE_MODERATE,
	SCALE_VERYLOW,
	SCALE_MODERATE,
	SCALE_HIGH,
	SCALE_MODERATE,
	SCALE_VERYLOW,
	SCALE_HIGH,
	SCALE_HIGH,
	SCALE_VERYLOW,
	SCALE_MODERATE,
	SCALE_VERYLOW,
	SCALE_HIGH,
	SCALE_VERYLOW,
	SCALE_MODERATE,
	SCALE_HIGH,
	SCALE_HIGH,
	SCALE_VERYLOW,
	SCALE_MODERATE,
	SCALE_HIGH,
};

/** In-game hours */
int time_h;
/** In-game minutes */
int time_m;

/**
State of synchronizing weather/time with a player (SYNC_STATE_ constants).
*/
static unsigned char timecycstate[MAX_PLAYERS];
/**
Weather signature of time when player started synchronizing.

If the signature doesn't match signature of current weather when the player
just synced, it means that the weather changed and they should sync again.
This should be rare, but it can happen.
*/
static unsigned int timecycsignature[MAX_PLAYERS];
/**
Last time timecyc_tick has run.

If this is more than 1000 from time_timestamp it means a tick should happen.
*/
static unsigned long lasttime;

#define SYNC_STATE_TIME1 0
#define SYNC_STATE_TIME2 1
#define SYNC_STATE_NONE 10

#define WEATHERSIGNATURE() (*((int*) &weather))

#define WEATHER_TIMER_INTERVAL (13 * 60000)
#define WEATHER_TIMER_DEVIATION (3 * 60000)

/**
Formats the METAR weather message.

@param type should be either 'report' or 'forecast'
*/
static
void timecyc_fmt_metar_msg(char *dest, const char* type, int weather)
{
	sprintf(dest,
		"METAR weather %s: %s, "
		"visibility: %s, winds: %.0fkts, waves: %s",
		type,
		weather_descriptions[weather],
		weather_visibilities[weather],
		weather_winds[weather] * WIND_MULTIPLIER,
		weather_waves[weather]);
}

/**
Sets the weather, it will be interpolated for players.
*/
static
void timecyc_set_weather(int newweather)
{
	int playerid, i;

	if (weather.locked != newweather) {
		weather.locked = newweather;
		NC_PARS(2);
		nc_params[2] = newweather;
		i = playercount;
		while (i--) {
			playerid = players[i];
			if (spawned[playerid] && !isafk[playerid]) {
				nc_params[1] = playerid;
				NC(n_SetPlayerWeather);
			}
		}
	}
}

/**
Advances the weather.

Makes a query to db to update weather stats, except right after initializing.

@param unused unused param just to make the signature correct for timer callback
*/
static
int timecyc_next_weather(void *unused)
{
	int newweather;
	char msg144[144];

	newweather = weather_mapping[NC_random(NEXT_WEATHER_POSSIBILITIES)];
	if (weather.current != WEATHER_INITIAL) {
		csprintf(buf144,
			"INSERT INTO wth(w,l,t) "
			"VALUES(%d,%d,UNIX_TIMESTAMP())",
			newweather,
			weather.current);
		NC_mysql_tquery_nocb(buf144a);
	}
	timecyc_set_weather(newweather);
	missions_on_weather_changed(newweather);
	timecyc_fmt_metar_msg(msg144, "forecast", newweather);
	SendClientMessageToAll(COL_METAR, msg144);
	if (playercount) {
		echo_send_generic_message(ECHO_PACK12_METAR, msg144);
	}

	return WEATHER_TIMER_INTERVAL + NC_random(WEATHER_TIMER_DEVIATION);
}

/**
Starts the time and weather system.
*/
static
void timecyc_init()
{
	time_h = 7;
	time_m = 59;
	weather.current = WEATHER_INITIAL;

	timer_set(timecyc_next_weather(NULL), timecyc_next_weather, NULL);

#ifdef TIMECYC_OVERLAY_CLOCK
	clocktext = TextDrawCreate(608.0, 22.0, "12:73")
	TextDrawColor clocktext, 0xE1E1E1FF
	TextDrawLetterSize clocktext, 0.55, 2.2
	TextDrawSetProportional clocktext, 0
	TextDrawFont clocktext, 3
	TextDrawAlignment clocktext, 3
	TextDrawSetShadow clocktext, 0
	TextDrawSetOutline clocktext, 2
	TextDrawBackgroundColor clocktext, 0x000000FF
#endif
}

/**
Sync weather (and thus time) for a player.

Weather interpolation:
- there are three weather variables: current, upcoming, locked
- there is equilibrium when all three are the same
- when changing, lockedweather gets set
- nothing happens until the clock strikes the hour
- when the clock strikes the hour, upcoming is set to locked
- the weather will transition this whole hour
- when the clock strikes again, current is set to upcoming, forming equilibrium

- changing the clock forward during a transition only jumps the transition
- next phase triggers when the current minute is lower than last minute
  - so when the hour changes, minutes goes from 60 to 0, hence cycle
  - this means changing minutes backwards will force cycle, but not with hours

- when player dies, a cycle goes (TODO check)

- this works to change the weather instantly:
  - TogglePlayerClock 0
  - SetPlayerWeather 16
  - TogglePlayerClock 1
  - SetPlayerWeather 0
  - now current and upcoming are 16, locked is 0
*/
static
void timecyc_sync(int playerid)
{
	/*Set current weather to all.
	Setting the weather without having a clock changes all three of
	current, upcoming, locked.*/
	NC_TogglePlayerClock(playerid, 0);
	NC_SetPlayerWeather(playerid, weather.current);
	/*No delay needed here^.*/

	if (weather.current == weather.upcoming) {
		/*Not in a transition, can just set the time right.*/
		NC_SetPlayerTime(playerid, time_h, time_m);
		NC_TogglePlayerClock(playerid, 1);
		if (weather.upcoming != weather.locked) {
			/*Transition is in queue, let player know.*/
			NC_SetPlayerWeather(playerid, weather.locked);
			/*No delay needed here^.*/
		}
		timecycstate[playerid] = SYNC_STATE_NONE;
	} else {
		/*Need to change upcoming, so force a transition cycle.
		Setting minutes to 30, to later set back to 0
		(in timecyc_on_player_update), which will force a cycle.*/
		NC_SetPlayerTime(playerid, time_h, 30);
		NC_TogglePlayerClock(playerid, 1);
		NC_SetPlayerWeather(playerid, weather.upcoming);
		/*This sets lockedweather^.*/
		timecycstate[playerid] = SYNC_STATE_TIME1;
		timecycsignature[playerid] = WEATHERSIGNATURE();
		/*Rest is done in timecyc_on_player_update, since the current
		changes need to be processed before continuing.*/
	}
}

static
void timecyc_on_player_connect(int playerid)
{
	timecycstate[playerid] = SYNC_STATE_NONE;
}

static
void timecyc_on_player_death(int playerid)
{
	NC_TogglePlayerClock(playerid, 0);
}

static
void timecyc_on_player_request_class(int playerid)
{
	NC_TogglePlayerClock(playerid, 0);
	NC_SetPlayerTime(playerid, 12, 0);
	NC_SetPlayerWeather(playerid, 1);
}

static
void timecyc_on_player_update(int playerid)
{
	switch (timecycstate[playerid]) {
	case SYNC_STATE_TIME1:
		NC_SetPlayerTime(playerid, time_h, 0);
		timecycstate[playerid] = SYNC_STATE_TIME2;
		break;
	case SYNC_STATE_TIME2:
		if (timecycsignature[playerid] == WEATHERSIGNATURE()) {
			NC_SetPlayerTime(playerid, time_h, time_m);
			if (weather.locked != weather.upcoming) {
				NC_SetPlayerWeather(playerid, weather.locked);
			}
			timecycstate[playerid] = SYNC_STATE_NONE;
		} else {
			/*TODO: remove this after a while maybe*/
			logprintf("[timecyc] it happened");
			timecyc_sync(playerid);
		}
		break;
	}
}

static
void timecyc_on_player_was_afk(int playerid)
{
	timecyc_sync(playerid);
}

/**
Syncs timecyc's tick function.
*/
static
void timecyc_reset()
{
	lasttime = time_timestamp();
}

/**
Syncs everyone's clock if there's no transition going on.

Note: SA-MP syncs time for people with SetPlayerTime set every 5 minutes, so by
doing it here that's being prevented.
*/
static
void timecyc_sync_clocks()
{
	int i, playerid;

	if (weather.current == weather.upcoming &&
		weather.current == weather.locked)
	{
		NC_PARS(3);
		nc_params[2] = time_h;
		nc_params[3] = time_m;
		i = playercount;
		while (i--) {
			playerid = players[i];
			if (spawned[playerid] && !isafk[playerid] &&
				timecycstate[playerid] == SYNC_STATE_NONE)
			{
				nc_params[1] = playerid;
				NC(n_SetPlayerTime);
			}
		}
	}
}

/**
Let the clock tick. This will also call most timed functions. TODO: move those timed function calls somewhere else
*/
static
void timecyc_tick()
{
	unsigned long nowtime;
	int idx, playerid;
	struct vec3 pos;
	char rcon_cmd[100];

	nowtime = time_timestamp();
	if (nowtime - lasttime > 1000) {
		lasttime += 1000;
		if (time_m++ == 30) {
			goto timer30s;
		} else if (time_m >= 60) {
			time_m = 0;
			weather.current = weather.upcoming;
			weather.upcoming = weather.locked;
			if (++time_h >= 24) {
				time_h = 0;
			}

			if (time_h == PANEL_NIGHT_COLORS_FROM_HR || time_h == PANEL_NIGHT_COLORS_TO_HR) {
				panel_day_night_changed();
			}
			/*timer1m*/
timer30s:
			/*timer30s*/
			heartbeat_timed_update();
		}
		sprintf(rcon_cmd, "worldtime %02d:%02d %s", time_h, time_m, weather_descriptions[weather.current]);
		SendRconCommand(rcon_cmd);
		/*timer1000*/
		veh_timed_1s_update();

		idx = playercount;
		while (idx--) {
			playerid = players[idx];
			if (!isafk[playerid]) {
				GetPlayerPos(playerid, &pos);
				/*GPS_SHOULD_SHOW could be checked here,
				but most players will probably have it enabled so that branch is left out here.*/
				zones_update(playerid, pos);
			}
		}

		if ((time_m % 5) == 0) {
			/*timer5000*/
			anticheat_decrease_infractions();
			veh_commit_next_vehicle_odo_to_db();
			timecyc_sync_clocks();
		}
	}
}

static
int timecyc_get_current_render_distance()
{
	/*Could interpolate between current and upcoming, but I suspect it's not linear, and it's not always synced.*/
	return weather_view_distances[weather.upcoming];
}
