
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"

/*

viedistance can be /2 by night

VDIS viewdistance
CC   cloud coverage
FOGY foggyness
FOGD fogdensity
RAIN rain
SAND sandstorm
WIND wind
HH   heathaze
WAVE waveyness

+-----+-----------------------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
| id  | name                  | VDIS  | CC    | FOGY  | FOGD  | RAIN  | SAND  | WIND  | HH    | WAVE  |
+-----+-----------------------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
|  0  | LA Extra Sunny        | 800   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 1.0   | 0.3   |
|  1  | LA Sunny              | 800   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.25  | 1.0   | 0.55  |
|  2  | LA Extra Sunny Smog   | 800   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 1.0   | 0.3   |
|  3  | LA Sunny Smog         | 800   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.2   | 1.0   | 0.5   |
|  4  | LA Cloudy             | 700   | 1.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.7   | 0.0   | 1.0   |
|                                                                                                     |
|  5  | SF Sunny              | 455   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.25  | 0.0   | 0.55  |
|  6  | SF Extra Sunny        | 455   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.3   |
|  7  | SF Cloudy             | 1150  | 1.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.7   | 0.0   | 1.0   |
|  8  | SF Rainy              | 650   | 1.0   | 0.0   | 0.18  | 1.0   | 0.0   | 1.0   | 0.0   | 1.0   |
|  9  | SF Foggy              | 250   | 1.0   | 1.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.3   |
|                                                                                                     |
| 10  | LV Sunny              | 1000  | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.2   | 0.0   | 0.5   |
| 11  | LV Extra Sunny        | 1000  | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 1.0   | 0.3   |
| 12  | LV Cloudy             | 1000  | 1.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.4   | 0.0   | 0.7   |
|                                                                                                     |
| 13  | CS Extra Sunny        | 1500  | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.3   |
| 14  | CS Sunny              | 1500  | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.3   | 0.0   | 0.6   |
| 15  | CS Cloudy             | 1150  | 1.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.7   | 0.0   | 1.0   |
| 16  | CS Rainy              | 650   | 1.0   | 0.0   | 0.2   | 1.0   | 0.0   | 1.0   | 0.0   | 1.0   |
|                                                                                                     |
| 17  | DE Extra Sunny        | 1500  | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 1.0   | 0.3   |
| 18  | DE Sunny              | 1500  | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.3   | 1.0   | 0.6   |
| 19  | DE Sandstorm          | 150   | 1.0   | 1.0   | 0.0   | 0.0   | 1.0   | 1.5   | 0.0   | 1.0   |
|                                                                                                     |
| 20  | Underwater            | 300   | 1.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.0   | 0.3   |
+-----+-----------------------+-------+-------+-------+-------+-------+-------+-------+-------+-------+

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
20: thick fog

default:
6 28.5% clear
6 28.5% light clouds
4 19.0% overcast
2 9.5% thunderstorms
2 9.5% thick fog
1 4.7% sandstorms

wind * 34kts

 // from sa-mp forums: https://forum.sa-mp.com/showthread.php?t=285077
 0: METAR INFO: [Current Weather]: Sunny Skies |[Visibility]:High |[Winds]:NA |[Clouds]:Moderate
 1: METAR INFO: [Current Weather]: Moderate Sunny Skies [Visibility]:High [Winds]:NA [Clouds]:Moderate
 2: METAR INFO: [Current Weather]: Sunny Skies [Visibility]:High [Winds]:NA [Clouds]:Moderate
 3: METAR INFO: [Current Weather]: Sunny Skies [Visibility]:High [Winds]:NA [Clouds]:Moderate
 4: METAR INFO: [Current Weather]: Sunny Skies |[Visibility]:High |[Winds]:NA |[Clouds]:Moderate
 5: METAR INFO: [Current Weather]: Sunny Skies |[Visibility]:High |[Winds]:NA |[Clouds]:Moderate
 6:
 7: METAR INFO: [Current Weather]: Sunny Skies |[Visibility]:High |[Winds]:NA |[Clouds]:Moderate
 8: METAR INFO: [Current Weather]: Wet Rainy Weather |[Visibility]:Medium |[Winds]:Moderate |[Clouds]:Heavy
 9: METAR INFO: [Current Weather]: Thick Fog |[Visibility]:Low |[Winds]:Moderate |[Clouds]:Heavy
10: METAR INFO: [Current Weather]: Moderate Sunny Skies |[Visibility]:High |[Winds]:NA |[Clouds]:Moderate
11: METAR INFO: [Current Weather]: HeatWave |[Visibility]:High |[Winds]:NA |[Clouds]:Moderate
12: METAR INFO: [Current Weather]: Hazy/Dull Weather |[Visibility]:Moderate |[Winds]:NA |[Clouds]:High
13: METAR INFO: [Current Weather]: Hazy/Dull Weather |[Visibility]:Moderate |[Winds]:NA |[Clouds]:High
14: METAR INFO: [Current Weather]: Hazy/Dull Weather |[Visibility]:Moderate |[Winds]:NA |[Clouds]:High
15: METAR INFO: [Current Weather]: Heavy RainStorm |[Visibility]:Low |[Winds]:Very High |[Clouds]: very Thick
16: METAR INFO: [Current Weather]: Scorching Hot Bright Weather |[Visibility]:High |[Winds]:NA |[Clouds]:Low
17: METAR INFO: [Current Weather]: Scorching Hot Bright Weather |[Visibility]:High |[Winds]:NA |[Clouds]:Low
18: METAR INFO: [Current Weather]: Scorching Hot Bright Weather |[Visibility]:High |[Winds]:NA |[Clouds]:Low
19: METAR INFO: [Current Weather]: Sand Storm |[Visibility]:Very-Low |[Winds]: High Speed Winds |[Clouds]:Heavy
20: METAR INFO: [Current Weather]: Toxic Green Smog |[Visibility]:Low |[Winds]:Moderate |[Clouds]:Heavy

weather table 0x8D5EB0

CS | 13, 13, 13, 13, 13, 13, 13, 13, 14, 15,  9,  9, 15, 14, 13, 14, 14, 13, 13, 13, 13, 14, 14, 13, 13
LA |  2,  2,  0,  0,  0,  2,  2,  2,  3,  0,  3,  2,  0,  0,  3,  3,  3,  4,  4,  0,  0,  1,  1,  0,  2
SF |  5,  5,  5,  5,  5,  7,  7,  9,  9,  7,  5,  5,  5,  5,  5,  7,  7,  7,  8,  8,  7,  7,  5,  5,  5
LV | 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 10, 10, 10, 10, 11
DE | 17, 17, 17, 19, 19, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 17

CS | 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 15,  9,  9, 15, 16, 16, 16, 16, 15, 14
LA |  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  2,  2,  3,  3,  0,  0, 16,  0,  4,  1
SF |  5,  6,  6,  5,  7,  9,  9,  7,  5,  5,  7,  5,  5,  6,  6,  5,  5,  5,  7,  7,  8,  8,  7,  7,  9
LV | 11, 11, 11, 11, 11, 11, 11, 11, 10, 11, 11, 11, 11, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11
DE | 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 19, 19, 19, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17

CS | 14, 14, 14, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13
LA |  1,  1,  1,  0,  0,  2,  2,  2,  2,  2,  2,  2,  2,  2
SF |  9,  5,  5,  6,  6,  6,  5,  5,  6,  7,  7,  6,  6,  5
LV | 11, 11, 11, 11, 11, 11, 10, 10, 12, 12, 10, 12, 10, 10
DE | 18, 18, 18, 17, 17, 17, 17, 17, 17, 17, 19, 19, 19, 17

*/

