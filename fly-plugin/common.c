char kick_update_delay[MAX_PLAYERS];

void atoc(cell *dst, char *src, int max)
{
	TRACE;
	while ((--max || (*dst = 0)) && (*dst++ = (cell) *src++));
}

void ctoa(char *dst, cell *src, int max)
{
	TRACE;
	while ((--max || (*dst = 0)) && (*dst++ = (char) *src++));
}

void atoci(cell *dstsrc, int len)
{
	TRACE;
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
	TRACE;
	cell *src;

	src = (cell*) dstsrc;
	while ((*dstsrc++ = (char) *src++));
}

static
int stricmp(char *a, char *b)
{
	TRACE;
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
	TRACE;
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
	TRACE;
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

static
int hexdigit(char c)
{
	TRACE;
	if ('0' <= c && c <= '9') return c - '0';
	if ('A' <= c && c <= 'F') return c - 'A' + 10;
	if ('a' <= c && c <= 'f') return c - 'a' + 10;
	return 0;
}

static
unsigned int hexnum(char *c, int len)
{
	TRACE;
	unsigned int res = 0;

	while (len-- > 0) {
		res <<= 4;
		res |= hexdigit(*c);
		c++;
	}
	return res;
}

void common_tp_player(int playerid, struct vec4 pos)
{
	TRACE;
	natives_SetPlayerPos(playerid, pos.coords);
	SetPlayerFacingAngle(playerid, pos.r);
	SetCameraBehindPlayer(playerid);
}

float common_dist_sq(struct vec3 a, struct vec3 b)
{
	TRACE;
	float dx, dy, dz;
	dx = a.x - b.x;
	dy = a.y - b.y;
	dz = a.z - b.z;
	return dx * dx + dy * dy + dz * dz;
}

float common_xy_dist_sq(struct vec3 a, struct vec3 b)
{
	TRACE;
	float dx, dy;
	dx = a.x - b.x;
	dy = a.y - b.y;
	return dx * dx + dy * dy;
}

int common_SetVehiclePos(int vehicleid, struct vec3 *pos)
{
	TRACE;
	NC_PARS(4);
	nc_params[1] = vehicleid;
	memcpy(nc_params + 2, pos, sizeof(struct vec3));
	return NC(n_SetVehiclePos);
}

static
void common_common_varargs_cb_call(cell p1, cell p2, cb_t callback, void *data)
{
	TRACE;
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
	TRACE;
	common_common_varargs_cb_call(pw, hash, callback, data);
	NC(n_bcrypt_check);
}

void common_bcrypt_hash(cell pw, cb_t callback, void *data)
{
	TRACE;
	common_common_varargs_cb_call(pw, 12 /*cost*/, callback, data);
	NC(n_bcrypt_hash);
}

void common_mysql_escape_string(char *data, char *dest, int maxlen)
{
	TRACE;
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
	TRACE;
	char dst[1000];
	ctoa(dst, (cell*) ((int) &fakeamx_data + (int) cella), 1000);
	logprintf("%s", dst);
	_NC_mysql_tquery_nocb(cella);
}
#endif

void common_mysql_tquery(char *query, cb_t callback, void *data)
{
	TRACE;
#ifdef PRINTQUERIES
	logprintf("%s", query);
#endif
	atoc(buf4096, query, 2000);
	common_common_varargs_cb_call(1 /*handle*/, buf4096a, callback, data);
	NC(n_mysql_tquery);
}

float common_vectorsize_sq(struct vec3 vec)
{
	TRACE;
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}
