
/* vim: set filetype=c ts=8 noexpandtab: */

/*prototype for commonly used struct*/
struct dbvehicle;

/**
Convert char string to cell string.

@param max must be gt 0
*/
void atoc(cell *dst, char *src, int max);
/**
Convert cell string to char string.

@param max must be gt 0
*/
void ctoa(char *dst, cell *src, int max);
/**
Convert char string to cell string, in place.

@param len length of the string, excluding zero term
*/
void atoci(cell *dstsrc, int len);
/**
Convert a cell string to a char string, in place.

Relies on good faith that the cell string is zero terminated.
*/
void ctoai(char *dstsrc);

#define CLAMP(X,L,U) ((X < L) ? L : ((X > U) ? U : X))
#define Q_USE_EQ_INSTEAD(X) #X
#define EQ(X) Q_USE_EQ_INSTEAD(X)

#define MK_PLAYER_CC(PLAYERID) ((_cc[PLAYERID] & 0xFFFF) | (PLAYERID << 16))
#define V_MK_PLAYER_CC(PLAYERID) ((void*) MK_PLAYER_CC(PLAYERID))
#define PLAYER_CC_GETID(VALUE) (((unsigned int) VALUE >> 16) & 0xFFFF)
#define PLAYER_CC_CHECK(VALUE,PLAYERID) (_cc[PLAYERID] == (unsigned short) ((unsigned int) VALUE & 0xFFFF))
#define PLAYER_CC_GET_CHECK_RETURN(VALUE,PLAYERID) \
	PLAYERID=PLAYER_CC_GETID(VALUE);\
	if(!PLAYER_CC_CHECK(VALUE,PLAYERID))return;

#define ECOL_INFO "{3498db}"
#define ECOL_WARN "{e84c3d}"
#define ECOL_SUCC "{2cc36b}"
#define ECOL_DIALOG_CAPTION "{d2d2d2}"
#define ECOL_DIALOG_TEXT "{a9c4e4}"
#define ECOL_DIALOG_BUTTON "{c8c8c8}"
#define ECOL_MISSION "{ff9900}"
#define ECOL_SAMP_GREY "{a9c4e4}"

#define WARN "! "
#define INFO "* "
#define SUCC "" /*This used to be a plus symbol, but that might confuse people.*/

/* XXX: don't use white as a color for system messages. abusive users can take advantage of
        chatmessage wrapping and timestamps not showing by default and naivity to trick users
        into believing their wrapped message (in white color) is a system message.
        That's also why I should revert disabling the text filter that removed embedded
        colors from user chat messages*/

#define COL_SAMP_GREEN 0x88aa62ff /*as in username error message*/
#define COL_SAMP_GREY 0xa9c4e4ff /*as in 'Connected to..'*/
#define COL_SAMP_BLUE 0x2587ceff /*as in '* Admin:'*/
#define COL_SAMP_LIGHTGREEN 0xb9c9bfff /*as in the hostname color in 'Connected to..'*/
#define COL_INFO 0x3498dbff
#define COL_WARN 0xe84c3dff
#define COL_SUCC 0x2cc36bff
#define COL_INFO_GENERIC 0xcee21bff
#define COL_INFO_LIGHT 0x33ccffff
#define COL_INFO_BROWN 0xcc9966ff
#define COL_PRIVMSG 0xffff00ff
#define COL_PRIVMSG_HINT 0xafafafff
#define COL_MISSION 0xff9900ff
#define COL_MISSION_ALT 0xffbb55ff
#define COL_JOIN 0x1b8ae4ff
#define COL_QUIT 0x1674Bcff
#define COL_METAR COL_INFO_LIGHT
#define COL_IRC COL_INFO_GENERIC
#define COL_IRC_MODE 0x22ffffff
#define COL_IRC_ACTION 0xff66ffff
#define COL_RADIO 0xccccccff

#define COL_CHANGELOG COL_INFO_BROWN

/*A panel is a menu (like created with SCM OP 08D4). These are all ABGR.*/
#define COL_PANEL_BACKGROUND 0xBE000000
#define COL_PANEL_TITLE 0xFFE1E1E1
#define COL_PANEL_ROW_HEADER 0xFFC4C4C4
#define COL_PANEL_ROW_NORMAL 0xFF5D4F41
#define COL_PANEL_ROW_SELECTED 0xFFD1B196
#define COL_PANEL_ROW_DISABLED 0xFF838383
/*Inactive is when this panel is not currently interactive.*/
#define COL_PANEL_ROW_INACTIVE 0xFF6C4B31

#define GROUP_BANNED (1)
#define GROUP_GUEST (2)
#define GROUP_MEMBER (4)
#define GROUP_ADMIN (268435456)
#define GROUP_OWNER (1073741824)

#define GROUPS_ALL \
	(GROUP_BANNED|GROUP_GUEST|GROUP_MEMBER|\
	GROUP_ADMIN|GROUP_OWNER)
