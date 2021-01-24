#define ECHO_PORT_OUT 7767
#define ECHO_PORT_IN 7768
#define ECHO_PING_INTERVAL 10000
#define ECHO_PING_TIMEOUT_THRESHOLD (ECHO_PING_INTERVAL + ECHO_PING_INTERVAL / 2)

#define COL_IRC COL_INFO_GENERIC

static cell socket_in = SOCKET_INVALID_SOCKET;
static cell socket_out = SOCKET_INVALID_SOCKET;

static short players_needing_echo_status_message[MAX_PLAYERS];
static int num_players_needing_echo_status_message;
static char need_request_echo_status;
static int last_request_echo_status_time;

static int echo_last_ping_received;
static char echo_is_irc_down;

int echo_init(void *data)
{
	static const char *BUFLO = "127.0.0.1";

	int sin, timer_interval = 0;

	echo_is_irc_down = 1;

	if (socket_in == SOCKET_INVALID_SOCKET) {
		sin = NC_ssocket_create(SOCKET_UDP);
		if (sin == SOCKET_INVALID_SOCKET) {
			logprintf("failed to create echo game socket");
			timer_interval = 60000;
		} else {
			if (!NC_ssocket_listen(sin, ECHO_PORT_IN)) {
				NC_ssocket_destroy(sin);
				timer_interval = 60000;
			} else {
				socket_in = sin;
			}
		}
	}
	if (socket_out == SOCKET_INVALID_SOCKET) {
		socket_out = NC_ssocket_create(SOCKET_UDP);
		if (socket_out == SOCKET_INVALID_SOCKET) {
			logprintf("failed to create echo irc socket");
			timer_interval = 60000;
		} else {
			atoc(buf32, (char*) BUFLO, 32);
			NC_ssocket_connect(socket_out, buf32a, ECHO_PORT_OUT);
			buf144[0] = 0x02594C46;
			buf144[1] = 0x07030301;
			NC_ssocket_send(socket_out, buf144a, 8);
		}
	}

	return timer_interval;
}

void echo_dispose()
{
	if (socket_out != SOCKET_INVALID_SOCKET) {
		buf144[0] = 0x04594C46;
		NC_ssocket_send(socket_out, buf144a, 4);
		NC_ssocket_destroy(socket_out);
		socket_out = SOCKET_INVALID_SOCKET;
	}
	if (socket_in != SOCKET_INVALID_SOCKET) {
		NC_ssocket_destroy(socket_in);
		socket_in = SOCKET_INVALID_SOCKET;
	}
}

/**
Send player connection packet to IRC echo.

@param playerid playerid that (dis)connected
@param reason 3 when OnPlayerConnection, samp disconnection reason when OnPlayerDisconnect
*/
static
void echo_on_player_connection(int playerid, int reason)
{
	int nicklen;
	struct playerdata *pd = pdata[playerid];

	if (socket_out != SOCKET_INVALID_SOCKET) {
		nicklen = pd->namelen;
		buf144[0] = 0x1E594C46;
		buf144[1] =
			(playerid & 0xFFFF) |
			((reason & 0xFF) << 16) |
			((nicklen & 0xFF) << 24);
		memcpy(cbuf144 + 8, pd->name, nicklen);
		NC_ssocket_send(socket_out, buf144a, 8 + nicklen);
	}
}

static
void echo_send_status_request()
{
	if (socket_out != SOCKET_INVALID_SOCKET) {
		buf144[0] = 0x07594C46;
		NC_ssocket_send(socket_out, buf144a, 4);
	}
}

/**
The /irc command shows irc status
*/
static
int echo_cmd_irc(CMDPARAMS)
{
	int i;

	if (echo_is_irc_down) {
		SendClientMessage(playerid, COL_WARN, WARN"IRC bridge is down!");
	} else {
		i = 0;
		while (i < num_players_needing_echo_status_message) {
			if (players_needing_echo_status_message[i] == playerid) {
				return 1;
			}
			i++;
		}
		players_needing_echo_status_message[num_players_needing_echo_status_message++] = playerid;
		need_request_echo_status = 1;
	}
	return 1;
}

static
void echo_on_player_connect(int playerid)
{
	echo_on_player_connection(playerid, ECHO_CONN_REASON_GAME_CONNECTED);

	if (echo_is_irc_down) {
		SendClientMessage(playerid, COL_WARN, WARN"IRC bridge is down!");
	} else {
		players_needing_echo_status_message[num_players_needing_echo_status_message++] = playerid;
		need_request_echo_status = 1;
	}
}

static
void echo_on_player_disconnect(int playerid, int reason)
{
	int i, popped;

	echo_on_player_connection(playerid, reason);

	i = 0;
	while (i < num_players_needing_echo_status_message) {
		if (players_needing_echo_status_message[i] == playerid) {
			popped = players_needing_echo_status_message[--num_players_needing_echo_status_message];
			players_needing_echo_status_message[i] = popped;
			break;
		}
		i++;
	}
}

