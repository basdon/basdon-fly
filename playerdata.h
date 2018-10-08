
/* vim: set filetype=c ts=8 noexpandtab: */

void pdata_init();

struct playerdata {
	char ip[16];
	char name[MAX_PLAYER_NAME + 1];
	short namelen;
};

extern struct playerdata *pdata[MAX_PLAYERS];

