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
#define WEATHERS 21

const int weather_view_distances[WEATHERS] = {
	800, 800, 800, 700,
	455, 455, 1150, 650, 250,
	1000, 1000, 1000,
	1500, 1500, 1150, 650,
	1500, 1500, 150,
};

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
#define VEHICLE_MODEL_MIN (400)
#define VEHICLE_MODEL_MAX (611)
#define VEHICLE_MODEL_TOTAL (VEHICLE_MODEL_MAX - VEHICLE_MODEL_MIN + 1)

#define NUM_AIRCRAFT_MODELS 23

static char *vehnames[] = {
	"Landstalker",
	"Bravura",
	"Buffalo",
	"Linerunner",
	"Perennial",
	"Sentinel",
	"Dumper",
	"Fire Truck",
	"Trashmaster",
	"Stretch",
	"Manana",
	"Infernus",
	"Voodoo",
	"Pony",
	"Mule",
	"Cheetah",
	"Ambulance",
	"Leviathan",
	"Moonbeam",
	"Esperanto",
	"Taxi",
	"Washington",
	"Bobcat",
	"Mr Whoopee",
	"BF Injection",
	"Hunter",
	"Premier",
	"Enforcer",
	"Securicar",
	"Banshee",
	"Predator",
	"Bus",
	"Rhino",
	"Barracks",
	"Hotknife",
	"Artic Trailer 1",
	"Previon",
	"Coach",
	"Cabbie",
	"Stallion",
	"Rumpo",
	"RC Bandit",
	"Romero",
	"Packer",
	"Monster",
	"Admiral",
	"Squalo",
	"Seasparrow",
	"Pizzaboy",
	"Tram",
	"Artic Trailer 2",
	"Turismo",
	"Speeder",
	"Reefer",
	"Tropic",
	"Flatbed",
	"Yankee",
	"Caddy",
	"Solair",
	"Berkley's RC Van",
	"Skimmer",
	"PCJ-600",
	"Faggio",
	"Freeway",
	"RC Baron",
	"RC Raider",
	"Glendale",
	"Oceanic",
	"Sanchez",
	"Sparrow",
	"Patriot",
	"Quadbike",
	"Coast Guard",
	"Dinghy",
	"Hermes",
	"Sabre",
	"Rustler",
	"ZR-350",
	"Walton",
	"Regina",
	"Comet",
	"BMX",
	"Burrito",
	"Camper",
	"Marquis",
	"Baggage",
	"Dozer",
	"Maverick",
	"News Chopper",
	"Rancher",
	"FBI Rancher",
	"Virgo",
	"Greenwood",
	"Jetmax",
	"Hotring Racer",
	"Sandking",
	"Blista Compact",
	"Police Maverick",
	"Boxville",
	"Benson",
	"Mesa",
	"RC Goblin",
	"Hotring Racer A",
	"Hotring Racer B",
	"Bloodring Banger",
	"Lure Rancher",
	"Super GT",
	"Elegant",
	"Journey",
	"Bike",
	"Mountain Bike",
	"Beagle",
	"Cropduster",
	"Stuntplane",
	"Tanker",
	"Roadtrain",
	"Nebula",
	"Majestic",
	"Buccaneer",
	"Shamal",
	"Hydra",
	"FCR-900",
	"NRG-500",
	"HPV1000",
	"Cement Truck",
	"Towtruck",
	"Fortune",
	"Cadrona",
	"FBI Truck",
	"Willard",
	"Forklift",
	"Tractor",
	"Combine Harvester",
	"Feltzer",
	"Remington",
	"Slamvan",
	"Blade",
	"Freight",
	"Brown Streak",
	"Vortex",
	"Vincent",
	"Bullet",
	"Clover",
	"Sadler",
	"Fire Truck Ladder",
	"Hustler",
	"Intruder",
	"Primo",
	"Cargobob",
	"Tampa",
	"Sunrise",
	"Merit",
	"Utility Van",
	"Nevada",
	"Yosemite",
	"Windsor",
	"Monster A",
	"Monster B",
	"Uranus",
	"Jester",
	"Sultan",
	"Stratum",
	"Elegy",
	"Raindance",
	"RC Tiger",
	"Flash",
	"Tahoma",
	"Savanna",
	"Bandito",
	"Freight flat",
	"Brown Streak Carriage",
	"Kart",
	"Mower",
	"Dune",
	"Sweeper",
	"Broadway",
	"Tornado",
	"AT-400",
	"DFT-30",
	"Huntley",
	"Stafford",
	"BF-400",
	"Newsvan",
	"Tug",
	"Tanker Trailer",
	"Emperor",
	"Wayfarer",
	"Euros",
	"Hotdog",
	"Club",
	"Freight box",
	"Artic Trailer 3",
	"Andromada",
	"Dodo",
	"RC Cam",
	"Launch",
	"Police LS",
	"Police SF",
	"Police LV",
	"Ranger",
	"Picador",
	"S.W.A.T.",
	"Alpha",
	"Phoenix",
	"Damaged Glendale",
	"Damaged Sadler",
	"Baggage Box A",
	"Baggage Box B",
	"Tug Stairs",
	"Black Boxville",
	"Farm Trailer",
	"Utility Trailer",
};

char *vehmodelnames[] = {
	"landstal",
	"bravura",
	"buffalo",
	"linerun",
	"peren",
	"sentinel",
	"dumper",
	"firetruk",
	"trash",
	"stretch",
	"manana",
	"infernus",
	"voodoo",
	"pony",
	"mule",
	"cheetah",
	"ambulan",
	"leviathn",
	"moonbeam",
	"esperant",
	"taxi",
	"washing",
	"bobcat",
	"mrwhoop",
	"bfinject",
	"hunter",
	"premier",
	"enforcer",
	"securica",
	"banshee",
	"predator",
	"bus",
	"rhino",
	"barracks",
	"hotknife",
	"artict1",
	"previon",
	"coach",
	"cabbie",
	"stallion",
	"rumpo",
	"rcbandit",
	"romero",
	"packer",
	"monster",
	"admiral",
	"squalo",
	"seaspar",
	"pizzaboy",
	"tram",
	"artict2",
	"turismo",
	"speeder",
	"reefer",
	"tropic",
	"flatbed",
	"yankee",
	"caddy",
	"solair",
	"topfun",
	"skimmer",
	"pcj600",
	"faggio",
	"freeway",
	"rcbaron",
	"rcraider",
	"glendale",
	"oceanic",
	"sanchez",
	"sparrow",
	"patriot",
	"quad",
	"coastg",
	"dinghy",
	"hermes",
	"sabre",
	"rustler",
	"zr350",
	"walton",
	"regina",
	"comet",
	"bmx",
	"burrito",
	"camper",
	"marquis",
	"baggage",
	"dozer",
	"maverick",
	"vcnmav",
	"rancher",
	"fbiranch",
	"virgo",
	"greenwoo",
	"jetmax",
	"hotring",
	"sandking",
	"blistac",
	"polmav",
	"boxville",
	"benson",
	"mesa",
	"rcgoblin",
	"hotrina",
	"hotrinb",
	"bloodra",
	"rnchlure",
	"supergt",
	"elegant",
	"journey",
	"bike",
	"mtbike",
	"beagle",
	"cropdust",
	"stunt",
	"petro",
	"rdtrain",
	"nebula",
	"majestic",
	"buccanee",
	"shamal",
	"hydra",
	"fcr900",
	"nrg500",
	"copbike",
	"cement",
	"towtruck",
	"fortune",
	"cadrona",
	"fbitruck",
	"willard",
	"forklift",
	"tractor",
	"combine",
	"feltzer",
	"remingtn",
	"slamvan",
	"blade",
	"freight",
	"streak",
	"vortex",
	"vincent",
	"bullet",
	"clover",
	"sadler",
	"firela",
	"hustler",
	"intruder",
	"primo",
	"cargobob",
	"tampa",
	"sunrise",
	"merit",
	"utility",
	"nevada",
	"yosemite",
	"windsor",
	"monstera",
	"monsterb",
	"uranus",
	"jester",
	"sultan",
	"stratum",
	"elegy",
	"raindanc",
	"rctiger",
	"flash",
	"tahoma",
	"savanna",
	"bandito",
	"freiflat",
	"streakc",
	"kart",
	"mower",
	"duneride",
	"sweeper",
	"broadway",
	"tornado",
	"at400",
	"dft30",
	"huntley",
	"stafford",
	"bf400",
	"newsvan",
	"tug",
	"petrotr",
	"emperor",
	"wayfarer",
	"euros",
	"hotdog",
	"club",
	"freibox",
	"artict3",
	"androm",
	"dodo",
	"rccam",
	"launch",
	"copcarla",
	"copcarsf",
	"copcarvg",
	"copcarru",
	"picador",
	"swatvan",
	"alpha",
	"phoenix",
	"glenshit",
	"sadlshit",
	"bagboxa",
	"bagboxb",
	"tugstair",
	"boxburg",
	"farmtr1",
	"utiltr1",
};

struct CARCOLDATA {
	char amount;
	short position;
};