/**
Send flight finish message to IRC echo.

Uses buf4096.

@param type one of the ECHO_PACK12_* constants that is allowed to be sent from the game, as per packet spec.
*/
static
void echo_send_generic_message(char type, char *text)
{
#pragma pack(push,1)
	struct GENERIC_MESSAGE {
		union {
			int magic;
			struct {
				char _pad0[3];
				char packet_type;
			} bytes;
		} packet_header;
		char type;
		short message_length;
		char message[1]; /*actually arbitrary size*/
	};
#pragma pack(pop)

	struct GENERIC_MESSAGE *data;
	int textlen;

	if (socket_out != SOCKET_INVALID_SOCKET) {
		textlen = strlen(text);
		if (textlen > 450) {
			textlen = 450;
		}
		data = (void*) cbuf144;
		data->packet_header.magic = 0x594C46;
		data->packet_header.bytes.packet_type = PACK_GENERIC_MESSAGE;
		data->type = type;
		data->message_length = (short) textlen;
		memcpy(data->message, text, textlen);
		NC_ssocket_send(socket_out, buf144a, 7 + textlen);
	}
}

void echo_on_game_chat_or_action(int t, int playerid, char *text)
{
	int nicklen, msglen;
	struct playerdata *pd = pdata[playerid];

	if (socket_out != SOCKET_INVALID_SOCKET) {
		nicklen = pd->namelen;
		msglen = strlen(text);
		if (msglen > 480) {
			msglen = 480;
		}
#if PACK_ACTION != PACK_CHAT + 1
#error "the next line will fail"
#endif
		buf144[0] = 0x0A594C46 + 0x01000000 * t;
		buf144[1] =
			(playerid & 0xFFFF) |
			((pd->namelen & 0xFF) << 16);
		cbuf144[7] = msglen & 0xFF;
		cbuf144[8] = (msglen >> 8) & 0xFF;
		memcpy((cbuf144) + 9, pd->name, nicklen);
		memcpy((cbuf144) + 9 + nicklen, text, msglen);
		NC_ssocket_send(socket_out, buf144a, 9 + nicklen + msglen);
	}
}

static
void echo_send_status_message(char is_response_to_status_request)
{
	int i, playerid;
	short msglen;
	char *bufstart;
	char *bufpos;

	if (socket_out != SOCKET_INVALID_SOCKET) {
		msglen = 0;
		buf4096[0] = 0x08594C46;
		cbuf4096[4] = is_response_to_status_request;
		bufstart = cbuf4096 + 7;
		bufpos = bufstart;
		if (playercount) {
			bufpos += sprintf(bufpos, "%d player(s) online:", playercount);
			for (i = 0; i < playercount; i++) {
				if (bufpos - bufstart > 450) {
					bufpos += sprintf(bufpos, " and %d more", playercount - i);
					break;
				}
				playerid = players[i];
				bufpos += sprintf(bufpos, " %s(%d)", pdata[playerid]->name, playerid);
			}
		} else {
			bufpos += sprintf(bufpos, "No players online (or none passed login screen).");
		}
		msglen = bufpos - bufstart;
		*(short*) (cbuf4096 + 5) = msglen;
		NC_ssocket_send(socket_out, buf4096a, 7 + msglen);
	}
}

static
void echo_on_receive_status_message(char *buf, int len)
{
#pragma pack(push,1)
	struct STATUS_MESSAGE {
		int packet_header;
		char is_response_to_status_request;
		short message_length;
		char message[1]; /*actually arbitrary size*/
	};
#pragma pack(pop)

	struct STATUS_MESSAGE *data;

	data = (void*) buf;
	data->message[data->message_length] = 0;
	if (!data->is_response_to_status_request) {
		/*Means the IRC brige just got online, so inform everyone.*/
		SendClientMessageToAll(COL_IRC, data->message);
	} else {
		SendClientMessageToBatch(players_needing_echo_status_message, num_players_needing_echo_status_message, COL_IRC, data->message);
	}
	num_players_needing_echo_status_message = 0;
}

