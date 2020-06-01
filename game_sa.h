
/* vim: set filetype=c ts=8 noexpandtab: */

#define PLANE 0x1
#define HELI 0x2

#define WEAPON_BRASSKNUCKLE 1
#define WEAPON_GOLFCLUB 2
#define WEAPON_NITESTICK 3
#define WEAPON_KNIFE 4
#define WEAPON_BAT 5
#define WEAPON_SHOVEL 6
#define WEAPON_POOLSTICK 7
#define WEAPON_KATANA 8
#define WEAPON_CHAINSAW 9
#define WEAPON_DILDO 10
#define WEAPON_DILDO2 11
#define WEAPON_VIBRATOR 12
#define WEAPON_VIBRATOR2 13
#define WEAPON_FLOWER 14
#define WEAPON_CANE 15
#define WEAPON_GRENADE 16
#define WEAPON_TEARGAS 17
#define WEAPON_MOLTOV 18
#define WEAPON_COLT45 22
#define WEAPON_SILENCED 23
#define WEAPON_DEAGLE 24
#define WEAPON_SHOTGUN 25
#define WEAPON_SAWEDOFF 26
#define WEAPON_SHOTGSPA 27
#define WEAPON_UZI 28
#define WEAPON_MP5 29
#define WEAPON_AK47 30
#define WEAPON_M4 31
#define WEAPON_TEC9 32
#define WEAPON_RIFLE 33
#define WEAPON_SNIPER 34
#define WEAPON_ROCKETLAUNCHER 35
#define WEAPON_HEATSEEKER 36
#define WEAPON_FLAMETHROWER 37
#define WEAPON_MINIGUN 38
#define WEAPON_SATCHEL 39
#define WEAPON_BOMB 40
#define WEAPON_SPRAYCAN 41
#define WEAPON_FIREEXTINGUISHER 42
#define WEAPON_CAMERA 43
#define WEAPON_NVIS 44
#define WEAPON_IRVIS 45
#define WEAPON_PARACHUTE 46
/*?*/
#define WEAPON_FAKEPISTOL 47
/*Only usable in SendDeathMessage*/
#define WEAPON_VEHICLE 49
/*Only usable in SendDeathMessage*/
#define WEAPON_HELIBLADES 50
/*Only usable in SendDeathMessage*/
#define WEAPON_EXPLOSION 51
/*Only usable in SendDeathMessage*/
#define WEAPON_DROWN 53
/*Only usable in SendDeathMessage*/
#define WEAPON_COLLISION 54
/*Only usable in SendDeathMessage*/
#define WEAPON_CONNECT 200
/*Only usable in SendDeathMessage*/
#define WEAPON_DISCONNECT 201
/*Only usable in SendDeathMessage*/
#define WEAPON_SUICIDE 255

#define WEATHER_LA_EXTRASUNNY 0
#define WEATHER_LA_SUNNY 1
#define WEATHER_LA_EXTRASUNNYSMOG 2
#define WEATHER_LA_SUNNYSMOG 3
#define WEATHER_LA_CLOUDY 4
#define WEATHER_SF_SUNNY 5
#define WEATHER_SF_EXTRASUNNY 6
#define WEATHER_SF_CLOUDY 7
#define WEATHER_SF_RAINY 8
#define WEATHER_SF_FOGGY 9
#define WEATHER_LV_SUNNY 10
#define WEATHER_LV_EXTRASUNNY 11
#define WEATHER_LV_CLOUDY 12
#define WEATHER_CS_EXTRASUNNY 13
#define WEATHER_CS_SUNNY 14
#define WEATHER_CS_CLOUDY 15
#define WEATHER_CS_RAINY 16
#define WEATHER_DE_EXTRASUNNY 17
#define WEATHER_DE_SUNNY 18
#define WEATHER_DE_SANDSTORMS 19
#define WEATHER_UNDERWATER 20
#define WEATHERS 21

