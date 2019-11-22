
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "publics.h"

int p_OnPlayerCommandTextHash;
int p_REMOVEME_isplaying;

cell pc_result;

int publics_find()
{
	struct PUBLIC {
		char *name;
		int *var;
	};
	struct PUBLIC publics[] = {
#define P(X) {#X,&p_##X}
		P(OnPlayerCommandTextHash),
		P(REMOVEME_isplaying),
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