void echo_on_receive(cell socket_handle, cell data_a,
		     char *data, int len)
{
	/*An Echo message can be up to 512 chars*/
	char msg512[512];

	if (socket_handle == socket_in && len >= 4 &&
		data[0] == 'F' && data[1] == 'L' && data[2] == 'Y')
	{
		switch (data[3]) {
		case PACK_HELLO:
			echo_is_irc_down = 0;
			echo_last_ping_received = time_timestamp();
			logprintf("IRC bridge is up");
			SendClientMessageToAll(COL_IRC, "IRC brige is up");
			if (len == 8) {
				data[3] = PACK_IMTHERE;
				NC_ssocket_send(socket_out, data_a, 8);
			}
			echo_send_status_message(0);
			break;
		case PACK_IMTHERE:
			echo_is_irc_down = 0;
			echo_last_ping_received = time_timestamp();
			logprintf("IRC bridge is up");
			/*no point printing this to chat, because this only
			happens right after the server starts, thus nobody
			is connected yet*/
			break;
		case PACK_BYE:
			echo_is_irc_down = 1;
			need_request_echo_status = 0;
			num_players_needing_echo_status_message = 0;
			logprintf("IRC bridge is down");
			SendClientMessageToAll(COL_IRC, "IRC brige is down");
			break;
		case PACK_PING:
			if (len == 8) {
				echo_last_ping_received = time_timestamp();
				data[3] = PACK_PONG;
				NC_ssocket_send(socket_out, data_a, 8);
			}
			break;
		/*game doesn't send PING packets, so not checking PONG*/
		case PACK_STATUS_REQUEST:
			echo_send_status_message(1);
			break;
		case PACK_STATUS:
			echo_on_receive_status_message(data, len);
			break;
		case PACK_CHAT:
		case PACK_ACTION:
		{
			short msglen;
			int nicklen, col;
			char *b;

			if (len < 11 ||
				(nicklen = data[6]) < 1 || nicklen > 49 ||
				(msglen = *((short*) (data + 7))) < 1 ||
				msglen > 512 ||
				9 + nicklen + msglen != len)
			{
				break;
			}
			msg512[0] = 'I';
			msg512[1] = 'R';
			msg512[2] = 'C';
			msg512[3] = ':';
			msg512[4] = ' ';
			b = msg512 + 5;
			if (data[3] == PACK_ACTION) {
				*(b++) = '*';
				*(b++) = ' ';
				col = COL_IRC_ACTION;
			} else {
				*(b++) = '<';
				col = COL_IRC;
			}
			memcpy(b, data + 9, nicklen + 1);
			b += nicklen;
			if (data[3] != PACK_ACTION) {
				*(b++) = '>';
			}
			*(b++) = ' ';
			memcpy(b, data + 9 + nicklen, msglen);
			b[msglen] = 0;
			/*Do we want to filter out embedded colors?*/
			SendClientMessageToAll(col, msg512);
			break;
		}
		case PACK_GENERIC_MESSAGE:
		{
			short msglen;
			int col;

			if (len < 8 ||
				(msglen = *((short*) (data + 5))) > 450 ||
				7 + msglen != len)
			{
				break;
			}
			memcpy(msg512, data + 7, msglen);
			msg512[msglen] = 0;
			switch (data[4]) {
			case ECHO_PACK12_TRAC_MESSAGE:
				col = COL_INFO_BROWN;
				break;
			case ECHO_PACK12_IRC_MODE:
			case ECHO_PACK12_IRC_TOPIC:
				col = COL_IRC_MODE;
				break;
			case ECHO_PACK12_IRC_NICK:
				col = COL_IRC_ACTION;
				break;
			default:
				return;
			}
			SendClientMessageToAll(col, msg512);
			break;
		}
		case PACK_PLAYER_CONNECTION:
		{
			int nicklen;
			char *b;

			if (len < 8 ||
				(nicklen = data[7]) < 1 || nicklen > 49 ||
				8 + nicklen != len)
			{
				break;
			}
			switch (data[6]) {
			case ECHO_CONN_REASON_IRC_QUIT:
				b = msg512 + sprintf(msg512, "IRC: Quits: ");
				break;
			case ECHO_CONN_REASON_IRC_PART:
				b = msg512 + sprintf(msg512, "IRC: Parts: ");
				break;
			case ECHO_CONN_REASON_IRC_KICK:
				b = msg512 + sprintf(msg512, "IRC: Kicked: ");
				break;
			case ECHO_CONN_REASON_IRC_JOIN:
				b = msg512 + sprintf(msg512, "IRC: Joins: ");
				break;
			default:
				return;
			}
			memcpy(b, data + 8, nicklen);
			b[nicklen] = 0;
			SendClientMessageToAll(COL_IRC, msg512);
			break;
		}
		}
	}
}

static
void echo_tick()
{
	int time;

	if (!echo_is_irc_down && time_timestamp() - echo_last_ping_received > ECHO_PING_TIMEOUT_THRESHOLD) {
		SendClientMessageToAll(COL_WARN, WARN"IRC bridge ping timeout!");
		echo_is_irc_down = 1;
		need_request_echo_status = 0;
		num_players_needing_echo_status_message = 0;
	}

	if (need_request_echo_status) {
		time = time_timestamp();
		if (time - last_request_echo_status_time > 1000) {
			need_request_echo_status = 0;
			last_request_echo_status_time = time;
			echo_send_status_request();
		}
	}
}
