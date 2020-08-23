struct ZONE {
	float x1, y1, z1, x2, y2, z2;
	int id;
};

struct REGION {
	struct ZONE zone;
	int minzone, maxzone;
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
};

static int regioncount = sizeof(regions)/sizeof(regions[0]);
static int zonecount = sizeof(zones)/sizeof(zones[0]);
