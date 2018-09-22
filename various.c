
/* vim: set filetype=c ts=8 noexpandtab: */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "common.h"

cell AMX_NATIVE_CALL Urlencode(AMX *amx, cell *params)
{
	int len = params[2];
	int len0 = len + 1;
	char *in;
	char *out;
	int i = 0, io = 0;

	cell *inaddr = NULL, *outaddr = NULL;
	amx_GetAddr(amx, params[1], &inaddr);
	amx_GetAddr(amx, params[3], &outaddr);

	in = (char *) malloc(len0 * sizeof(char));
	out = (char *) malloc((len * 3 + 1) * sizeof(char));
	amx_GetUString(in, inaddr, len0);
	while (i < len) {
		if (('a' <= in[i] && in[i] <= 'z') ||
			('A' <= in[i] && in[i] <= 'Z') ||
			('0' <= in[i] && in[i] <= '9'))
		{
			out[io++] = in[i++];
			continue;
		}

		sprintf(out + io, "%%%02X", in[i]);
		io += 3;
		if (in[i++] != 0) {
			continue;
		}

		if (i == len) {
			break;
		}
		/*
		amx_GetString seems to be doing a zero term check but we trust len
		so if there's a nul value in there, fetch the next part of the string
		*/
		amx_GetUString(in + i, inaddr + i, len0 - i);
	}
	out[io] = 0;
	amx_SetUString(outaddr, out, io + 1);

	free(in);
	free(out);
	return io;
}