/*see carcols.dat and carcolsgen in basdon/basdon-tools*/
static struct CARCOLDATA carcoldata[VEHICLE_MODEL_TOTAL] = {
	 { 8, 0 },
	 { 7, 16 },
	 { 8, 30 },
	 { 8, 46 },
	 { 8, 62 },
	 { 8, 78 },
	 { 1, 94 },
	 { 1, 96 },
	 { 1, 98 },
	 { 1, 94 },
	 { 8, 100 },
	 { 8, 116 },
	 { 8, 132 },
	 { 8, 148 },
	 { 8, 164 },
	 { 8, 180 },
	 { 1, 196 },
	 { 1, 94 },
	 { 8, 198 },
	 { 8, 214 },
	 { 1, 230 },
	 { 8, 232 },
	 { 7, 248 },
	 { 6, 262 },
	 { 8, 274 },
	 { 1, 290 },
	 { 8, 292 },
	 { 1, 308 },
	 { 1, 310 },
	 { 8, 312 },
	 { 1, 328 },
	 { 8, 330 },
	 { 1, 290 },
	 { 1, 290 },
	 { 7, 346 },
	 { 1, 94 },
	 { 7, 360 },
	 { 8, 374 },
	 { 1, 390 },
	 { 8, 392 },
	 { 8, 408 },
	 { 8, 424 },
	 { 8, 440 },
	 { 8, 456 },
	 { 6, 472 },
	 { 8, 484 },
	 { 8, 500 },
	 { 1, 516 },
	 { 1, 94 },
	 { 1, 518 },
	 { 1, 94 },
	 { 8, 520 },
	 { 8, 536 },
	 { 1, 552 },
	 { 1, 98 },
	 { 8, 554 },
	 { 8, 570 },
	 { 8, 586 },
	 { 8, 602 },
	 { 8, 618 },
	 { 8, 634 },
	 { 8, 650 },
	 { 8, 312 },
	 { 8, 666 },
	 { 1, 682 },
	 { 1, 682 },
	 { 8, 684 },
	 { 8, 700 },
	 { 4, 716 },
	 { 1, 196 },
	 { 1, 290 },
	 { 8, 724 },
	 { 2, 740 },
	 { 2, 740 },
	 { 8, 744 },
	 { 8, 760 },
	 { 8, 776 },
	 { 8, 792 },
	 { 8, 808 },
	 { 8, 824 },
	 { 7, 840 },
	 { 8, 854 },
	 { 8, 870 },
	 { 8, 886 },
	 { 4, 902 },
	 { 7, 910 },
	 { 1, 94 },
	 { 8, 924 },
	 { 2, 940 },
	 { 6, 944 },
	 { 1, 956 },
	 { 6, 958 },
	 { 8, 970 },
	 { 1, 986 },
	 { 8, 988 },
	 { 8, 1004 },
	 { 8, 1020 },
	 { 1, 308 },
	 { 8, 1036 },
	 { 8, 1052 },
	 { 8, 1068 },
	 { 1, 682 },
	 { 8, 1084 },
	 { 8, 1100 },
	 { 8, 1116 },
	 { 6, 944 },
	 { 5, 1132 },
	 { 8, 292 },
	 { 1, 94 },
	 { 8, 1142 },
	 { 8, 1158 },
	 { 8, 1174 },
	 { 8, 1190 },
	 { 8, 1206 },
	 { 8, 1222 },
	 { 8, 1238 },
	 { 8, 1254 },
	 { 8, 1270 },
	 { 8, 760 },
	 { 1, 94 },
	 { 1, 94 },
	 { 8, 1286 },
	 { 8, 1302 },
	 { 1, 94 },
	 { 8, 1318 },
	 { 7, 1334 },
	 { 8, 760 },
	 { 8, 1348 },
	 { 1, 94 },
	 { 8, 292 },
	 { 8, 1364 },
	 { 6, 1380 },
	 { 1, 94 },
	 { 8, 1392 },
	 { 8, 292 },
	 { 8, 1408 },
	 { 8, 1424 },
	 { 1, 94 },
	 { 1, 94 },
	 { 7, 1440 },
	 { 8, 292 },
	 { 8, 700 },
	 { 8, 1454 },
	 { 8, 1470 },
	 { 1, 96 },
	 { 8, 1486 },
	 { 8, 1502 },
	 { 8, 1518 },
	 { 1, 94 },
	 { 8, 1534 },
	 { 8, 292 },
	 { 8, 1550 },
	 { 3, 1566 },
	 { 8, 1572 },
	 { 8, 1588 },
	 { 8, 1604 },
	 { 1, 94 },
	 { 1, 94 },
	 { 8, 1620 },
	 { 8, 700 },
	 { 8, 1636 },
	 { 8, 392 },
	 { 8, 1652 },
	 { 1, 1668 },
	 { 1, 94 },
	 { 8, 292 },
	 { 8, 1670 },
	 { 8, 1686 },
	 { 8, 760 },
	 { 1, 94 },
	 { 1, 94 },
	 { 6, 1380 },
	 { 8, 1702 },
	 { 8, 1718 },
	 { 1, 98 },
	 { 8, 1734 },
	 { 8, 1750 },
	 { 6, 1766 },
	 { 1, 94 },
	 { 8, 292 },
	 { 9, 1778 },
	 { 8, 1796 },
	 { 8, 1812 },
	 { 1, 94 },
	 { 1, 94 },
	 { 1, 94 },
	 { 1, 94 },
	 { 8, 1828 },
	 { 1, 94 },
	 { 8, 1844 },
	 { 1, 94 },
	 { 1, 94 },
	 { 1, 94 },
	 { 1, 94 },
	 { 1, 94 },
	 { 1, 1860 },
	 { 1, 308 },
	 { 1, 308 },
	 { 1, 308 },
	 { 1, 308 },
	 { 8, 1862 },
	 { 1, 94 },
	 { 8, 1878 },
	 { 8, 1878 },
	 { 8, 684 },
	 { 8, 1894 },
	 { 1, 94 },
	 { 1, 94 },
	 { 1, 94 },
	 { 1, 1910 },
	 { 1, 94 },
	 { 1, 94 },
};

static char carcols[1912] = {
	101,1, 113,1, 123,1, 36,1, 4,1, 40,1, 62,1, 75,1,
	113,113, 41,41, 47,47, 52,52, 66,66, 74,74, 87,87,
	10,10, 110,110, 13,13, 22,22, 30,30, 39,39, 90,90, 98,98,
	101,1, 113,1, 25,1, 28,1, 30,1, 36,1, 37,1, 40,1,
	101,101, 109,100, 113,39, 119,50, 123,92, 66,25, 83,110, 95,105,
	11,1, 123,1, 24,1, 36,1, 4,1, 40,1, 75,1, 91,1,
	1,1,
	3,1,
	26,26,
	10,1, 25,1, 36,1, 4,1, 40,1, 45,1, 84,1, 9,1,
	106,1, 112,1, 116,1, 12,1, 123,1, 64,1, 75,1, 80,1,
	10,8, 11,1, 25,8, 27,1, 29,8, 30,1, 37,8, 9,1,
	105,1, 109,1, 119,1, 25,1, 4,1, 87,1, 88,1, 91,1,
	24,1, 25,1, 28,1, 43,1, 67,1, 72,1, 9,1, 95,1,
	0,1, 20,1, 25,1, 36,1, 40,1, 62,1, 75,1, 92,1,
	1,3,
	108,108, 114,114, 117,227, 119,119, 41,41, 61,61, 81,81, 95,95,
	13,76, 33,75, 45,75, 47,76, 54,75, 59,75, 69,76, 87,76,
	6,1,
	13,1, 25,1, 30,1, 36,1, 4,1, 40,1, 75,1, 95,1,
	101,25, 111,31, 113,36, 67,59, 83,57, 96,25, 97,25,
	1,16, 1,17, 1,35, 1,5, 1,53, 1,56,
	1,0, 15,30, 2,2, 24,53, 3,2, 3,6, 35,61, 6,16,
	43,0,
	10,10, 11,11, 15,15, 37,37, 42,42, 53,53, 62,62, 7,7,
	0,1,
	4,75,
	1,2, 1,3, 10,10, 12,12, 13,13, 14,14, 2,1, 3,1,
	46,26,
	47,74, 55,83, 59,83, 71,59, 71,87, 75,59, 82,87, 92,72,
	1,1, 12,12, 2,2, 4,4, 46,46, 53,53, 6,6,
	109,1, 11,1, 119,45, 83,1, 87,1, 92,1, 95,1,
	105,20, 123,20, 125,21, 54,7, 79,7, 87,7, 95,16, 98,20,
	6,76,
	25,78, 37,78, 43,21, 54,38, 57,8, 65,79, 67,8, 8,17,
	110,110, 118,118, 121,121, 20,20, 32,32, 34,34, 66,66, 84,84,
	110,54, 126,96, 2,96, 67,86, 67,98, 70,96, 79,42, 82,54,
	0,0, 0,109, 0,36, 11,105, 25,109, 36,0, 40,36, 75,36,
	20,1, 24,1, 25,1, 36,1, 4,1, 40,1, 54,1, 84,1,
	32,14, 32,32, 32,36, 32,42, 32,53, 32,66,
	34,34, 35,35, 37,37, 39,39, 41,41, 43,43, 45,45, 47,47,
	0,0, 1,22, 1,35, 1,44, 1,5, 1,53, 1,57, 3,3,
	75,2,
	1,74,
	123,123, 125,125, 16,16, 18,18, 36,36, 46,46, 61,61, 75,75,
	1,16, 1,22, 1,3, 1,35, 1,44, 1,5, 1,53, 1,57,
	56,56,
	1,31, 32,74, 32,74, 43,31, 77,31, 84,15, 84,31, 84,58,
	102,65, 105,72, 110,93, 12,95, 121,93, 23,1, 84,63, 91,63,
	13,1, 18,1, 2,1, 32,1, 34,1, 45,1, 58,1, 63,1,
	101,1, 109,1, 113,1, 25,1, 30,1, 36,1, 4,1, 91,1,
	106,106, 112,112, 26,26, 28,28, 44,44, 51,51, 57,57, 72,72,
	1,18, 1,3, 1,30, 1,9, 17,23, 46,23, 46,32, 57,34,
	36,1, 37,1, 43,1, 53,1, 61,1, 75,1, 79,1, 88,1,
	11,11, 19,19, 22,22, 36,36, 53,53, 7,7, 79,79, 84,84,
	14,75,
	16,76, 18,76, 2,76, 25,76, 45,88, 67,76, 68,76, 78,76,
	13,8, 2,1, 22,1, 36,8, 51,1, 58,8, 60,1, 68,8,
	3,3, 46,46, 53,53, 6,6,
	103,111, 120,112, 120,113, 120,114, 120,117, 66,71, 74,83, 74,91,
	56,15, 56,53,
	105,1, 110,1, 74,1, 81,1, 83,1, 84,1, 91,1, 97,1,
	17,1, 2,39, 21,1, 33,0, 37,0, 41,29, 56,29, 9,39,
	1,6, 103,102, 119,117, 6,7, 7,6, 71,77, 77,87, 89,91,
	0,1, 101,1, 121,1, 22,1, 36,1, 75,1, 92,1, 94,1,
	20,1, 35,1, 39,1, 40,1, 45,1, 59,1, 66,1, 72,1,
	27,36, 40,29, 45,32, 49,23, 54,31, 55,41, 59,36, 60,35,
	12,12, 2,2, 4,4, 46,46, 53,53, 6,6, 73,45,
	1,1, 12,9, 14,1, 26,1, 3,3, 46,46, 6,6, 65,9,
	10,10, 41,41, 48,48, 52,52, 62,62, 64,64, 71,71, 85,85,
	0,6, 1,20, 1,31, 1,31, 1,5, 16,0, 17,0, 3,6,
	12,35, 40,26, 50,32, 66,36,
	1,73, 1,74, 1,75, 1,76, 1,77, 1,78, 1,79,
	12,39, 26,14, 26,3, 26,57, 29,42, 3,29, 54,29, 74,35,
	2,26, 2,29,
	112,120, 120,123, 13,118, 14,123, 76,102, 84,110,
	0,0,
	30,72, 40,65, 52,66, 60,72, 64,72, 71,72,
	24,55, 28,56, 30,26, 49,59, 52,69, 71,107, 77,26, 81,27,
	36,13,
	36,117, 36,13, 42,30, 42,33, 54,36, 75,79, 92,101, 98,109,
	101,106, 114,108, 116,115, 118,117, 119,122, 123,124, 5,6, 88,99,
	0,0, 20,20, 22,22, 37,19, 53,56, 66,72, 74,72, 9,14,
	11,123, 13,120, 20,117, 24,112, 27,107, 36,105, 37,107, 43,93,
	10,32, 109,25, 109,32, 112,32, 30,44, 32,52, 84,66, 84,69,
	13,119, 21,119, 25,119, 28,119, 4,119, 40,110, 40,84, 75,84,
	1,58, 36,88, 51,75, 53,75, 7,94, 75,61, 75,67, 79,62,
	101,100, 103,101, 117,116, 123,36, 83,66, 87,74, 87,75, 98,83,
	12,9, 14,1, 26,1, 34,9, 45,29, 51,39, 57,38, 65,9,
	3,3, 52,52, 6,6, 7,7, 76,76,
	16,1, 25,1, 30,1, 36,1, 53,1, 61,1, 7,1, 74,1,
	16,16, 2,2, 28,28, 39,39, 43,43, 46,46, 5,5, 6,6,
	12,60, 27,97, 3,90, 34,51, 37,51, 4,90, 7,68, 8,66,
	15,123, 17,39, 32,112, 45,88, 52,71, 57,67, 61,96, 96,96,
	21,34, 21,36, 30,34, 38,51, 48,18, 51,6, 54,34, 55,20,
	10,1, 113,1, 25,1, 28,1, 36,1, 40,1, 54,1, 75,1,
	11,76, 13,76, 24,77, 39,78, 42,76, 54,77, 62,77, 63,78,
	116,1, 119,1, 122,1, 24,1, 27,1, 36,1, 4,1, 9,1,
	36,36, 37,36, 40,36, 43,41, 47,41, 51,72, 54,75, 55,84,
	115,118, 118,118, 25,118, 36,0, 74,74, 75,13, 87,118, 92,3,
	3,3, 3,8, 36,105, 39,106, 51,118, 6,25, 7,79, 8,82,
	60,24, 61,27, 61,61, 62,61, 65,31, 81,35, 83,64, 83,66,
	1,1, 17,20, 18,20, 22,30, 36,43, 44,51, 52,54,
	109,1, 52,1, 53,1, 66,1, 75,1, 76,1, 81,1, 95,1,
	110,1, 111,1, 112,1, 114,1, 119,1, 122,1, 13,1, 4,1,
	11,22, 2,35, 36,2, 40,35, 51,53, 91,2,
	73,1, 74,1, 75,1, 77,1, 79,1, 83,1, 84,1, 91,1,
	118,1, 123,1, 28,1, 3,1, 31,1, 55,1, 66,1, 97,1,
	12,1, 26,96, 30,96, 32,1, 37,1, 57,96, 71,96, 9,1,
	61,98, 70,86, 75,75, 75,91, 79,74, 86,70, 96,67,
	113,92, 119,113, 122,113, 13,118, 24,118, 31,93, 32,92, 45,92,
	11,11, 2,2, 32,8, 43,8, 67,8, 76,8, 8,90, 83,13,
	28,96, 30,1, 39,1, 40,96, 44,96, 47,1, 50,1, 9,96,
	113,78, 119,62, 2,62, 2,78, 3,87, 62,37, 7,78, 78,38,
	10,1, 122,1, 123,1, 125,1, 24,1, 37,1, 55,1, 66,1,
	72,39, 74,39, 75,39, 79,39, 83,36, 84,36, 89,35, 91,35,
	101,1, 109,1, 20,1, 67,1, 72,1, 75,1, 83,1, 91,1,
	26,124, 49,49, 56,56,
	102,119, 111,3, 38,9, 55,23, 61,74, 71,87, 91,87, 98,114,
	12,32, 14,32, 15,32, 34,30, 43,32, 45,32, 53,32, 65,32,
	13,1, 2,1, 22,1, 36,1, 51,1, 58,1, 60,1, 68,1,
	112,1, 116,1, 117,1, 24,1, 30,1, 35,1, 36,1, 40,1,
	17,1, 21,1, 33,0, 37,0, 41,29, 52,39, 56,29, 9,39,
	101,1, 11,1, 113,1, 116,1, 17,1, 35,1, 36,1, 92,1,
	1,6,
	109,1, 30,8, 52,8, 71,1, 72,8, 83,1, 84,8, 95,1,
	102,114, 114,1, 88,64, 90,96, 93,64, 97,96, 97,96, 99,81,
	101,1, 116,1, 117,1, 25,1, 30,1, 37,1, 4,1, 94,1,
	115,43, 77,18, 78,8, 79,18, 79,7, 85,6, 86,24, 91,38,
	12,1, 19,96, 25,96, 31,64, 38,1, 51,96, 57,1, 66,96,
	67,1, 68,96, 72,1, 74,8, 75,96, 76,8, 79,1, 84,96,
	1,3, 23,31, 40,44, 8,10, 8,16, 8,7,
	43,43, 47,47, 51,51, 53,53, 61,61, 66,66, 67,67, 81,81, 92,92,
	101,1, 36,1, 54,1, 58,1, 66,1, 72,1, 75,1, 87,1,
	110,113, 112,116, 114,118, 119,101, 41,10, 41,20, 49,11, 56,123,
	101,1, 36,1, 40,1, 43,1, 53,1, 72,1, 75,1, 95,1,
	112,112, 114,114, 124,124, 22,22, 23,23, 31,31, 37,37, 7,7,
	112,20,
	11,11, 2,2, 32,8, 43,8, 67,8, 8,90, 81,8, 83,13,
	13,1, 18,1, 32,1, 34,1, 45,45, 58,1, 69,1, 75,77,
	11,11, 2,2, 32,8, 43,8, 61,8, 67,8, 8,90, 83,13,
	36,36,
};

