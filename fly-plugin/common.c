char kick_update_delay[MAX_PLAYERS];

void atoc(cell *dst, char *src, int max)
{
	while ((--max || (*dst = 0)) && (*dst++ = (cell) *src++));
}

void ctoa(char *dst, cell *src, int max)
{
	while ((--max || (*dst = 0)) && (*dst++ = (char) *src++));
}

void atoci(cell *dstsrc, int len)
{
	char *src;

	src = ((char*) dstsrc) + len;
	dstsrc += len;
	*dstsrc = 0;
a:
	if (len) {
		*--dstsrc = (cell) *--src;
		len--;
		goto a;
	}
}

void ctoai(char *dstsrc)
{
	cell *src;

	src = (cell*) dstsrc;
	while ((*dstsrc++ = (char) *src++));
}

static
int stricmp(char *a, char *b)
{
	register int x;
	register char _a, _b;

	for (;;) {
		_a = *a;
		if ('A' <= _a && _a <= 'Z') {
			_a |= 0x20;
		}
		_b = *b;
		if ('A' <= _b && _b <= 'Z') {
			_b |= 0x20;
		}
		x = _a - _b;
		if (x || !_a) {
			return x;
		}
		a++;
		b++;
	}
}

static
int strincmp(char *a, char *b, int n)
{
	register int x;
	register char _a, _b;

	while (n--) {
		_a = *a;
		if ('A' <= _a && _a <= 'Z') {
			_a |= 0x20;
		}
		_b = *b;
		if ('A' <= _b && _b <= 'Z') {
			_b |= 0x20;
		}
		x = _a - _b;
		if (x || !_a) {
			return x;
		}
		a++;
		b++;
	}
	return 0;
}

/*
Hashes text until a character <= space is found (case-insensitive).
*/
static
int strhashcode(char *text)
{
	register int val;
	int result = 0;

	/* same as hashCode in Java (but case insensitive) */
	while ((val = *text) > ' ') {
		text++;
		if ('A' <= val && val <= 'Z') {
			val |= 0x20;
		}
		result = 31 * result + val;
	}
	return result;
}

void common_tp_player(int playerid, struct vec4 pos)
{
	natives_SetPlayerPos(playerid, pos.coords);
	SetPlayerFacingAngle(playerid, pos.r);
	SetCameraBehindPlayer(playerid);
}

float common_dist_sq(struct vec3 a, struct vec3 b)
{
	float dx, dy, dz;
	dx = a.x - b.x;
	dy = a.y - b.y;
	dz = a.z - b.z;
	return dx * dx + dy * dy + dz * dz;
}

float common_xy_dist_sq(struct vec3 a, struct vec3 b)
{
	float dx, dy;
	dx = a.x - b.x;
	dy = a.y - b.y;
	return dx * dx + dy * dy;
}

int common_GetVehicleDamageStatus(int vehicleid, struct VEHICLEDAMAGE *d)
{
	int res;
	NC_PARS(5);
	nc_params[1] = vehicleid;
	nc_params[2] = buf32a;
	nc_params[3] = buf32a + 0x4;
	nc_params[4] = buf32a + 0x8;
	nc_params[5] = buf32a + 0xC;
	res = NC(n_GetVehicleDamageStatus);
	memcpy(d, buf32, sizeof(struct VEHICLEDAMAGE));
	return res;
}

int common_SetVehiclePos(int vehicleid, struct vec3 *pos)
{
	NC_PARS(4);
	nc_params[1] = vehicleid;
	memcpy(nc_params + 2, pos, sizeof(struct vec3));
	return NC(n_SetVehiclePos);
}

int common_UpdateVehicleDamageStatus(int vehicleid, struct VEHICLEDAMAGE *d)
{
	NC_PARS(5);
	nc_params[1] = vehicleid;
	memcpy(nc_params + 2, d, sizeof(struct VEHICLEDAMAGE));
	return NC(n_UpdateVehicleDamageStatus);
}

static
void common_common_varargs_cb_call(cell p1, cell p2, cb_t callback, void *data)
{
	buf4096[4088] = 'M';
	buf4096[4089] = 'M';
	buf4096[4090] = 0;
	buf4096[4091] = 'i';
	buf4096[4092] = 'i';
	buf4096[4093] = 0;
	buf4096[4094] = (cell) callback;
	buf4096[4095] = (cell) data;
	nc_params[0] = 6 << 2; /*varargs, don't remove*/
	nc_params[1] = p1;
	nc_params[2] = p2;
	nc_params[3] = buf4096a + 4088 * 4;
	nc_params[4] = buf4096a + 4091 * 4;
	nc_params[5] = buf4096a + 4094 * 4;
	nc_params[6] = buf4096a + 4095 * 4;
}

void common_bcrypt_check(cell pw, cell hash, cb_t callback, void *data)
{
	common_common_varargs_cb_call(pw, hash, callback, data);
	NC(n_bcrypt_check);
}

void common_bcrypt_hash(cell pw, cb_t callback, void *data)
{
	common_common_varargs_cb_call(pw, 12 /*cost*/, callback, data);
	NC(n_bcrypt_hash);
}

void common_mysql_escape_string(char *data, char *dest, int maxlen)
{
	atoc(buf4096, data, 4096);
	NC_PARS(4);
	nc_params[1] = nc_params[2] = buf4096a;
	nc_params[3] = 1; /*connectionhandle*/
	nc_params[4] = maxlen;
	NC(n_mysql_escape_string);
	ctoa(dest, buf4096, maxlen);
	dest[maxlen - 1] = 0;
}

#ifdef PRINTQUERIES
void DEBUG_NC_mysql_tquery_nocb(cell cella)
{
	char dst[1000];
	ctoa(dst, (cell*) ((int) &fakeamx_data + (int) cella), 1000);
	logprintf("%s", dst);
	_NC_mysql_tquery_nocb(cella);
}
#endif

void common_mysql_tquery(char *query, cb_t callback, void *data)
{
#ifdef PRINTQUERIES
	logprintf("%s", query);
#endif
	atoc(buf4096, query, 2000);
	common_common_varargs_cb_call(1 /*handle*/, buf4096a, callback, data);
	NC(n_mysql_tquery);
}

float common_vectorsize_sq(struct vec3 vec)
{
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}
