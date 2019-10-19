
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"

int p_OnPlayerCommandTextHash;
int p_REMOVEME_isafk;

cell pc_result;

/* finds exported symbols */
int publics_find(AMX *amx)
{
	struct PUBLIC {
		char *name;
		int *var;
	};
	struct PUBLIC publics[] = {
		{ "OnPlayerCommandTextHash", &p_OnPlayerCommandTextHash },
		{ "REMOVEME_isafk", &p_REMOVEME_isafk },
	};
	struct PUBLIC *p = publics + sizeof(publics)/sizeof(struct PUBLIC);

	while (p-- != publics) {
		if (amx_FindPublic(amx, p->name, p->var)) {
			logprintf("ERR: no %s public", p->name);
			return 0;
		}
	}

	return 1;
}
