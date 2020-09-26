#define ECHO_PORT_OUT 7767
#define ECHO_PORT_IN 7768

#define COL_IRC COL_INFO_GENERIC

static cell socket_in = SOCKET_INVALID_SOCKET;
static cell socket_out = SOCKET_INVALID_SOCKET;

int echo_init(void *data)
{
	static const char *BUFLO = "127.0.0.1";

	int sin, timer_interval = 0;

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

void echo_on_flight_finished(char *text)
{
	int textlen;

	if (socket_out != SOCKET_INVALID_SOCKET) {
		textlen = strlen(text);
		if (textlen > 450) {
			textlen = 450;
		}
#if 0x0C != PACK_GENERIC_MESSAGE
#error generic message packet should be 12
#endif
		buf144[0] = 0x0C594C46;
		cbuf144[4] = PACK12_FLIGHT_MESSAGE;
		*((short*) (cbuf144 + 5)) = (short) textlen;
		memcpy(cbuf144 + 7, text, textlen);
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
		if (msglen > 144) {
			msglen = 144;
		}
#if PACK_ACTION != PACK_CHAT + 1
#error "the next line will fail"
#endif
		buf144[0] = 0x0A594C46 + 0x01000000 * t;
		buf144[1] =
			(playerid & 0xFFFF) |
			((pd->namelen & 0xFF) << 16) |
			((msglen & 0xFF) << 24);
		memcpy((cbuf144) + 8, pd->name, nicklen);
		memcpy((cbuf144) + 8 + nicklen, text, msglen);
		NC_ssocket_send(socket_out, buf144a, 8 + nicklen + msglen);
	}
}

void echo_on_receive(cell socket_handle, cell data_a,
		     char *data, int len)
{
	/*An IRC message can be up to 512 chars*/
	char msg512[512];

	if (socket_handle == socket_in && len >= 4 &&
		data[0] == 'F' && data[1] == 'L' && data[2] == 'Y')
	{
		switch (data[3]) {
		case PACK_HELLO:
			logprintf("IRC bridge is up");
			SendClientMessageToAll(COL_IRC, "IRC brige is up");
			if (len == 8) {
				data[3] = PACK_IMTHERE;
				NC_ssocket_send(socket_out, data_a, 8);
			}
			break;
		case PACK_IMTHERE:
			logprintf("IRC bridge is up");
			/*no point printing this to chat, because this only
			happens right after the server starts, thus nobody
			is connected yet*/
			break;
		case PACK_PING:
			if (len == 8) {
				data[3] = PACK_PONG;
				NC_ssocket_send(socket_out, data_a, 8);
			}
			break;
		/*game doesn't send PING packets, so not checking PONG*/
		case PACK_BYE:
			logprintf("IRC bridge is down");
			if (len == 4) {
				/*TODO: why is this only sent when len is checked?*/
				SendClientMessageToAll(COL_IRC, "IRC brige is down");
			}
			break;
		case PACK_CHAT:
		case PACK_ACTION:
		{
			short msglen;
			int nicklen, col;
			char *b;

			if (len < 10 ||
				(nicklen = data[6]) < 1 || nicklen > 49 ||
				(msglen = (data[7] & 0xFF)) < 1 ||
				msglen > 144 ||
				8 + nicklen + msglen != len)
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
			memcpy(b, data + 8, nicklen + 1);
			b += nicklen;
			if (data[3] != PACK_ACTION) {
				*(b++) = '>';
			}
			*(b++) = ' ';
			memcpy(b, data + 8 + nicklen, msglen);
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
			case PACK12_TRAC_MESSAGE:
				col = COL_INFO_BROWN;
				break;
			case PACK12_IRC_MODE:
			case PACK12_IRC_TOPIC:
				col = COL_IRC_MODE;
				break;
			case PACK12_IRC_NICK:
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

