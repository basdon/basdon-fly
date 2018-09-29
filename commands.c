
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"

cell AMX_NATIVE_CALL CommandHash(AMX *amx, cell *params)
{
	char buf[144];
	int val, pos = 0, result = 0;

	cell *inaddr = NULL;
	amx_GetAddr(amx, params[1], &inaddr);
	amx_GetUString(buf, inaddr, 144);

	/* same as hashCode in Java (but case insensitive) */
	while (buf[pos] != 0 && buf[pos] > ' ') {
		val = buf[pos++];
		if ('A' <= val && val <= 'Z') {
			val |= 0x20;	
		}
		result = 31 * result + val;
	}

	return result;
}

