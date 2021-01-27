#define NAMETAGS_UPDATE_INTERVAL 1000
#define NAMETAGS_DEFAULT_MAX_DISTANCE 5000

static char nametag_created[MAX_PLAYERS][MAX_PLAYERS];

static int nametags_next_updating_playeridx;
/*Since nametags update every second, but cycle should be every other second,
duplicating the states is an easy stupid way to do that.*/
#define NAMETAG_CYCLE_DISTANCE_1 0
#define NAMETAG_CYCLE_DISTANCE_2 1
#define NAMETAG_CYCLE_ALT_1 2
#define NAMETAG_CYCLE_ALT_2 3
#define NAMETAG_CYCLE_VEHICLE_1 4
#define NAMETAG_CYCLE_VEHICLE_2 5
#define NAMETAG_NUM_CYCLES 6
static char nametags_current_cycle;

static
void nametags_update_for_player(int forplayerid)
{
	struct SampPlayer *forplayer;
	struct vec3 *playerpos, forplayerpos, d;
	int playerid, i, bitbuf_bitlength, current_render_dist, vehiclemodel;
	float dist, lerp_dist, max_dist_sq;
	char buf[128];
	char bitbuf[150];

	forplayer = player[forplayerid];
	forplayerpos = forplayer->pos;

	if (nametags_max_distance[forplayerid] > 50000) {
		max_dist_sq = float_pinf;
	} else {
		max_dist_sq = nametags_max_distance[forplayerid];
		max_dist_sq *= max_dist_sq;
	}
	
	current_render_dist = timecyc_get_current_render_distance() - 80 /*leeway.. this should be big enough..*/;
	if (current_render_dist < 400) {
		/*Too close is really bad since the warping gets too much...*/
		current_render_dist = 400;
	}

	for (i = 0; i < playercount; i++) {
		playerid = players[i];
		if (forplayerid == playerid) {
			continue;
		}
		if (!spawned[forplayerid] || !spawned[playerid]) {
			goto hide;
		}
		playerpos = &player[playerid]->pos;

		d.x = playerpos->x - forplayerpos.x;
		d.y = playerpos->y - forplayerpos.y;

		dist = d.x * d.x + d.y * d.y;

		if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
			vehiclemodel = GetVehicleModel(GetPlayerVehicleID(playerid));
			if (game_is_air_vehicle(vehiclemodel)) {
				if (dist > max_dist_sq) {
					goto hide;
				}
				d.z = playerpos->z - forplayerpos.z;
				dist += d.z * d.z;
				dist = (float) sqrt(dist);
				switch (nametags_current_cycle) {
				case NAMETAG_CYCLE_DISTANCE_1:
				case NAMETAG_CYCLE_DISTANCE_2:
					sprintf(buf, "%s (%d)\n%.0fm", pdata[playerid]->name, playerid, dist);
					break;
				case NAMETAG_CYCLE_ALT_1:
				case NAMETAG_CYCLE_ALT_2:
					sprintf(buf, "%s (%d)\n%.0fft", pdata[playerid]->name, playerid, playerpos->z);
					break;
				case NAMETAG_CYCLE_VEHICLE_1:
				case NAMETAG_CYCLE_VEHICLE_2:
					sprintf(buf, "%s (%d)\n%s",
						pdata[playerid]->name,
						playerid,
						vehnames[vehiclemodel - VEHICLE_MODEL_MIN]);
					break;
				}
				bitbuf_bitlength = EncodeString(bitbuf, buf, sizeof(bitbuf));
				if (forplayer->playerStreamedIn[playerid] && dist < current_render_dist) {
					Create3DTextLabel(
						forplayerid, NAMETAG_TEXTLABEL_ID_BASE + playerid, player[playerid]->color, vec3_zero,
						nametags_max_distance[forplayerid], 1, playerid, INVALID_VEHICLE_ID, bitbuf, bitbuf_bitlength
					);
				} else {
					/*So.. labels don't render if they're out of render distance (which means that's affected by weather)...
					Lerping position then to try to make sure they will render. I can't think of a better solution...*/
					if (current_render_dist > dist) {
						d = *playerpos;
					} else {
						lerp_dist = (float) current_render_dist / dist;
						d.x = forplayerpos.x + d.x * lerp_dist;
						d.y = forplayerpos.y + d.y * lerp_dist;
						d.z = forplayerpos.z + d.z * lerp_dist;
					}
					Create3DTextLabel(
						forplayerid, NAMETAG_TEXTLABEL_ID_BASE + playerid, player[playerid]->color, &d,
						float_pinf, 1, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, bitbuf, bitbuf_bitlength
					);
				}
				nametag_created[forplayerid][playerid] = 1;
				continue;
			}
		}

		if (forplayer->playerStreamedIn[playerid] && dist < max_dist_sq) {
			sprintf(buf, "%s (%d)", pdata[playerid]->name, playerid);
			bitbuf_bitlength = EncodeString(bitbuf, buf, sizeof(bitbuf));
			Create3DTextLabel(
				forplayerid, NAMETAG_TEXTLABEL_ID_BASE + playerid, player[playerid]->color, vec3_zero,
				nametags_max_distance[forplayerid], 1, playerid, INVALID_VEHICLE_ID, bitbuf, bitbuf_bitlength
			);
			nametag_created[forplayerid][playerid] = 1;
		} else {
hide:
			if (nametag_created[forplayerid][playerid]) {
				nametag_created[forplayerid][playerid] = 0;
				Delete3DTextLabel(forplayerid, NAMETAG_TEXTLABEL_ID_BASE + playerid);
			}
		}
	}
}

static
int nametags_update(void *data)
{
	int forplayerid;

	if (playercount < 2) {
		return NAMETAGS_UPDATE_INTERVAL;
	}

	nametags_next_updating_playeridx++;
	if (nametags_next_updating_playeridx >= playercount) {
		nametags_next_updating_playeridx = 0;
		nametags_current_cycle++;
		if (nametags_current_cycle >= NAMETAG_NUM_CYCLES) {
			nametags_current_cycle = 0;
		}
	}
	forplayerid = players[nametags_next_updating_playeridx];
	if (!isafk[forplayerid]) {
		nametags_update_for_player(forplayerid);
	}

	/*Calculate delay so that nametags for everyone will update about once a second.*/
	return NAMETAGS_UPDATE_INTERVAL / playercount;
}

static
void nametags_init()
{
	timer_set(NAMETAGS_UPDATE_INTERVAL, nametags_update, NULL);
}

static
void nametags_on_player_connect(int forplayerid)
{
	memset(nametag_created[forplayerid], 0, sizeof(nametag_created[forplayerid]));
	nametags_max_distance[forplayerid] = NAMETAGS_DEFAULT_MAX_DISTANCE;
}

static
void nametags_on_player_disconnect(int playerid)
{
	int forplayerid, i;

	for (i = 0; i < playercount; i++) {
		forplayerid = players[i];
		if (nametag_created[forplayerid][playerid]) {
			nametag_created[forplayerid][playerid] = 0;
			Delete3DTextLabel(forplayerid, NAMETAG_TEXTLABEL_ID_BASE + playerid);
		}
	}
}