struct ZONE {
	float min_x, min_y, min_z, max_x, max_y, max_z;
	int id;
};

struct REGION {
	struct ZONE zone;
	int from_zone_idx, to_zone_idx_exclusive;
};

/* see info.zon and american.gxt */
#define ZONE_ALDEA 0
#define ZONE_ANGPI 1
#define ZONE_ARCO 2
#define ZONE_BACKO 3
#define ZONE_BARRA 4
#define ZONE_BATTP 5
#define ZONE_BAYV 6
#define ZONE_BEACO 7
#define ZONE_BFC 8
#define ZONE_BFLD 9
#define ZONE_BIGE 10
#define ZONE_BINT 11
#define ZONE_BLUAC 12
#define ZONE_BLUEB 13
#define ZONE_BLUF 14
#define ZONE_BONE 15
#define ZONE_BRUJA 16
#define ZONE_BYTUN 17
#define ZONE_CALI 18
#define ZONE_CALT 19
#define ZONE_CAM 20
#define ZONE_CARSO 21
#define ZONE_CHC 22
#define ZONE_CHINA 23
#define ZONE_CITYS 24
#define ZONE_CIVI 25
#define ZONE_COM 26
#define ZONE_CONF 27
#define ZONE_CRANB 28
#define ZONE_CREE 29
#define ZONE_CREEK 30
#define ZONE_CUNTC 31
#define ZONE_DAM 32
#define ZONE_DILLI 33
#define ZONE_DOH 34
#define ZONE_DRAG 35
#define ZONE_EASB 36
#define ZONE_EBAY 37
#define ZONE_EBE 38
#define ZONE_ELCA 39
#define ZONE_ELCO 40
#define ZONE_ELQUE 41
#define ZONE_ELS 42
#define ZONE_ESPE 43
#define ZONE_ESPN 44
#define ZONE_ETUNN 45
#define ZONE_FALLO 46
#define ZONE_FARM 47
#define ZONE_FERN 48
#define ZONE_FINA 49
#define ZONE_FISH 50
#define ZONE_FLINTC 51
#define ZONE_FLINTI 52
#define ZONE_FLINTR 53
#define ZONE_FLINW 54
#define ZONE_FRED 55
#define ZONE_GAN 56
#define ZONE_GANTB 57
#define ZONE_GARC 58
#define ZONE_GARV 59
#define ZONE_GGC 60
#define ZONE_GLN 61
#define ZONE_HANKY 62
#define ZONE_HASH 63
#define ZONE_HAUL 64
#define ZONE_HBARNS 65
#define ZONE_HGP 66
#define ZONE_HIGH 67
#define ZONE_HILLP 68
#define ZONE_ISLE 69
#define ZONE_IWD 70
#define ZONE_JEF 71
#define ZONE_JTE 72
#define ZONE_JTN 73
#define ZONE_JTS 74
#define ZONE_JTW 75
#define ZONE_JUNIHI 76
#define ZONE_JUNIHO 77
#define ZONE_KACC 78
#define ZONE_KINC 79
#define ZONE_LA 80
#define ZONE_LAIR 81
#define ZONE_LDM 82
#define ZONE_LDOC 83
#define ZONE_LDS 84
#define ZONE_LDT 85
#define ZONE_LEAFY 86
#define ZONE_LFL 87
#define ZONE_LIND 88
#define ZONE_LINDEN 89
#define ZONE_LMEX 90
#define ZONE_LOT 91
#define ZONE_LSINL 92
#define ZONE_LST 93
#define ZONE_LVA 94
#define ZONE_MAKO 95
#define ZONE_MAR 96
#define ZONE_MARKST 97
#define ZONE_MART 98
#define ZONE_MEAD 99
#define ZONE_MKT 100
#define ZONE_MONINT 101
#define ZONE_MONT 102
#define ZONE_MTCHI 103
#define ZONE_MUL 104
#define ZONE_MULINT 105
#define ZONE_NROCK 106
#define ZONE_OCEAF 107
#define ZONE_OCTAN 108
#define ZONE_OVS 109
#define ZONE_PALMS 110
#define ZONE_PALO 111
#define ZONE_PANOP 112
#define ZONE_PARA 113
#define ZONE_PAYAS 114
#define ZONE_PER1 115
#define ZONE_PILL 116
#define ZONE_PINK 117
#define ZONE_PINT 118
#define ZONE_PIRA 119
#define ZONE_PLS 120
#define ZONE_PROBE 121
#define ZONE_PRP 122
#define ZONE_QUARY 123
#define ZONE_RED 124
#define ZONE_REDE 125
#define ZONE_REDW 126
#define ZONE_REST 127
#define ZONE_RIE 128
#define ZONE_RIH 129
#define ZONE_RING 130
#define ZONE_ROBAD 131
#define ZONE_ROBINT 132
#define ZONE_ROCE 133
#define ZONE_ROD 134
#define ZONE_ROY 135
#define ZONE_RSE 136
#define ZONE_RSW 137
#define ZONE_SANB 138
#define ZONE_SASO 139
#define ZONE_SF 140
#define ZONE_SFAIR 141
#define ZONE_SFDWT 142
#define ZONE_SHACA 143
#define ZONE_SHERR 144
#define ZONE_SILLY 145
#define ZONE_SMB 146
#define ZONE_SPIN 147
#define ZONE_SRY 148
#define ZONE_STAR 149
#define ZONE_STRIP 150
#define ZONE_SUN 151
#define ZONE_SUNMA 152
#define ZONE_SUNNN 153
#define ZONE_THEA 154
#define ZONE_TOM 155
#define ZONE_TOPFA 156
#define ZONE_UNITY 157
#define ZONE_VAIR 158
#define ZONE_VALLE 159
#define ZONE_VE 160
#define ZONE_VERO 161
#define ZONE_VIN 162
#define ZONE_VISA 163
#define ZONE_WESTP 164
#define ZONE_WHET 165
#define ZONE_WWE 166
#define ZONE_YBELL 167
#define ZONE_YELLOW 168
#define ZONE_NONE_NW 169
#define ZONE_NONE_NE 170
#define ZONE_NONE_SW 171
#define ZONE_NONE_SE 172
#define ZONE_INVALID 173
#define ZONE_OCTA 174
#define ZONE_OCTA_AIRPORT 175
#define ZONE_OCTA_RESIDENTIAL 176
#define ZONE_OCTA_MOUNTAIN 177
#define ZONE_OCTA_BEACH 178
#define ZONE_CATA 179
#define ZONE_CATA_CITY 179

