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

void common_tp_player(int playerid, struct vec4 pos)
{
	natives_SetPlayerPos(playerid, pos.coords);
	NC_PARS(2);
	nc_params[1] = playerid;
	nc_paramf[2] = pos.r;
	NC(n_SetPlayerFacingAngle);
	NC_PARS(1);
	NC(n_SetCameraBehindPlayer);
}

int common_find_player_in_vehicle_seat(int vehicleid, int seat)
{
	int n, playerid;

	for (n = playercount; n; ) {
		playerid = players[--n];
		if (GetPlayerVehicleID(playerid) == vehicleid && GetPlayerVehicleSeat(playerid) == seat) {
			return playerid;
		}
	}
	return INVALID_PLAYER_ID;
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

void common_GetPlayerKeys(int playerid, struct PLAYERKEYS *keys)
{
	NC_PARS(4);
	nc_params[1] = playerid;
	nc_params[2] = buf32a;
	nc_params[3] = buf32a + 4;
	nc_params[4] = buf32a + 8;
	NC(n_GetPlayerKeys);
	memcpy(keys, buf32, sizeof(struct PLAYERKEYS));
}

int common_GetPlayerPos(int playerid, struct vec3 *pos)
{
	int res;
	res = NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
	pos->x = *fbuf32;
	pos->y = *fbuf64;
	pos->z = *fbuf144;
	return res;
}

int common_GetPlayerPosRot(int playerid, struct vec4 *pos)
{
	int res;
	res = NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
	pos->coords.x = *fbuf32;
	pos->coords.y = *fbuf64;
	pos->coords.z = *fbuf144;
	NC_PARS(2);
	res &= NC(n_GetPlayerFacingAngle);
	pos->r = *fbuf32;
	return res;
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

void common_on_vehicle_respawn_or_destroy(int vehicleid, struct dbvehicle *veh)
{
	missions_on_vehicle_destroyed_or_respawned(veh);
	nav_reset_for_vehicle(vehicleid);
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