#define GROUPS_ADMIN (GROUP_ADMIN|GROUP_OWNER)
#define GROUPS_NOTBANNED (GROUPS_ALL&~GROUPS_BANNED)
#define GROUPS_REGISTERED (GROUPS_ALL&~GROUP_GUEST)

#define GROUPS_ISADMIN(X) ((X) >= GROUP_ADMIN)

/*in cells*/
#define STACK_HEAP_SIZE 1024

struct FAKEAMX {
	char _cod[3152];
	union {
		char _dat;
		cell emptystring;
	};
	cell underscorestring[2];
	union {
		cell ascell[32];
		char aschr[32 * sizeof(cell)];
		float asflt[32];
	} a32;
	union {
		cell ascell[32];
		char aschr[32 * sizeof(cell)];
		float asflt[32];
	} a32_1;
	union {
		cell ascell[64];
		char aschr[64 * sizeof(cell)];
		float asflt[64];
	} a64;
	union {
		cell ascell[144];
		char aschr[144 * sizeof(cell)];
		float asflt[144];
	} a144;
	union {
		cell ascell[4096];
		char aschr[4096 * sizeof(cell)];
		float asflt[4096];
		struct {
			char _pad[4096 * 3];
			char last4096[4096];
		} splitted;
	} a4096;
	cell _stackheap[STACK_HEAP_SIZE];
};

#define basea ((int) &fakeamx._dat)
#define emptystringa ((int) &fakeamx.emptystring - basea)
#define underscorestringa ((int) &fakeamx.underscorestring - basea)
#define buf32a ((int) &fakeamx.a32 - basea)
#define buf32_1a ((int) &fakeamx.a32_1 - basea)
#define buf64a ((int) &fakeamx.a64 - basea)
#define buf144a ((int) &fakeamx.a144 - basea)
#define buf4096a ((int) &fakeamx.a4096 - basea)
#define buf32 (fakeamx.a32.ascell)
#define buf32_1 (fakeamx.a32_1.ascell)
#define buf64 (fakeamx.a64.ascell)
#define buf144 (fakeamx.a144.ascell)
#define buf4096 (fakeamx.a4096.ascell)
#define cbuf32 fakeamx.a32.aschr
#define cbuf32_1 fakeamx.a32_1.aschr
#define cbuf64 fakeamx.a64.aschr
#define cbuf144 fakeamx.a144.aschr
#define cbuf4096 fakeamx.a4096.aschr
#define cbuf4096_ fakeamx.a4096.splitted.last4096
#define fbuf32 fakeamx.a32.asflt
#define fbuf32_1 fakeamx.a32_1.asflt
#define fbuf64 fakeamx.a64.asflt
#define fbuf144 fakeamx.a144.asflt
#define fbuf4096 fakeamx.a4096.asflt

/**
Should be decremented in OnPlayerUpdate and player kicked when it reaches 0.
*/
extern char kick_update_delay[MAX_PLAYERS];
/**
Holds spawned status of players.
*/
extern int spawned[MAX_PLAYERS];
/**
Holds class the players are playing as.
*/
extern int playerclass[MAX_PLAYERS];
/**
Current in-game hour.
*/
extern int time_h;
/**
Current in-game minute.
*/
extern int time_m;

/**
Teleport the player to a coordinate, and set facing angle and reset camera.
*/
void common_tp_player(int playerid, struct vec4 pos);
/**
@return squared xyz distance between given points
*/
float common_dist_sq(struct vec3 a, struct vec3 b);
/**
@return squared xy distance between given points
*/
float common_xy_dist_sq(struct vec3 a, struct vec3 b);
/**
Convenience method to set a vehicle's pos to given vec3 struct.

Uses buf32 buf64 buf144.
*/
int common_SetVehiclePos(int vehicleid, struct vec3 *pos);
/**
Check a string agains its hash.

Uses the last 8 components of buf4096 for callback stuff.

@param callback function to call when the checking is done
@param data usually a pointer to allocated memory that should be freed in the cb
*/
void common_bcrypt_check(cell pw, cell hash, cb_t callback, void *data);
/**
Hash a string.

Uses the last 8 components of buf4096 for callback stuff.

@param callback function to call when the hasing is done
@param data usually a pointer to allocated memory that should be freed in the cb
*/
void common_bcrypt_hash(cell pw, cb_t callback, void *data);
/**
Uses the mysql plugin escape string routine.

Uses buf4096
TODO: bad parameter order, i expect dest first then src, is this the only odd one out?
*/
void common_mysql_escape_string(char *data, char *dest, int maxlen);
/**
Calls mysql_tquery with a callback and data to pass.

Uses buf4096 for query. Also uses the last 8 components of buf4096.

@param callback function to call when the query has been executed
@param data usually a pointer to allocated memory that should be freed in the cb
*/
void common_mysql_tquery(char *query, cb_t callback, void *data);
float common_vectorsize_sq(struct vec3 vec);