#define MODEL_LANDSTAL (400)
#define MODEL_BRAVURA (401)
#define MODEL_BUFFALO (402)
#define MODEL_LINERUN (403)
#define MODEL_PEREN (404)
#define MODEL_SENTINEL (405)
#define MODEL_DUMPER (406)
#define MODEL_FIRETRUK (407)
#define MODEL_TRASH (408)
#define MODEL_STRETCH (409)
#define MODEL_MANANA (410)
#define MODEL_INFERNUS (411)
#define MODEL_VOODOO (412)
#define MODEL_PONY (413)
#define MODEL_MULE (414)
#define MODEL_CHEETAH (415)
#define MODEL_AMBULAN (416)
#define MODEL_LEVIATHN (417)
#define MODEL_MOONBEAM (418)
#define MODEL_ESPERANT (419)
#define MODEL_TAXI (420)
#define MODEL_WASHING (421)
#define MODEL_BOBCAT (422)
#define MODEL_MRWHOOP (423)
#define MODEL_BFINJECT (424)
#define MODEL_HUNTER (425)
#define MODEL_PREMIER (426)
#define MODEL_ENFORCER (427)
#define MODEL_SECURICA (428)
#define MODEL_BANSHEE (429)
#define MODEL_PREDATOR (430)
#define MODEL_BUS (431)
#define MODEL_RHINO (432)
#define MODEL_BARRACKS (433)
#define MODEL_HOTKNIFE (434)
#define MODEL_ARTICT1 (435)
#define MODEL_PREVION (436)
#define MODEL_COACH (437)
#define MODEL_CABBIE (438)
#define MODEL_STALLION (439)
#define MODEL_RUMPO (440)
#define MODEL_RCBANDIT (441)
#define MODEL_ROMERO (442)
#define MODEL_PACKER (443)
#define MODEL_MONSTER (444)
#define MODEL_ADMIRAL (445)
#define MODEL_SQUALO (446)
#define MODEL_SEASPAR (447)
#define MODEL_PIZZABOY (448)
#define MODEL_TRAM (449)
#define MODEL_ARTICT2 (450)
#define MODEL_TURISMO (451)
#define MODEL_SPEEDER (452)
#define MODEL_REEFER (453)
#define MODEL_TROPIC (454)
#define MODEL_FLATBED (455)
#define MODEL_YANKEE (456)
#define MODEL_CADDY (457)
#define MODEL_SOLAIR (458)
#define MODEL_TOPFUN (459)
#define MODEL_SKIMMER (460)
#define MODEL_PCJ600 (461)
#define MODEL_FAGGIO (462)
#define MODEL_FREEWAY (463)
#define MODEL_RCBARON (464)
#define MODEL_RCRAIDER (465)
#define MODEL_GLENDALE (466)
#define MODEL_OCEANIC (467)
#define MODEL_SANCHEZ (468)
#define MODEL_SPARROW (469)
#define MODEL_PATRIOT (470)
#define MODEL_QUAD (471)
#define MODEL_COASTG (472)
#define MODEL_DINGHY (473)
#define MODEL_HERMES (474)
#define MODEL_SABRE (475)
#define MODEL_RUSTLER (476)
#define MODEL_ZR350 (477)
#define MODEL_WALTON (478)
#define MODEL_REGINA (479)
#define MODEL_COMET (480)
#define MODEL_BMX (481)
#define MODEL_BURRITO (482)
#define MODEL_CAMPER (483)
#define MODEL_MARQUIS (484)
#define MODEL_BAGGAGE (485)
#define MODEL_DOZER (486)
#define MODEL_MAVERICK (487)
#define MODEL_VCNMAV (488)
#define MODEL_RANCHER (489)
#define MODEL_FBIRANCH (490)
#define MODEL_VIRGO (491)
#define MODEL_GREENWOO (492)
#define MODEL_JETMAX (493)
#define MODEL_HOTRING (494)
#define MODEL_SANDKING (495)
#define MODEL_BLISTAC (496)
#define MODEL_POLMAV (497)
#define MODEL_BOXVILLE (498)
#define MODEL_BENSON (499)
#define MODEL_MESA (500)
#define MODEL_RCGOBLIN (501)
#define MODEL_HOTRINA (502)
#define MODEL_HOTRINB (503)
#define MODEL_BLOODRA (504)
#define MODEL_RNCHLURE (505)
#define MODEL_SUPERGT (506)
#define MODEL_ELEGANT (507)
#define MODEL_JOURNEY (508)
#define MODEL_BIKE (509)
#define MODEL_MTBIKE (510)
#define MODEL_BEAGLE (511)
#define MODEL_CROPDUST (512)
#define MODEL_STUNT (513)
#define MODEL_PETRO (514)
#define MODEL_RDTRAIN (515)
#define MODEL_NEBULA (516)
#define MODEL_MAJESTIC (517)
#define MODEL_BUCCANEE (518)
#define MODEL_SHAMAL (519)
#define MODEL_HYDRA (520)
#define MODEL_FCR900 (521)
#define MODEL_NRG500 (522)
#define MODEL_COPBIKE (523)
#define MODEL_CEMENT (524)
#define MODEL_TOWTRUCK (525)
#define MODEL_FORTUNE (526)
#define MODEL_CADRONA (527)
#define MODEL_FBITRUCK (528)
#define MODEL_WILLARD (529)
#define MODEL_FORKLIFT (530)
#define MODEL_TRACTOR (531)
#define MODEL_COMBINE (532)
#define MODEL_FELTZER (533)
#define MODEL_REMINGTN (534)
#define MODEL_SLAMVAN (535)
#define MODEL_BLADE (536)
#define MODEL_FREIGHT (537)
#define MODEL_STREAK (538)
#define MODEL_VORTEX (539)
#define MODEL_VINCENT (540)
#define MODEL_BULLET (541)
#define MODEL_CLOVER (542)
#define MODEL_SADLER (543)
#define MODEL_FIRELA (544)
#define MODEL_HUSTLER (545)
#define MODEL_INTRUDER (546)
#define MODEL_PRIMO (547)
#define MODEL_CARGOBOB (548)
#define MODEL_TAMPA (549)
#define MODEL_SUNRISE (550)
#define MODEL_MERIT (551)
#define MODEL_UTILITY (552)
#define MODEL_NEVADA (553)
#define MODEL_YOSEMITE (554)
#define MODEL_WINDSOR (555)
#define MODEL_MONSTERA (556)
#define MODEL_MONSTERB (557)
#define MODEL_URANUS (558)
#define MODEL_JESTER (559)
#define MODEL_SULTAN (560)
#define MODEL_STRATUM (561)
#define MODEL_ELEGY (562)
#define MODEL_RAINDANC (563)
#define MODEL_RCTIGER (564)
#define MODEL_FLASH (565)
#define MODEL_TAHOMA (566)
#define MODEL_SAVANNA (567)
#define MODEL_BANDITO (568)
#define MODEL_FREIFLAT (569)
#define MODEL_STREAKC (570)
#define MODEL_KART (571)
#define MODEL_MOWER (572)
#define MODEL_DUNERIDE (573)
#define MODEL_SWEEPER (574)
#define MODEL_BROADWAY (575)
#define MODEL_TORNADO (576)
#define MODEL_AT400 (577)
#define MODEL_DFT30 (578)
#define MODEL_HUNTLEY (579)
#define MODEL_STAFFORD (580)
#define MODEL_BF400 (581)
#define MODEL_NEWSVAN (582)
#define MODEL_TUG (583)
#define MODEL_PETROTR (584)
#define MODEL_EMPEROR (585)
#define MODEL_WAYFARER (586)
#define MODEL_EUROS (587)
#define MODEL_HOTDOG (588)
#define MODEL_CLUB (589)
#define MODEL_FREIBOX (590)
#define MODEL_ARTICT3 (591)
#define MODEL_ANDROM (592)
#define MODEL_DODO (593)
#define MODEL_RCCAM (594)
#define MODEL_LAUNCH (595)
#define MODEL_COPCARLA (596)
#define MODEL_COPCARSF (597)
#define MODEL_COPCARVG (598)
#define MODEL_COPCARRU (599)
#define MODEL_PICADOR (600)
#define MODEL_SWATVAN (601)
#define MODEL_ALPHA (602)
#define MODEL_PHOENIX (603)
#define MODEL_GLENSHIT (604)
#define MODEL_SADLSHIT (605)
#define MODEL_BAGBOXA (606)
#define MODEL_BAGBOXB (607)
#define MODEL_TUGSTAIR (608)
#define MODEL_BOXBURG (609)
#define MODEL_FARMTR1 (610)
#define MODEL_UTILTR1 (611)
#define VEHICLE_MODEL_TOTAL (611-400+1)

#define NUM_AIRCRAFT_MODELS 23

struct CARCOLDATA {
	char amount;
	short position;
};

/**
Some mapping, every air vehicle has a unique value 0-22, -1 for others
*/
extern char aircraftmodelindex[VEHICLE_MODEL_TOTAL];
extern char *vehnames[];
extern char *vehmodelnames[];
extern unsigned int vehicleflags[VEHICLE_MODEL_TOTAL];
extern struct CARCOLDATA carcoldata[VEHICLE_MODEL_TOTAL];
extern char carcols[1912];

/**
Check if the given vehicle model is an air vehicle (plane or helicopter).
*/
int game_is_air_vehicle(int model);
/**
Check if the given vehicle model is a helicopter.
*/
int game_is_heli(int model);
/**
Check if the given vehicle model is a plane.
*/
int game_is_plane(int model);
/**
Gives a random 'normal' random vehicle colors combination for given model.

Data is from game's carcols.dat.

@param col1 ptr to int or NULL if primary color is not needed
@param col2 ptr to int or NULL if secondary color is not needed
*/
void game_random_carcol(int model, int *col1, int *col2);
void game_sa_init();