/* see info.zon and american.gxt */
static char *zonenames[] = {
	"Aldea Malvada",
	"Angel Pine",
	"Arco del Oeste",
	"Back o Beyond",
	"Las Barrancas",
	"Battery Point",
	"Palisades",
	"Beacon Hill",
	"Blackfield Chapel",
	"Blackfield",
	"'The Big Ear'",
	"Blackfield Intersection",
	"Blueberry Acres",
	"Blueberry",
	"Verdant Bluffs",
	"Bone County",
	"Las Brujas",
	"Bayside Tunnel",
	"Caligula's Palace",
	"Calton Heights",
	"The Camel's Toe",
	"Fort Carson",
	"Las Colinas",
	"Chinatown",
	"City Hall",
	"Santa Flora",
	"Commerce",
	"Conference Center",
	"Cranberry Station",
	"Creek",
	"Shady Creeks",
	"Avispa Country Club",
	"The Sherman Dam",
	"Dillimore",
	"Doherty",
	"The Four Dragons Casino",
	"Easter Basin",
	"Easter Bay Chemicals",
	"East Beach",
	"El Castillo del Diablo",
	"El Corona",
	"El Quebrados",
	"East Los Santos",
	"Esplanade East",
	"Esplanade North",
	"Easter Tunnel",
	"Fallow Bridge",
	"The Farm",
	"Fern Ridge",
	"Financial",
	"Fisher's Lagoon",
	"Flint County",
	"Flint Intersection",
	"Flint Range",
	"Flint Water",
	"Frederick Bridge",
	"Ganton",
	"Gant Bridge",
	"Garcia",
	"Garver Bridge",
	"Greenglass College",
	"Glen Park",
	"Hankypanky Point",
	"Hashbury",
	"Fallen Tree",
	"Hampton Barns",
	"Harry Gold Parkway",
	"The High Roller",
	"Missionary Hill",
	"The Emerald Isle",
	"Idlewood",
	"Jefferson",
	"Julius Thruway East",
	"Julius Thruway North",
	"Julius Thruway South",
	"Julius Thruway West",
	"Juniper Hill",
	"Juniper Hollow",
	"K.A.C.C. Military Fuels",
	"Kincaid Bridge",
	"Los Santos",
	"Los Santos International",
	"Last Dime Motel",
	"Ocean Docks",
	"Linden Side",
	"Downtown Los Santos",
	"Leafy Hollow",
	"Los Flores",
	"Willowfield",
	"Linden Station",
	"Little Mexico",
	"Come-A-Lot",
	"Los Santos Inlet",
	"Linden Station",
	"LVA Freight Depot",
	"The Mako Span",
	"Marina",
	"Market Station",
	"Martin Bridge",
	"Verdant Meadows",
	"Market",
	"Montgomery Intersection",
	"Montgomery",
	"Mount Chiliad",
	"Mulholland",
	"Mulholland Intersection",
	"North Rock",
	"Ocean Flats",
	"Octane Springs",
	"Old Venturas Strip",
	"Green Palms",
	"Palomino Creek",
	"The Panopticon",
	"Paradiso",
	"Las Payasadas",
	"Pershing Square",
	"Pilgrim",
	"The Pink Swan",
	"Pilson Intersection",
	"Pirates in Men's Pants",
	"Playa del Seville",
	"Lil' Probe Inn",
	"Prickle Pine",
	"Hunter Quarry",
	"Red County",
	"Redsands East",
	"Redsands West",
	"Restricted Area",
	"Randolph Industrial Estate",
	"Richman",
	"The Clown's Pocket",
	"Tierra Robada",
	"Robada Intersection",
	"Roca Escalante",
	"Rodeo",
	"Royal Casino",
	"Rockshore East",
	"Rockshore West",
	"San Fierro Bay",
	"San Andreas Sound",
	"San Fierro",
	"Easter Bay Airport",
	"Downtown",
	"Shady Cabin",
	"Sherman Reservoir",
	"Foster Valley",
	"Santa Maria Beach",
	"Spinybed",
	"Sobell Rail Yards",
	"Starfish Casino",
	"The Strip",
	"Temple",
	"Bayside Marina",
	"Bayside",
	"King's",
	"Regular Tom",
	"Hilltop Farm",
	"Unity Station",
	"Las Venturas Airport",
	"Valle Ocultado",
	"Las Venturas",
	"Verona Beach",
	"Vinewood",
	"The Visage",
	"Queens",
	"Whetstone",
	"Whitewood Estates",
	"Yellow Bell Golf Course",
	"Yellow Bell Station",
	"San Andreas (NW)",
	"San Andreas (NE)",
	"San Andreas (SW)",
	"San Andreas (SE)",
	"INVALID ZONE",
	"Octavia Island",
	"Airport",
	"Downtown",
	"Mountain",
	"Beach",
	"Catalina City",
};

static struct REGION regions[] = {
	#define R(A,B,C,D,E,F,G) {{A,B,C,D,E,F,G},0,0}
	R(44.6147f, -2892.97f, -242.99f, 2997.06f, -768.027f, 900.0f, ZONE_LA),
	R(-2997.47f, -1115.58f, -242.99f, -1213.91f, 1659.68f, 900.0f, ZONE_SF),
	R(869.461f, 596.349f, -242.99f, 2997.06f, 2993.87f, 900.0f, ZONE_VE),
	R(-480.539f, 596.349f, -242.99f, 869.461f, 2993.87f, 900.0f, ZONE_BONE),
	R(-2997.47f, 1659.68f, -242.99f, -480.539f, 2993.87f, 900.0f, ZONE_ROBAD),
	R(-1213.91f, 596.349f, -242.99f, -480.539f, 1659.68f, 900.0f, ZONE_ROBAD),
	R(-1213.91f, -768.027f, -242.99f, 2997.06f, 596.349f, 900.0f, ZONE_RED),
	R(-1213.91f, -2892.97f, -242.99f, 44.6147f, -768.027f, 900.0f, ZONE_FLINTC),
	R(-2997.47f, -2892.97f, -242.99f, -1213.91f, -1115.58f, 900.0f, ZONE_WHET),
	R(11740.0f, 5875.0f, -242.99f, 13718.0f, 7471.5f, 900.0f, ZONE_OCTA),
	R(-35585.12f, -2906.60f, -242.99f, -32640.87f, -228.24f, 900.0f, ZONE_CATA),
	#undef R
};

/* zones must be in region order */

