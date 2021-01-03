#if NUM_KNEEBOARD_TEXTDRAWS != 3
#error
#endif
static struct TEXTDRAW td_kb_header = { "title", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_kb_info = { "info", 300, NULL };
static struct TEXTDRAW td_kb_distance = { "bg/info2", 300, NULL };

static int kneeboard_last_distance[MAX_PLAYERS];

static char kneeboard_is_shown[MAX_PLAYERS];

#define KNEEBOARD_SHOULD_SHOW(PLAYERID) (spawned[PLAYERID])

static
void kneeboard_init()
{
	textdraws_load_from_file("kneeboard", TEXTDRAW_KNEEBOARD_BASE, 3, &td_kb_distance, &td_kb_info, &td_kb_header);
}

static
void kneeboard_on_player_connect(int playerid)
{
	kneeboard_is_shown[playerid] = 0;
	kneeboard_last_distance[playerid] = -1;
}

/**
@return the length of the string that was put in out_buf, or 0 when no update is needed
*/
static
int kneeboard_format_distance(int playerid, struct vec3 *playerpos, char *out_buf)
{
	int dist;

	dist = missions_get_distance_to_next_cp(playerid, playerpos);
	if (dist > 1000) {
		dist /= 100;
		dist *= 100;
	}
	if (dist != kneeboard_last_distance[playerid]) {
		kneeboard_last_distance[playerid] = dist;
		if (dist == -1) {
			return sprintf(out_buf, "~n~~n~~n~~n~~n~");
		} else if (dist < 1000) {
			return sprintf(out_buf, "~n~~n~~n~~n~~n~~w~Distance: %dm~n~", dist);
		} else {
			return sprintf(out_buf, "~n~~n~~n~~n~~n~~w~Distance: %.1fKm~n~", (float) dist / 1000.0f);
		}
	}
	return 0;
}

static
void kneeboard_update_distance(int playerid, struct vec3 *playerpos)
{
	int len;

	if (kneeboard_is_shown[playerid]) {
		len = kneeboard_format_distance(playerid, playerpos, &rpcdata_freeform.byte[4]);
		if (len) {
			rpcdata_freeform.word[0] = td_kb_distance.rpcdata->textdrawid;
			rpcdata_freeform.word[1] = (short) len;
			bitstream_freeform.ptrData = &rpcdata_freeform;
			bitstream_freeform.numberOfBitsUsed = (2 + 2 + rpcdata_freeform.word[1]) * 8;
			SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream_freeform, playerid, 2);
		}
	}
}

static
void kneeboard_update_all(int playerid, struct vec3 *playerpos)
{
	if (!KNEEBOARD_SHOULD_SHOW(playerid)) {
		if (kneeboard_is_shown[playerid]) {
			textdraws_hide_consecutive(playerid, TEXTDRAW_KNEEBOARD_BASE, 3);
			kneeboard_is_shown[playerid] = 0;
		}
		return;
	}

	kneeboard_last_distance[playerid] = -2; /*To force update when calling kneeboard_format_distance below.*/

	if (kneeboard_is_shown[playerid]) {
		rpcdata_freeform.word[0] = td_kb_distance.rpcdata->textdrawid;
		rpcdata_freeform.word[1] = kneeboard_format_distance(playerid, playerpos, &rpcdata_freeform.byte[4]);
		bitstream_freeform.ptrData = &rpcdata_freeform;
		bitstream_freeform.numberOfBitsUsed = (2 + 2 + rpcdata_freeform.word[1]) * 8;
		SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream_freeform, playerid, 2);

		rpcdata_freeform.word[0] = td_kb_info.rpcdata->textdrawid;
		rpcdata_freeform.word[1] = missions_format_kneeboard_info_text(playerid, &rpcdata_freeform.byte[4]);
		bitstream_freeform.ptrData = &rpcdata_freeform;
		bitstream_freeform.numberOfBitsUsed = (2 + 2 + rpcdata_freeform.word[1]) * 8;
		SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bitstream_freeform, playerid, 2);
	} else {
		td_kb_distance.rpcdata->text_length = kneeboard_format_distance(playerid, playerpos, td_kb_distance.rpcdata->text);
		td_kb_info.rpcdata->text_length = missions_format_kneeboard_info_text(playerid, td_kb_info.rpcdata->text);

		textdraws_show(playerid, 3, &td_kb_header, &td_kb_info, &td_kb_distance);
		kneeboard_is_shown[playerid] = 1;
	}
}
