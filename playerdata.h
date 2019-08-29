
/* vim: set filetype=c ts=8 noexpandtab: */

void pdata_init();

struct playerdata {
	char ip[16];
	char name[MAX_PLAYER_NAME + 1];
	char normname[MAX_PLAYER_NAME + 1];
	char namelen;
	int userid, groups;
};

extern struct playerdata *pdata[MAX_PLAYERS];

extern void useridornull(int playerid, char *storage);
