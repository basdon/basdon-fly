
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"
#include <string.h>

char *prefs_append_pref(char *buf, char *description, const int state)
{
	while ((*(buf++) = *(description++)));
	*(--buf) = ':';
	buf++;
	*(buf++) = '\t';
	if (state) {
		strcpy(buf, "{00f600}ENABLED");
		buf += 15;
	} else {
		strcpy(buf, "{f60000}DISABLED");
		buf += 16;
	}
	*(buf++) = '\n';
	return buf;
}

/* native Prefs_DoActionForRow(listitem, &playerprefs) */
cell AMX_NATIVE_CALL Prefs_DoActionForRow(AMX *amx, cell *params)
{
	const int listitem = params[1];
	cell *addr;

	amx_GetAddr(amx, params[2], &addr);
	/* must be same order as in Prefs_FormatDialog */
	switch (listitem) {
	case 0: *addr ^= PREF_CONSTANT_WORK; return 1;
	case 1: *addr ^= PREF_ENABLE_PM; return 1;
	case 2: *addr ^= PREF_SHOW_MISSION_MSGS; return 1;
	}
	return 0;
}

/* native Prefs_FormatDialog(playerprefs, buf[]) */
cell AMX_NATIVE_CALL Prefs_FormatDialog(AMX *amx, cell *params)
{
	const int playerprefs = params[1];
	cell *addr;
	char buf[255], *bp = buf;

	/* must be same order as in Prefs_DoActionForRow */
	bp = prefs_append_pref(bp, "Constant Work", playerprefs & PREF_CONSTANT_WORK);
	bp = prefs_append_pref(bp, "Accepting PMs", playerprefs & PREF_ENABLE_PM);
	bp = prefs_append_pref(bp, "Mission Messages", playerprefs & PREF_SHOW_MISSION_MSGS);
	*(--bp) = 0;

	amx_GetAddr(amx, params[2], &addr);
	amx_SetUString(addr, buf, sizeof(buf));
	return 1;
}
