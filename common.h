
/* vim: set filetype=c ts=8 noexpandtab: */

#include <stdlib.h>
#include <stdio.h>

#include "vendor/SDK/amx/amx.h"
#include "vendor/SDK/plugincommon.h"
#include "sharedsymbols.h"

typedef void (*logprintf_t)(char* format, ...);

extern logprintf_t logprintf;

#define amx_GetUString(dest, source, size) amx_GetString(dest, source, 0, size)
#define amx_SetUString(dest, source, size) amx_SetString(dest, source, 0, 0, size)

#define M_PI 3.14159265358979323846f
#define M_PI2 1.57079632679489661923f

#define CLAMP(X,L,U) ((X < L) ? L : ((X > U) ? U : X))

struct vec3 {
	float x, y, z;
};