static struct ZONE zones[] = {
	/* start LA region zones */
	{ 1692.62f, -1971.8f, -20.4921f, 1812.62f, -1932.8f, 79.5079f, ZONE_UNITY },
	{ 1249.62f, -2394.33f, -89.0839f, 1852.0f, -2179.25f, 110.916f, ZONE_LAIR },
	{ 1852.0f, -2394.33f, -89.0839f, 2089.0f, -2179.25f, 110.916f, ZONE_LAIR },
	{ 930.221f, -2488.42f, -89.0839f, 1249.62f, -2006.78f, 110.916f, ZONE_BLUF },
	{ 1812.62f, -2179.25f, -89.0839f, 1970.62f, -1852.87f, 110.916f, ZONE_ELCO },
	{ 1970.62f, -2179.25f, -89.0839f, 2089.0f, -1852.87f, 110.916f, ZONE_LIND },
	{ 2089.0f, -2235.84f, -89.0839f, 2201.82f, -1989.9f, 110.916f, ZONE_LIND },
	{ 2089.0f, -1989.9f, -89.0839f, 2324.0f, -1852.87f, 110.916f, ZONE_LIND },
	{ 2201.82f, -2095.0f, -89.0839f, 2324.0f, -1989.9f, 110.916f, ZONE_LIND },
	{ 2373.77f, -2697.09f, -89.0837f, 2809.22f, -2330.46f, 110.916f, ZONE_LDOC },
	{ 2201.82f, -2418.33f, -89.0837f, 2324.0f, -2095.0f, 110.916f, ZONE_LDOC },
	{ 647.712f, -1804.21f, -89.0839f, 851.449f, -1577.59f, 110.916f, ZONE_MAR },
	{ 647.712f, -2173.29f, -89.0839f, 930.221f, -1804.21f, 110.916f, ZONE_VERO },
	{ 930.221f, -2006.78f, -89.0839f, 1073.22f, -1804.21f, 110.916f, ZONE_VERO },
	{ 1073.22f, -2006.78f, -89.0839f, 1249.62f, -1842.27f, 110.916f, ZONE_BLUF },
	{ 1249.62f, -2179.25f, -89.0839f, 1692.62f, -1842.27f, 110.916f, ZONE_BLUF },
	{ 1692.62f, -2179.25f, -89.0839f, 1812.62f, -1842.27f, 110.916f, ZONE_ELCO },
	{ 851.449f, -1804.21f, -89.0839f, 1046.15f, -1577.59f, 110.916f, ZONE_VERO },
	{ 647.712f, -1577.59f, -89.0838f, 807.922f, -1416.25f, 110.916f, ZONE_MAR },
	{ 807.922f, -1577.59f, -89.0839f, 926.922f, -1416.25f, 110.916f, ZONE_MAR },
	{ 1161.52f, -1722.26f, -89.0839f, 1323.9f, -1577.59f, 110.916f, ZONE_VERO },
	{ 1046.15f, -1722.26f, -89.0839f, 1161.52f, -1577.59f, 110.916f, ZONE_VERO },
	{ 1046.15f, -1804.21f, -89.0839f, 1323.9f, -1722.26f, 110.916f, ZONE_CONF },
	{ 1073.22f, -1842.27f, -89.0839f, 1323.9f, -1804.21f, 110.916f, ZONE_CONF },
	{ 1323.9f, -1842.27f, -89.0839f, 1701.9f, -1722.26f, 110.916f, ZONE_COM },
	{ 1323.9f, -1722.26f, -89.0839f, 1440.9f, -1577.59f, 110.916f, ZONE_COM },
	{ 1370.85f, -1577.59f, -89.084f, 1463.9f, -1384.95f, 110.916f, ZONE_COM },
	{ 1463.9f, -1577.59f, -89.0839f, 1667.96f, -1430.87f, 110.916f, ZONE_COM },
	{ 1440.9f, -1722.26f, -89.0839f, 1583.5f, -1577.59f, 110.916f, ZONE_PER1 },
	{ 1583.5f, -1722.26f, -89.0839f, 1758.9f, -1577.59f, 110.916f, ZONE_COM },
	{ 1701.9f, -1842.27f, -89.0839f, 1812.62f, -1722.26f, 110.916f, ZONE_LMEX },
	{ 1758.9f, -1722.26f, -89.0839f, 1812.62f, -1577.59f, 110.916f, ZONE_LMEX },
	{ 1667.96f, -1577.59f, -89.0839f, 1812.62f, -1430.87f, 110.916f, ZONE_COM },
	{ 1812.62f, -1852.87f, -89.0839f, 1971.66f, -1742.31f, 110.916f, ZONE_IWD },
	{ 1812.62f, -1742.31f, -89.0839f, 1951.66f, -1602.31f, 110.916f, ZONE_IWD },
	{ 1951.66f, -1742.31f, -89.0839f, 2124.66f, -1602.31f, 110.916f, ZONE_IWD },
	{ 1812.62f, -1602.31f, -89.0839f, 2124.66f, -1449.67f, 110.916f, ZONE_IWD },
	{ 2124.66f, -1742.31f, -89.0839f, 2222.56f, -1494.03f, 110.916f, ZONE_IWD },
	{ 1812.62f, -1449.67f, -89.0839f, 1996.91f, -1350.72f, 110.916f, ZONE_GLN },
	{ 1812.62f, -1100.82f, -89.0839f, 1994.33f, -973.38f, 110.916f, ZONE_GLN },
	{ 1996.91f, -1449.67f, -89.0839f, 2056.86f, -1350.72f, 110.916f, ZONE_JEF },
	{ 2124.66f, -1494.03f, -89.0839f, 2266.21f, -1449.67f, 110.916f, ZONE_JEF },
	{ 2056.86f, -1372.04f, -89.0839f, 2281.45f, -1210.74f, 110.916f, ZONE_JEF },
	{ 2056.86f, -1210.74f, -89.0839f, 2185.33f, -1126.32f, 110.916f, ZONE_JEF },
	{ 2185.33f, -1210.74f, -89.0839f, 2281.45f, -1154.59f, 110.916f, ZONE_JEF },
	{ 1994.33f, -1100.82f, -89.0839f, 2056.86f, -920.815f, 110.916f, ZONE_CHC },
	{ 2056.86f, -1126.32f, -89.0839f, 2126.86f, -920.815f, 110.916f, ZONE_CHC },
	{ 2185.33f, -1154.59f, -89.0839f, 2281.45f, -934.489f, 110.916f, ZONE_CHC },
	{ 2126.86f, -1126.32f, -89.0839f, 2185.33f, -934.489f, 110.916f, ZONE_CHC },
	{ 1971.66f, -1852.87f, -89.0839f, 2222.56f, -1742.31f, 110.916f, ZONE_IWD },
	{ 2222.56f, -1852.87f, -89.0839f, 2632.83f, -1722.33f, 110.916f, ZONE_GAN },
	{ 2222.56f, -1722.33f, -89.0839f, 2632.83f, -1628.53f, 110.916f, ZONE_GAN },
	{ 2541.7f, -1941.4f, -89.0839f, 2703.58f, -1852.87f, 110.916f, ZONE_LIND },
	{ 2632.83f, -1852.87f, -89.0839f, 2959.35f, -1668.13f, 110.916f, ZONE_EBE },
	{ 2632.83f, -1668.13f, -89.0839f, 2747.74f, -1393.42f, 110.916f, ZONE_EBE },
	{ 2747.74f, -1668.13f, -89.0839f, 2959.35f, -1498.62f, 110.916f, ZONE_EBE },
	{ 2421.03f, -1628.53f, -89.0839f, 2632.83f, -1454.35f, 110.916f, ZONE_ELS },
	{ 2222.56f, -1628.53f, -89.0839f, 2421.03f, -1494.03f, 110.916f, ZONE_ELS },
	{ 2056.86f, -1449.67f, -89.0839f, 2266.21f, -1372.04f, 110.916f, ZONE_JEF },
	{ 2266.26f, -1494.03f, -89.0839f, 2381.68f, -1372.04f, 110.916f, ZONE_ELS },
	{ 2381.68f, -1494.03f, -89.0839f, 2421.03f, -1454.35f, 110.916f, ZONE_ELS },
	{ 2281.45f, -1372.04f, -89.084f, 2381.68f, -1135.04f, 110.916f, ZONE_ELS },
	{ 2381.68f, -1454.35f, -89.0839f, 2462.13f, -1135.04f, 110.916f, ZONE_ELS },
	{ 2462.13f, -1454.35f, -89.0839f, 2581.73f, -1135.04f, 110.916f, ZONE_ELS },
	{ 2581.73f, -1454.35f, -89.0839f, 2632.83f, -1393.42f, 110.916f, ZONE_LFL },
	{ 2581.73f, -1393.42f, -89.0839f, 2747.74f, -1135.04f, 110.916f, ZONE_LFL },
	{ 2747.74f, -1498.62f, -89.0839f, 2959.35f, -1120.04f, 110.916f, ZONE_EBE },
	{ 2747.74f, -1120.04f, -89.0839f, 2959.35f, -945.035f, 110.916f, ZONE_CHC },
	{ 2632.74f, -1135.04f, -89.0839f, 2747.74f, -945.035f, 110.916f, ZONE_CHC },
	{ 2281.45f, -1135.04f, -89.0839f, 2632.74f, -945.035f, 110.916f, ZONE_CHC },
	{ 1463.9f, -1430.87f, -89.084f, 1724.76f, -1290.87f, 110.916f, ZONE_LDT },
	{ 1724.76f, -1430.87f, -89.0839f, 1812.62f, -1250.9f, 110.916f, ZONE_LDT },
	{ 1463.9f, -1290.87f, -89.084f, 1724.76f, -1150.87f, 110.916f, ZONE_LDT },
	{ 1370.85f, -1384.95f, -89.0839f, 1463.9f, -1170.87f, 110.916f, ZONE_LDT },
	{ 1724.76f, -1250.9f, -89.0839f, 1812.62f, -1150.87f, 110.916f, ZONE_LDT },
	{ 1463.9f, -1150.87f, -89.0839f, 1812.62f, -768.027f, 110.916f, ZONE_MULINT },
	{ 787.461f, -1416.25f, -89.0838f, 1072.66f, -1310.21f, 110.916f, ZONE_MKT },
	{ 787.461f, -1310.21f, -89.0838f, 952.663f, -1130.84f, 110.916f, ZONE_VIN },
	{ 952.663f, -1310.21f, -89.0839f, 1072.66f, -1130.85f, 110.916f, ZONE_MKT },
	{ 1370.85f, -1170.87f, -89.0839f, 1463.9f, -1130.85f, 110.916f, ZONE_LDT },
	{ 1378.33f, -1130.85f, -89.0838f, 1463.9f, -1026.33f, 110.916f, ZONE_LDT },
	{ 1391.05f, -1026.33f, -89.0839f, 1463.9f, -926.999f, 110.916f, ZONE_LDT },
	{ 1252.33f, -1130.85f, -89.0839f, 1378.33f, -1026.33f, 110.916f, ZONE_SUN },
	{ 1252.33f, -1026.33f, -89.0839f, 1391.05f, -926.999f, 110.916f, ZONE_SUN },
	{ 1252.33f, -926.999f, -89.0839f, 1357.0f, -910.17f, 110.916f, ZONE_SUN },
	{ 1357.0f, -926.999f, -89.0838f, 1463.9f, -768.027f, 110.916f, ZONE_MUL },
	{ 1318.13f, -910.17f, -89.0839f, 1357.0f, -768.027f, 110.916f, ZONE_MUL },
	{ 1169.13f, -910.17f, -89.0838f, 1318.13f, -768.027f, 110.916f, ZONE_MUL },
	{ 787.461f, -1130.84f, -89.0839f, 952.604f, -954.662f, 110.916f, ZONE_VIN },
	{ 952.663f, -1130.84f, -89.084f, 1096.47f, -937.184f, 110.916f, ZONE_SUN },
	{ 1096.47f, -1130.84f, -89.0838f, 1252.33f, -1026.33f, 110.916f, ZONE_SUN },
	{ 1096.47f, -1026.33f, -89.0839f, 1252.33f, -910.17f, 110.916f, ZONE_SUN },
	{ 768.694f, -954.662f, -89.0838f, 952.604f, -860.619f, 110.916f, ZONE_MUL },
	{ 687.802f, -860.619f, -89.0839f, 911.802f, -768.027f, 110.916f, ZONE_MUL },
	{ 1096.47f, -910.17f, -89.0838f, 1169.13f, -768.027f, 110.916f, ZONE_MUL },
	{ 952.604f, -937.184f, -89.0839f, 1096.47f, -860.619f, 110.916f, ZONE_MUL },
	{ 911.802f, -860.619f, -89.0838f, 1096.47f, -768.027f, 110.916f, ZONE_MUL },
	{ 342.648f, -2173.29f, -89.0838f, 647.712f, -1684.65f, 110.916f, ZONE_SMB },
	{ 72.6481f, -2173.29f, -89.0839f, 342.648f, -1684.65f, 110.916f, ZONE_SMB },
	{ 72.6481f, -1684.65f, -89.084f, 225.165f, -1544.17f, 110.916f, ZONE_ROD },
	{ 72.6481f, -1544.17f, -89.0839f, 225.165f, -1404.97f, 110.916f, ZONE_ROD },
	{ 225.165f, -1684.65f, -89.0839f, 312.803f, -1501.95f, 110.916f, ZONE_ROD },
	{ 225.165f, -1501.95f, -89.0839f, 334.503f, -1369.62f, 110.916f, ZONE_ROD },
	{ 334.503f, -1501.95f, -89.0839f, 422.68f, -1406.05f, 110.916f, ZONE_ROD },
	{ 312.803f, -1684.65f, -89.0839f, 422.68f, -1501.95f, 110.916f, ZONE_ROD },
	{ 422.68f, -1684.65f, -89.0839f, 558.099f, -1570.2f, 110.916f, ZONE_ROD },
	{ 558.099f, -1684.65f, -89.0839f, 647.522f, -1384.93f, 110.916f, ZONE_ROD },
	{ 466.223f, -1570.2f, -89.0839f, 558.099f, -1385.07f, 110.916f, ZONE_ROD },
	{ 422.68f, -1570.2f, -89.0839f, 466.223f, -1406.05f, 110.916f, ZONE_ROD },
	{ 647.557f, -1227.28f, -89.0839f, 787.461f, -1118.28f, 110.916f, ZONE_VIN },
	{ 647.557f, -1118.28f, -89.0839f, 787.461f, -954.662f, 110.916f, ZONE_RIH },
	{ 647.557f, -954.662f, -89.0839f, 768.694f, -860.619f, 110.916f, ZONE_RIH },
	{ 466.223f, -1385.07f, -89.0839f, 647.522f, -1235.07f, 110.916f, ZONE_ROD },
	{ 334.503f, -1406.05f, -89.0839f, 466.223f, -1292.07f, 110.916f, ZONE_ROD },
	{ 225.165f, -1369.62f, -89.0839f, 334.503f, -1292.07f, 110.916f, ZONE_RIH },
	{ 225.165f, -1292.07f, -89.084f, 466.223f, -1235.07f, 110.916f, ZONE_RIH },
	{ 72.6481f, -1404.97f, -89.0839f, 225.165f, -1235.07f, 110.916f, ZONE_RIH },
	{ 72.6481f, -1235.07f, -89.0839f, 321.356f, -1008.15f, 110.916f, ZONE_RIH },
	{ 321.356f, -1235.07f, -89.0839f, 647.522f, -1044.07f, 110.916f, ZONE_RIH },
	{ 321.356f, -1044.07f, -89.0839f, 647.557f, -860.619f, 110.916f, ZONE_RIH },
	{ 321.356f, -860.619f, -89.0839f, 687.802f, -768.027f, 110.916f, ZONE_RIH },
	{ 1812.62f, -1350.72f, -89.0839f, 2056.86f, -1100.82f, 110.916f, ZONE_GLN },
	{ 2324.0f, -2302.33f, -89.0839f, 2703.58f, -2145.1f, 110.916f, ZONE_LDOC },
	{ 647.712f, -1416.25f, -89.0839f, 787.461f, -1227.28f, 110.916f, ZONE_VIN },
	{ 787.461f, -1410.93f, -34.1263f, 866.009f, -1310.21f, 65.873f, ZONE_MARKST },
	{ 1072.66f, -1416.25f, -89.084f, 1370.85f, -1130.85f, 110.916f, ZONE_MKT },
	{ 926.922f, -1577.59f, -89.0839f, 1370.85f, -1416.25f, 110.916f, ZONE_MKT },
	{ 2089.0f, -2394.33f, -89.0839f, 2201.82f, -2235.84f, 110.916f, ZONE_LDOC },
	{ 1382.73f, -2730.88f, -89.0839f, 2201.82f, -2394.33f, 110.916f, ZONE_LAIR },
	{ 2201.82f, -2730.88f, -89.0839f, 2324.0f, -2418.33f, 110.916f, ZONE_LDOC },
	{ 1974.63f, -2394.33f, -39.0839f, 2089.0f, -2256.59f, 60.9161f, ZONE_LAIR },
	{ 1400.97f, -2669.26f, -39.0839f, 2189.82f, -2597.26f, 60.9161f, ZONE_LAIR },
	{ 2051.63f, -2597.26f, -39.0839f, 2152.45f, -2394.33f, 60.9161f, ZONE_LAIR },
	{ 2703.58f, -2126.9f, -89.0839f, 2959.35f, -1852.87f, 110.916f, ZONE_PLS },
	{ 2703.58f, -2302.33f, -89.0839f, 2959.35f, -2126.9f, 110.916f, ZONE_LDOC },
	{ 2324.0f, -2145.1f, -89.084f, 2703.58f, -2059.23f, 110.916f, ZONE_LDOC },
	{ 2324.0f, -2059.23f, -89.0839f, 2541.7f, -1852.87f, 110.916f, ZONE_LIND },
	{ 2541.7f, -2059.23f, -89.0839f, 2703.58f, -1941.4f, 110.916f, ZONE_LIND },
	{ 1507.51f, -1385.21f, 110.916f, 1582.55f, -1325.31f, 335.916f, ZONE_LDT },
	/* end LA region zones */
	/* start SF region zones */
	{ -2007.83f, 56.3063f, 0.0f, -1922.0f, 224.782f, 100.0f, ZONE_CRANB },
	{ -2741.07f, 1268.41f, 0.0f, -2533.04f, 1490.47f, 200.0f, ZONE_BATTP },
	{ -2741.07f, 793.411f, 0.0f, -2533.04f, 1268.41f, 200.0f, ZONE_PARA },
	{ -2741.07f, 458.411f, 0.0f, -2533.04f, 793.411f, 200.0f, ZONE_CIVI },
	{ -2994.49f, 458.411f, 0.0f, -2741.07f, 1339.61f, 200.0f, ZONE_BAYV },
	{ -2867.85f, 277.411f, 0.0f, -2593.44f, 458.411f, 200.0f, ZONE_CITYS },
	{ -2994.49f, 277.411f, 0.0f, -2867.85f, 458.411f, 200.0f, ZONE_OCEAF },
	{ -2994.49f, -222.589f, 0.0f, -2593.44f, 277.411f, 200.0f, ZONE_OCEAF },
	{ -2994.49f, -430.276f, 0.0f, -2831.89f, -222.589f, 200.0f, ZONE_OCEAF },
	{ -2270.04f, -430.276f, 0.0f, -2178.69f, -324.114f, 200.0f, ZONE_SILLY },
	{ -2178.69f, -599.884f, 0.0f, -1794.92f, -324.114f, 200.0f, ZONE_SILLY },
	{ -2593.44f, -222.589f, 0.0f, -2411.22f, 54.722f, 200.0f, ZONE_HASH },
	{ -2533.04f, 968.369f, 0.0f, -2274.17f, 1358.9f, 200.0f, ZONE_JUNIHO },
	{ -2533.04f, 1358.9f, 0.0f, -1996.66f, 1501.21f, 200.0f, ZONE_ESPN },
	{ -1996.66f, 1358.9f, 0.0f, -1524.24f, 1592.51f, 200.0f, ZONE_ESPN },
	{ -1982.32f, 1274.26f, 0.0f, -1524.24f, 1358.9f, 200.0f, ZONE_ESPN },
	{ -1871.72f, 744.17f, 0.0f, -1701.3f, 1176.42f, 300.0f, ZONE_FINA },
	{ -2274.17f, 744.17f, 0.0f, -1982.32f, 1358.9f, 200.0f, ZONE_CALT },
	{ -1982.32f, 744.17f, 0.0f, -1871.72f, 1274.26f, 200.0f, ZONE_SFDWT },
	{ -1871.72f, 1176.42f, 0.0f, -1620.3f, 1274.26f, 200.0f, ZONE_SFDWT },
	{ -1700.01f, 744.267f, 0.0f, -1580.01f, 1176.52f, 200.0f, ZONE_SFDWT },
	{ -1580.01f, 744.267f, 0.0f, -1499.89f, 1025.98f, 200.0f, ZONE_SFDWT },
	{ -2533.04f, 578.396f, 0.0f, -2274.17f, 968.369f, 200.0f, ZONE_JUNIHI },
	{ -2274.17f, 578.396f, 0.0f, -2078.67f, 744.17f, 200.0f, ZONE_CHINA },
	{ -2078.67f, 578.396f, 0.0f, -1499.89f, 744.267f, 200.0f, ZONE_SFDWT },
	{ -2329.31f, 458.411f, 0.0f, -1993.28f, 578.396f, 200.0f, ZONE_THEA },
	{ -2411.22f, 265.243f, 0.0f, -1993.28f, 373.539f, 200.0f, ZONE_THEA },
	{ -2253.54f, 373.539f, 0.0f, -1993.28f, 458.411f, 200.0f, ZONE_THEA },
	{ -2411.22f, -222.589f, 0.0f, -2173.04f, 265.243f, 200.0f, ZONE_GARC },
	{ -2270.04f, -324.114f, 0.0f, -1794.92f, -222.589f, 200.0f, ZONE_DOH },
	{ -2173.04f, -222.589f, 0.0f, -1794.92f, 265.243f, 200.0f, ZONE_DOH },
	{ -1993.28f, 265.243f, 0.0f, -1794.92f, 578.396f, 200.0f, ZONE_SFDWT },
	{ -1499.89f, -50.0963f, 0.0f, -1242.98f, 249.904f, 200.0f, ZONE_SFAIR },
	{ -1794.92f, 249.904f, 0.0f, -1242.98f, 578.396f, 200.0f, ZONE_EASB },
	{ -1794.92f, -50.0963f, 0.0f, -1499.89f, 249.904f, 200.0f, ZONE_EASB },
	{ -1620.3f, 1176.52f, 0.0f, -1580.01f, 1274.26f, 200.0f, ZONE_ESPE },
	{ -1580.01f, 1025.98f, 0.0f, -1499.89f, 1274.26f, 200.0f, ZONE_ESPE },
	{ -1499.89f, 578.396f, -79.6152f, -1339.89f, 1274.26f, 20.3848f, ZONE_ESPE },
	{ -1794.92f, -730.118f, 0.0f, -1213.91f, -50.0963f, 200.0f, ZONE_SFAIR },
	{ -2533.04f, 458.411f, 0.0f, -2329.31f, 578.396f, 200.0f, ZONE_WESTP },
	{ -2593.44f, 54.722f, 0.0f, -2411.22f, 458.411f, 200.0f, ZONE_WESTP },
	{ -2411.22f, 373.539f, 0.0f, -2253.54f, 458.411f, 200.0f, ZONE_WESTP },
	{ -2741.07f, 1490.47f, 0.0f, -2616.4f, 1659.68f, 200.0f, ZONE_GANTB },
	{ -2178.69f, -1115.58f, 0.0f, -1794.92f, -599.884f, 200.0f, ZONE_SILLY },
	{ -1242.98f, -50.0963f, 0.0f, -1213.91f, 578.396f, 200.0f, ZONE_SFAIR },
	/*For some reason this GARV zone is too small to contain the
	  top of the bridge, using patched z2 coordinate.*/
	/*{ -1339.89f, 828.129f, -89.08f, -1213.91f, 1057.04f, 110.91f, ZONE_GARV },*/
	{ -1339.89f, 828.129f, -89.0839f, -1213.91f, 1057.04f, 175.0f, ZONE_GARV },
	/*For some reason this GARV zone is way below where it should be,
	  as a result this zone (first part of the Garver bridge coming from
	  SF) is in zone Downtown, so using patched z-coordinates here.*/
	/*{ -1499.89f, 696.442f, -179.61f, -1339.89f, 925.353f, 20.38f, ZONE_GARV },*/
	{ -1499.89f, 696.442f, 20.3848f, -1339.89f, 925.353f, 175.0f, ZONE_GARV },
	{ -1339.89f, 599.218f, -89.0839f, -1213.91f, 828.129f, 110.916f, ZONE_KINC },
	{ -1709.71f, -833.034f, 0.0f, -1446.01f, -730.118f, 200.0f, ZONE_ETUNN },
	{ -2616.4f, 1501.21f, 0.0f, -1996.66f, 1659.68f, 200.0f, ZONE_SANB },
	{ -1315.42f, -405.388f, 15.4061f, -1264.4f, -209.543f, 25.4061f, ZONE_SFAIR },
	{ -1354.39f, -287.398f, 15.4061f, -1315.42f, -209.543f, 25.406f, ZONE_SFAIR },
	{ -1490.33f, -209.543f, 15.4061f, -1264.4f, -148.388f, 25.4061f, ZONE_SFAIR },
	{ -2646.4f, -355.493f, 0.0f, -2270.04f, -222.589f, 200.0f, ZONE_CUNTC },
	{ -2831.89f, -430.276f, 0.0f, -2646.4f, -222.589f, 200.0f, ZONE_CUNTC },
	{ -2994.49f, -811.276f, 0.0f, -2178.69f, -430.276f, 200.0f, ZONE_HILLP },
	{ -2997.47f, -1115.58f, -47.916f, -2178.69f, -971.913f, 576.08f, ZONE_MTCHI },
	{ -2361.51f, -417.199f, 0.0f, -2270.04f, -355.493f, 200.0f, ZONE_CUNTC },
	{ -2667.81f, -302.135f, -28.8305f, -2646.4f, -262.32f, 71.1695f, ZONE_CUNTC },
	{ -2395.14f, -222.589f, 0.0f, -2354.09f, -204.792f, 200.0f, ZONE_GARC },
	{ -2470.04f, -355.493f, 0.0f, -2270.04f, -318.493f, 46.1f, ZONE_CUNTC },
	{ -2550.04f, -355.493f, 0.0f, -2470.04f, -318.493f, 39.7f, ZONE_CUNTC },
	/* end SF region zones */
	/* start VE region zones */
	{ 1377.48f, 2600.43f, -21.9263f, 1492.45f, 2687.36f, 78.0737f, ZONE_YELLOW },
	{ 2811.25f, 1229.59f, -39.594f, 2861.25f, 1407.59f, 60.406f, ZONE_LINDEN },
	{ 2027.4f, 863.229f, -89.0839f, 2087.39f, 1703.23f, 110.916f, ZONE_STRIP },
	{ 2106.7f, 1863.23f, -89.0839f, 2162.39f, 2202.76f, 110.916f, ZONE_STRIP },
	{ 1817.39f, 863.232f, -89.084f, 2027.39f, 1083.23f, 110.916f, ZONE_DRAG },
	{ 1817.39f, 1083.23f, -89.0839f, 2027.39f, 1283.23f, 110.916f, ZONE_PINK },
	{ 1817.39f, 1283.23f, -89.0839f, 2027.39f, 1469.23f, 110.916f, ZONE_HIGH },
	{ 1817.39f, 1469.23f, -89.084f, 2027.4f, 1703.23f, 110.916f, ZONE_PIRA },
	{ 1817.39f, 1863.23f, -89.0839f, 2106.7f, 2011.83f, 110.916f, ZONE_VISA },
	{ 1817.39f, 1703.23f, -89.0839f, 2027.4f, 1863.23f, 110.916f, ZONE_VISA },
	{ 1457.39f, 823.228f, -89.0839f, 2377.39f, 863.229f, 110.916f, ZONE_JTS },
	{ 1197.39f, 1163.39f, -89.0839f, 1236.63f, 2243.23f, 110.916f, ZONE_JTW },
	{ 2377.39f, 788.894f, -89.0839f, 2537.39f, 897.901f, 110.916f, ZONE_JTS },
	{ 2537.39f, 676.549f, -89.0839f, 2902.35f, 943.235f, 110.916f, ZONE_RSE },
	{ 2087.39f, 943.235f, -89.0839f, 2623.18f, 1203.23f, 110.916f, ZONE_LOT },
	{ 2087.39f, 1203.23f, -89.0839f, 2640.4f, 1383.23f, 110.916f, ZONE_CAM },
	{ 2087.39f, 1383.23f, -89.0839f, 2437.39f, 1543.23f, 110.916f, ZONE_ROY },
	{ 2087.39f, 1543.23f, -89.0839f, 2437.39f, 1703.23f, 110.916f, ZONE_CALI },
	{ 2137.4f, 1703.23f, -89.0839f, 2437.39f, 1783.23f, 110.916f, ZONE_CALI },
	{ 2437.39f, 1383.23f, -89.0839f, 2624.4f, 1783.23f, 110.916f, ZONE_PILL },
	{ 2437.39f, 1783.23f, -89.0839f, 2685.16f, 2012.18f, 110.916f, ZONE_STAR },
	{ 2027.4f, 1783.23f, -89.084f, 2162.39f, 1863.23f, 110.916f, ZONE_STRIP },
	{ 2027.4f, 1703.23f, -89.0839f, 2137.4f, 1783.23f, 110.916f, ZONE_STRIP },
	{ 2011.94f, 2202.76f, -89.0839f, 2237.4f, 2508.23f, 110.916f, ZONE_ISLE },
	{ 2162.39f, 2012.18f, -89.0839f, 2685.16f, 2202.76f, 110.916f, ZONE_OVS },
	{ 2498.21f, 2626.55f, -89.0839f, 2749.9f, 2861.55f, 110.916f, ZONE_KACC },
	{ 2749.9f, 1937.25f, -89.0839f, 2921.62f, 2669.79f, 110.916f, ZONE_CREE },
	{ 2749.9f, 1548.99f, -89.0839f, 2923.39f, 1937.25f, 110.916f, ZONE_SRY },
	{ 2749.9f, 1198.99f, -89.0839f, 2923.39f, 1548.99f, 110.916f, ZONE_LST },
	{ 2623.18f, 943.235f, -89.0839f, 2749.9f, 1055.96f, 110.916f, ZONE_JTE },
	{ 2749.9f, 943.235f, -89.0839f, 2923.39f, 1198.99f, 110.916f, ZONE_LDS },
	{ 2685.16f, 1055.96f, -89.0839f, 2749.9f, 2626.55f, 110.916f, ZONE_JTE },
	{ 2498.21f, 2542.55f, -89.0839f, 2685.16f, 2626.55f, 110.916f, ZONE_JTN },
	{ 2536.43f, 2442.55f, -89.0839f, 2685.16f, 2542.55f, 110.916f, ZONE_JTE },
	{ 2625.16f, 2202.76f, -89.0839f, 2685.16f, 2442.55f, 110.916f, ZONE_JTE },
	{ 2237.4f, 2542.55f, -89.0839f, 2498.21f, 2663.17f, 110.916f, ZONE_JTN },
	{ 2121.4f, 2508.23f, -89.0839f, 2237.4f, 2663.17f, 110.916f, ZONE_JTN },
	{ 1938.8f, 2508.23f, -89.0839f, 2121.4f, 2624.23f, 110.916f, ZONE_JTN },
	{ 1534.56f, 2433.23f, -89.0839f, 1848.4f, 2583.23f, 110.916f, ZONE_JTN },
	{ 1236.63f, 2142.86f, -89.084f, 1297.47f, 2243.23f, 110.916f, ZONE_JTW },
	{ 1848.4f, 2478.49f, -89.0839f, 1938.8f, 2553.49f, 110.916f, ZONE_JTN },
	{ 1777.39f, 863.232f, -89.0839f, 1817.39f, 2342.83f, 110.916f, ZONE_HGP },
	{ 1817.39f, 2011.83f, -89.0839f, 2106.7f, 2202.76f, 110.916f, ZONE_REDE },
	{ 1817.39f, 2202.76f, -89.0839f, 2011.94f, 2342.83f, 110.916f, ZONE_REDE },
	{ 1848.4f, 2342.83f, -89.084f, 2011.94f, 2478.49f, 110.916f, ZONE_REDE },
	{ 1704.59f, 2342.83f, -89.0839f, 1848.4f, 2433.23f, 110.916f, ZONE_JTN },
	{ 1236.63f, 1883.11f, -89.0839f, 1777.39f, 2142.86f, 110.916f, ZONE_REDW },
	{ 1297.47f, 2142.86f, -89.084f, 1777.39f, 2243.23f, 110.916f, ZONE_REDW },
	{ 1377.39f, 2243.23f, -89.0839f, 1704.59f, 2433.23f, 110.916f, ZONE_REDW },
	{ 1704.59f, 2243.23f, -89.0839f, 1777.39f, 2342.83f, 110.916f, ZONE_REDW },
	{ 1236.63f, 1203.28f, -89.0839f, 1457.37f, 1883.11f, 110.916f, ZONE_VAIR },
	{ 1457.37f, 1203.28f, -89.0839f, 1777.39f, 1883.11f, 110.916f, ZONE_VAIR },
	{ 1457.37f, 1143.21f, -89.0839f, 1777.4f, 1203.28f, 110.916f, ZONE_VAIR },
	{ 1457.39f, 863.229f, -89.0839f, 1777.4f, 1143.21f, 110.916f, ZONE_LVA },
	{ 1197.39f, 1044.69f, -89.0839f, 1277.05f, 1163.39f, 110.916f, ZONE_BINT },
	{ 1166.53f, 795.01f, -89.0839f, 1375.6f, 1044.69f, 110.916f, ZONE_BINT },
	{ 1277.05f, 1044.69f, -89.0839f, 1315.35f, 1087.63f, 110.916f, ZONE_BINT },
	{ 1375.6f, 823.228f, -89.084f, 1457.39f, 919.447f, 110.916f, ZONE_BINT },
	{ 1375.6f, 919.447f, -89.0839f, 1457.37f, 1203.28f, 110.916f, ZONE_LVA },
	{ 1277.05f, 1087.63f, -89.0839f, 1375.6f, 1203.28f, 110.916f, ZONE_LVA },
	{ 1315.35f, 1044.69f, -89.0839f, 1375.6f, 1087.63f, 110.916f, ZONE_LVA },
	{ 1236.63f, 1163.41f, -89.0839f, 1277.05f, 1203.28f, 110.916f, ZONE_LVA },
	{ 964.391f, 1044.69f, -89.0839f, 1197.39f, 1203.22f, 110.916f, ZONE_GGC },
	{ 964.391f, 930.89f, -89.0839f, 1166.53f, 1044.69f, 110.916f, ZONE_GGC },
	{ 964.391f, 1203.22f, -89.084f, 1197.39f, 1403.22f, 110.916f, ZONE_BFLD },
	{ 964.391f, 1403.22f, -89.084f, 1197.39f, 1726.22f, 110.916f, ZONE_BFLD },
	{ 2237.4f, 2202.76f, -89.0839f, 2536.43f, 2542.55f, 110.916f, ZONE_ROCE },
	{ 2536.43f, 2202.76f, -89.0839f, 2625.16f, 2442.55f, 110.916f, ZONE_ROCE },
	{ 1823.08f, 596.349f, -89.0839f, 1997.22f, 823.228f, 110.916f, ZONE_LDM },
	{ 1997.22f, 596.349f, -89.0839f, 2377.39f, 823.228f, 110.916f, ZONE_RSW },
	{ 2377.39f, 596.349f, -89.084f, 2537.39f, 788.894f, 110.916f, ZONE_RSW },
	{ 1558.09f, 596.349f, -89.084f, 1823.08f, 823.235f, 110.916f, ZONE_RIE },
	{ 1375.6f, 596.349f, -89.084f, 1558.09f, 823.228f, 110.916f, ZONE_BFC },
	{ 1325.6f, 596.349f, -89.084f, 1375.6f, 795.01f, 110.916f, ZONE_BFC },
	{ 1377.39f, 2433.23f, -89.0839f, 1534.56f, 2507.23f, 110.916f, ZONE_JTN },
	{ 1098.39f, 2243.23f, -89.0839f, 1377.39f, 2507.23f, 110.916f, ZONE_PINT },
	{ 883.308f, 1726.22f, -89.0839f, 1098.31f, 2507.23f, 110.916f, ZONE_WWE },
	{ 1534.56f, 2583.23f, -89.0839f, 1848.4f, 2863.23f, 110.916f, ZONE_PRP },
	{ 1117.4f, 2507.23f, -89.0839f, 1534.56f, 2723.23f, 110.916f, ZONE_PRP },
	{ 1848.4f, 2553.49f, -89.0839f, 1938.8f, 2863.23f, 110.916f, ZONE_PRP },
	{ 2121.4f, 2663.17f, -89.0839f, 2498.21f, 2861.55f, 110.916f, ZONE_SPIN },
	{ 1938.8f, 2624.23f, -89.0839f, 2121.4f, 2861.55f, 110.916f, ZONE_PRP },
	{ 2624.4f, 1383.23f, -89.084f, 2685.16f, 1783.23f, 110.916f, ZONE_PILL },
	{ 1117.4f, 2723.23f, -89.0839f, 1457.46f, 2863.23f, 110.916f, ZONE_YBELL },
	{ 1457.46f, 2723.23f, -89.0839f, 1534.56f, 2863.23f, 110.916f, ZONE_YBELL },
	{ 1515.81f, 1586.4f, -12.5f, 1729.95f, 1714.56f, 87.5f, ZONE_VAIR },
	{ 2437.39f, 1858.1f, -39.0839f, 2495.09f, 1970.85f, 60.9161f, ZONE_STAR },
	{ 2162.39f, 1883.23f, -89.0839f, 2437.39f, 2012.18f, 110.916f, ZONE_STAR },
	{ 2162.39f, 1783.23f, -89.0839f, 2437.39f, 1883.23f, 110.916f, ZONE_RING },
	{ 1098.31f, 1726.22f, -89.0839f, 1197.39f, 2243.23f, 110.916f, ZONE_WWE },
	/* end VE region zones */
	/* start BONE region zones */
	{ -376.233f, 826.326f, 0.0f, 123.717f, 1220.44f, 200.0f, ZONE_CARSO },
	{ 337.244f, 710.84f, -115.239f, 860.554f, 1031.71f, 203.761f, ZONE_QUARY },
	{ 338.658f, 1228.51f, 0.0f, 664.308f, 1655.05f, 200.0f, ZONE_OCTAN },
	{ 176.581f, 1305.45f, 0.0f, 338.658f, 1520.72f, 200.0f, ZONE_PALMS },
	{ -405.77f, 1712.86f, 0.0f, -276.719f, 1892.75f, 200.0f, ZONE_TOM },
	{ -365.167f, 2123.01f, 0.0f, -208.57f, 2217.68f, 200.0f, ZONE_BRUJA },
	{ 37.0325f, 2337.18f, 0.0f, 435.988f, 2677.9f, 200.0f, ZONE_MEAD },
	{ -354.332f, 2580.36f, 0.0f, -133.625f, 2816.82f, 200.0f, ZONE_PAYAS },
	{ -410.02f, 1403.34f, 0.0f, -137.969f, 1681.23f, 200.0f, ZONE_BIGE },
	{ -90.2183f, 1286.85f, 0.0f, 153.859f, 1554.12f, 200.0f, ZONE_PROBE },
	{ -464.515f, 2217.68f, 0.0f, -208.57f, 2580.36f, 200.0f, ZONE_ELCA },
	{ -208.57f, 2123.01f, 0.0f, 114.033f, 2337.18f, 200.0f, ZONE_ELCA },
	{ -208.57f, 2337.18f, 0.0f, 8.42999f, 2487.18f, 200.0f, ZONE_ELCA },
	{ -91.586f, 1655.05f, -50.0f, 421.234f, 2123.01f, 250.0f, ZONE_REST },
	/* end BONE region zones */
	/* start ROBAD1 region zones */
	{ -2353.17f, 2275.79f, 0.0f, -2153.17f, 2475.79f, 200.0f, ZONE_SUNMA },
	{ -2741.07f, 2175.15f, 0.0f, -2353.17f, 2722.79f, 200.0f, ZONE_SUNNN },
	{ -1645.23f, 2498.52f, 0.0f, -1372.14f, 2777.85f, 200.0f, ZONE_ELQUE },
	{ -1372.14f, 2498.52f, 0.0f, -1277.59f, 2615.35f, 200.0f, ZONE_ALDEA },
	{ -968.772f, 1929.41f, 0.0f, -481.126f, 2155.26f, 200.0f, ZONE_DAM },
	{ -901.129f, 2221.86f, 0.0f, -592.09f, 2571.97f, 200.0f, ZONE_ARCO },
	{ -2741.45f, 1659.68f, 0.0f, -2616.4f, 2175.15f, 200.0f, ZONE_GANTB },
	{ -789.737f, 1659.68f, -89.084f, -599.505f, 1929.41f, 110.916f, ZONE_SHERR },
	{ -2290.19f, 2548.29f, -89.084f, -1950.19f, 2723.29f, 110.916f, ZONE_BYTUN },
	{ -936.668f, 2611.44f, 0.0f, -715.961f, 2847.9f, 200.0f, ZONE_VALLE },
	{ -2616.4f, 1659.68f, 0.0f, -1996.66f, 2175.15f, 200.0f, ZONE_SANB },
	/* end ROBAD1 region zones */
	/* start ROBAD2 region zones */
	{ -926.13f, 1398.73f, 0.0f, -719.234f, 1634.69f, 200.0f, ZONE_BARRA },
	/*For some reason this GARV zone is too small to contain the
	  top of the bridge, using patched z2 coordinate.*/
	/*{ -1213.91f, 950.02f, -89.08f, -1087.93f, 1178.93f, 110.916f, ZONE_GARV },*/
	{ -1213.91f, 950.022f, -89.0839f, -1087.93f, 1178.93f, 175.0f, ZONE_GARV },
	{ -1213.91f, 721.111f, -89.0839f, -1087.93f, 950.022f, 110.916f, ZONE_KINC },
	{ -1087.93f, 855.37f, -89.0839f, -961.95f, 986.281f, 110.916f, ZONE_KINC },
	{ -1119.01f, 1178.93f, -89.084f, -862.025f, 1351.45f, 110.916f, ZONE_ROBINT },
	/* end ROBAD2 region zones */
	/* start RED region zones */
	{ -792.254f, -698.555f, 0.0f, -452.404f, -380.043f, 200.0f, ZONE_HAUL },
	{ 2576.92f, 62.1579f, 0.0f, 2759.25f, 385.503f, 200.0f, ZONE_HANKY },
	{ 2160.22f, -149.004f, 0.0f, 2576.92f, 228.322f, 200.0f, ZONE_PALO },
	{ 2285.37f, -768.027f, 0.0f, 2770.59f, -269.74f, 200.0f, ZONE_NROCK },
	{ 1119.51f, 119.526f, 0.0f, 1451.4f, 493.323f, 200.0f, ZONE_MONT },
	{ 1451.4f, 347.457f, 0.0f, 1582.44f, 420.802f, 200.0f, ZONE_MONT },
	{ 603.035f, 264.312f, 0.0f, 761.994f, 366.572f, 200.0f, ZONE_HBARNS },
	{ 508.189f, -139.259f, 0.0f, 1306.66f, 119.526f, 200.0f, ZONE_FERN },
	{ 580.794f, -674.885f, 0.0f, 861.085f, -404.79f, 200.0f, ZONE_DILLI },
	{ 967.383f, -450.39f, 0.0f, 1176.78f, -217.9f, 200.0f, ZONE_TOPFA },
	{ 104.534f, -220.137f, 0.0f, 349.607f, 152.236f, 200.0f, ZONE_BLUEB },
	{ 19.6074f, -404.136f, 0.0f, 349.607f, -220.137f, 200.0f, ZONE_BLUEB },
	{ -947.98f, -304.32f, 0.0f, -319.676f, 327.071f, 200.0f, ZONE_PANOP },
	{ 2759.25f, 296.501f, 0.0f, 2774.25f, 594.757f, 200.0f, ZONE_FRED },
	{ 1664.62f, 401.75f, 0.0f, 1785.14f, 567.203f, 200.0f, ZONE_MAKO },
	{ -319.676f, -220.137f, 0.0f, 104.534f, 293.324f, 200.0f, ZONE_BLUAC },
	{ -222.179f, 293.324f, 0.0f, -122.126f, 476.465f, 200.0f, ZONE_MART },
	{ 434.341f, 366.572f, 0.0f, 603.035f, 555.68f, 200.0f, ZONE_FALLO },
	{ -1132.82f, -768.027f, 0.0f, -956.476f, -578.118f, 200.0f, ZONE_EBAY },
	{ -1213.91f, -730.118f, 0.0f, -1132.82f, -50.0963f, 200.0f, ZONE_SFAIR },
	{ -1213.91f, -50.096f, 0.0f, -947.98f, 578.396f, 200.0f, ZONE_SFAIR },
	{ 1414.07f, -768.027f, -89.0839f, 1667.61f, -452.425f, 110.916f, ZONE_MUL },
	{ 1281.13f, -452.425f, -89.0839f, 1641.13f, -290.913f, 110.916f, ZONE_MUL },
	{ 1269.13f, -768.027f, -89.0839f, 1414.07f, -452.425f, 110.916f, ZONE_MUL },
	{ 737.573f, -768.027f, -89.0838f, 1142.29f, -674.885f, 110.916f, ZONE_MUL },
	{ 861.085f, -674.885f, -89.0839f, 1156.55f, -600.896f, 110.916f, ZONE_MUL },
	{ 321.356f, -768.027f, -89.0839f, 700.794f, -674.885f, 110.916f, ZONE_RIH },
	{ 2450.39f, 385.503f, -100.0f, 2759.25f, 562.349f, 200.0f, ZONE_SASO },
	{ 1916.99f, -233.323f, -100.0f, 2131.72f, 13.8002f, 200.0f, ZONE_FISH },
	{ -314.426f, -753.87f, -89.083f, -106.339f, -463.073f, 110.916f, ZONE_FLINW },
	{ 1546.65f, 208.164f, 0.0f, 1745.83f, 347.457f, 200.0f, ZONE_MONINT },
	{ 1582.44f, 347.457f, 0.0f, 1664.62f, 401.75f, 200.0f, ZONE_MONINT },
	/* end RED region zones */
	/* start FLINTC region zones */
	{ -1166.97f, -2641.19f, 0.0f, -321.744f, -1856.03f, 200.0f, ZONE_BACKO },
	{ -1166.97f, -1856.03f, 0.0f, -815.624f, -1602.07f, 200.0f, ZONE_LEAFY },
	{ -594.191f, -1648.55f, 0.0f, -187.7f, -1276.6f, 200.0f, ZONE_FLINTR },
	{ -1209.67f, -1317.1f, 114.981f, -908.161f, -787.391f, 251.981f, ZONE_FARM },
	{ -321.744f, -2224.43f, -89.083f, 44.6147f, -1724.43f, 110.916f, ZONE_LSINL },
	{ -187.7f, -1596.76f, -89.0839f, 17.0632f, -1276.6f, 110.916f, ZONE_FLINTI },
	{ -399.633f, -1075.52f, -1.489f, -319.033f, -977.516f, 198.511f, ZONE_BEACO },
	{ -1132.82f, -787.391f, 0.0f, -956.476f, -768.027f, 200.0f, ZONE_EBAY },
	/* end FLINTC region zones */
	/* start WHET region zones */
	{ -2324.94f, -2584.29f, 0.0f, -1964.22f, -2212.11f, 200.0f, ZONE_ANGPI },
	{ -1820.64f, -2643.68f, 0.0f, -1226.78f, -1771.66f, 200.0f, ZONE_CREEK },
	{ -2030.12f, -2174.89f, 0.0f, -1820.64f, -1771.66f, 200.0f, ZONE_CREEK },
	{ -2178.69f, -1250.97f, 0.0f, -1794.92f, -1115.58f, 200.0f, ZONE_SILLY },
	{ -2178.69f, -1771.66f, -47.916f, -1936.12f, -1250.97f, 576.08f, ZONE_MTCHI },
	{ -2994.49f, -2189.91f, -47.916f, -2178.69f, -1115.58f, 576.08f, ZONE_MTCHI },
	{ -2178.69f, -2189.91f, -47.916f, -2030.12f, -1771.66f, 576.08f, ZONE_MTCHI },
	{ -1632.83f, -2263.44f, 0.0f, -1601.33f, -2231.79f, 200.0f, ZONE_SHACA },
	/* end WHET region zones */
	/* start OCTA region zones */
	{ 12955.6768f, 6255.6094f, 0.0f, 13374.9365f, 7089.7993f, 200.0f, ZONE_OCTA_AIRPORT },
	{ 12867.0859f, 6754.1587f, 0.0f, 12955.6768f, 7089.7993f, 200.0f, ZONE_OCTA_AIRPORT },
	{ 12494.5635f, 6255.6094f, 0.0f, 12955.6768f, 6430.0532f, 200.0f, ZONE_OCTA_AIRPORT },
	{ 12886.6074f, 6430.0532f, 0.0f, 12955.6768f, 6641.6362f, 200.0f, ZONE_OCTA_AIRPORT },
	{ 12835.2139f, 6641.6362f, 0.0f, 12919.4531f, 6754.1587f, 200.0f, ZONE_OCTA_RESIDENTIAL },
	{ 12615.1553f, 6582.3755f, 0.0f, 12835.1816f, 6753.7251f, 200.0f, ZONE_OCTA_RESIDENTIAL },
	{ 12515.4824f, 6582.3755f, 0.0f, 12615.1553f, 6753.7251f, 200.0f, ZONE_OCTA_RESIDENTIAL },
	{ 12425.3369f, 6561.1528f, 0.0f, 12515.4824f, 6697.5884f, 200.0f, ZONE_OCTA_RESIDENTIAL },
	{ 12417.8926f, 6697.5884f, 0.0f, 12615.1553f, 6926.7129f, 200.0f, ZONE_OCTA_MOUNTAIN },
	{ 12292.7236f, 6486.2881f, 0.0f, 12425.3369f, 6697.5884f, 200.0f, ZONE_OCTA_BEACH },
	{ 12425.3369f, 6486.2881f, 0.0f, 12544.3184f, 6561.1528f, 200.0f, ZONE_OCTA_BEACH },
	{ 12515.4824f, 6536.6035f, 0.0f, 12886.6074f, 6582.3755f, 200.0f, ZONE_OCTA_BEACH },
	/* end OCTA region zones */
	/* start CATA region zones */
	{ -34629.93f, -1822.11f, 0.0f, -33286.82f, -848.27f, 200.0f, ZONE_CATA_CITY },
	/* end CATA region zones */
};

static int regioncount = sizeof(regions)/sizeof(regions[0]);
static int zonecount = sizeof(zones)/sizeof(zones[0]);
