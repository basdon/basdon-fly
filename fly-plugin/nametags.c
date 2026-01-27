#define NAMETAGS_UPDATE_INTERVAL 500
#define NAMETAGS_DEFAULT_MAX_DISTANCE 5000

/**[forplayerid][playerid]*/
static char nametag_created[MAX_PLAYERS][MAX_PLAYERS];

/** Nametags cycle between showing 2 different values: distance to player + altitude, vehicle model */
static char nametags_show_distance;

static
void nametags_update_for_player(int forplayerid)
{
	TRACE;
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
				if (nametags_show_distance) {
					sprintf(
						buf,
						"%s (%d)\n%.0fm (%.0fft)",
						pdata[playerid]->name,
						playerid,
						dist,
						playerpos->z
					);
				} else {
					sprintf(
						buf,
						"%s (%d)\n%s",
						pdata[playerid]->name,
						playerid,
						vehnames[vehiclemodel - VEHICLE_MODEL_MIN]
					);
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

/**
 * Called by timer, so return value is the delay until the next call.
 *
 * Idea here is that for every call, this will update all nametags for one player (if they're not afk).
 * So to reach the goal of updating nametags for every player in the target time, the return value should
 * be the target time divided by the amount of players.
 */
static
int nametags_update(void *data)
{
	TRACE;
	static int next_updating_playeridx = 0;
	/** Since nametags are updated every 500ms, this keeps count of doing 4 updates before changing what value to show. */
	static char nametags_updateindex = 0;

	int forplayerid;

	if (playercount < 2) {
		return NAMETAGS_UPDATE_INTERVAL;
	}

	if (++next_updating_playeridx >= playercount) {
		next_updating_playeridx = 0;
		nametags_updateindex++;
		nametags_show_distance = (nametags_show_distance + ((nametags_updateindex & 4) >> 2)) & 1;
		nametags_updateindex &= 3;
	}

	forplayerid = players[next_updating_playeridx];
	if (!isafk[forplayerid]) {
		nametags_update_for_player(forplayerid);
	}

	return NAMETAGS_UPDATE_INTERVAL / playercount;
}

static
void nametags_on_player_connect(int forplayerid)
{
	TRACE;
	memset(nametag_created[forplayerid], 0, sizeof(nametag_created[forplayerid]));
	nametags_max_distance[forplayerid] = NAMETAGS_DEFAULT_MAX_DISTANCE;
}

static
void nametags_on_player_disconnect(int playerid)
{
	TRACE;
	int forplayerid, i;

	for (i = 0; i < playercount; i++) {
		forplayerid = players[i];
		if (nametag_created[forplayerid][playerid]) {
			nametag_created[forplayerid][playerid] = 0;
			Delete3DTextLabel(forplayerid, NAMETAG_TEXTLABEL_ID_BASE + playerid);
		}
	}
}
