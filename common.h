
/* vim: set filetype=c ts=8 noexpandtab: */

#include <stdlib.h>
#include <stdio.h>

#include "vendor/SDK/amx/amx.h"
#include "vendor/SDK/plugincommon.h"

typedef void (*logprintf_t)(char* format, ...);

extern logprintf_t logprintf;

#define MAX_PLAYERS (50)

#define amx_GetUString(dest, source, size) amx_GetString(dest, source, 0, size)
#define amx_SetUString(dest, source, size) amx_SetString(dest, source, 0, 0, size)

