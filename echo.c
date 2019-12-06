
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "echo.h"
#include "playerdata.h"
#include <string.h>

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
		memcpy(cbuf144 + 8, pd->name, nicklen + 1);
		NC_ssocket_send(socket_out, buf144a, 8 + nicklen + 1);
	}
}

void echo_on_flight_finished(char *text)
{
	int textlen;

	if (socket_out != SOCKET_INVALID_SOCKET) {
		textlen = strlen(text);
		if (textlen > 144) {
			textlen = 144;
		}
		buf144[0] = 0x0C594C46;
		cbuf144[4] = textlen;
		memcpy(cbuf144 + 5, text, textlen);
		NC_ssocket_send(socket_out, buf144a, 5 + textlen);
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
		memcpy((cbuf144) + 8, pd->name, nicklen + 1);
		memcpy((cbuf144) + 9 + nicklen, text, msglen);
		*((cbuf144) + 9 + nicklen + msglen) = 0;
		NC_ssocket_send(socket_out, buf144a, 10+nicklen+msglen);
	}
}

/**
Send text in buf4096 using SendClientToAll after sanitizing.

Currently only replaces % characters to # characters.
Also splits up message when it's too long.
*/
static
void echo_sendclientmessage_buf4096_filtered()
{
	int len;
	cell tmp;
	cell addr, *b;

	len = 0;
	addr = buf4096a;
	b = buf4096;
	/*escape stuff TODO: escape embedded colors?*/
	while (*b != 0) {
		if (*b == '%') {
			*b = '#';
		}
		/*split up message when too long*/
		if (len > 140) {
			tmp = *b;
			*b = 0;
			NC_SendClientMessageToAll(COL_IRC, addr);
			addr += len * 4;
			len = 1;
			*b = tmp;
		}
		b++;
		len++;
	}
	NC_SendClientMessageToAll(COL_IRC, addr);
}

static const char *BRIDGE_UP = "IRC bridge is up";
static const char *BRIDGE_DOWN = "IRC bridge is down";

void echo_on_receive(cell socket_handle, cell data_a,
		     char *data, int len)
{
	if (socket_handle == socket_in && len >= 4 &&
		data[0] == 'F' && data[1] == 'L' && data[2] == 'Y')
	{
		switch (data[3]) {
		case PACK_HELLO:
			logprintf((char*) BRIDGE_UP);
			B144((char*) BRIDGE_UP);
			NC_SendClientMessageToAll(COL_IRC, buf144a);
			if (len == 8) {
				data[3] = PACK_IMTHERE;
				NC_ssocket_send(socket_out, data_a, 8);
			}
			break;
		case PACK_IMTHERE:
			logprintf((char*) BRIDGE_UP);
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
			logprintf((char*) BRIDGE_DOWN);
			if (len == 4) {
				B144((char*) BRIDGE_DOWN);
				NC_SendClientMessageToAll(COL_IRC, buf144a);
			}
			break;
		case PACK_CHAT:
		case PACK_ACTION:
		{
			short msglen;
			int nicklen;
			cell *b;

			if (len < 13 ||
				(nicklen = data[6]) < 1 || nicklen > 49 ||
				(msglen = (data[7] & 0xFF)) < 1 ||
				msglen > 144 ||
				10 + nicklen + msglen != len)
			{
				break;
			}
			buf4096[0] = 'I';
			buf4096[1] = 'R';
			buf4096[2] = 'C';
			buf4096[3] = ' ';
			b = buf4096 + 4;
			if (data[3] == PACK_ACTION) {
				*(b++) = '*';
				*(b++) = ' ';
			} else {
				*(b++) = '<';

			}
			atoc(b, data + 8, nicklen * sizeof(cell));
			b += nicklen;
			if (data[3] != PACK_ACTION) {
				*(b++) = '>';
			}
			*(b++) = ' ';
			atoc(b,
				data + 9 + nicklen,
				(msglen + 1) * sizeof(cell));
			echo_sendclientmessage_buf4096_filtered();
			break;
		}
		case PACK_PLAYER_CONNECTION:
		{
			int nicklen;
			char buf[144], *b = buf;

			if (len < 9 ||
				(nicklen = data[7]) < 1 || nicklen > 49 ||
				9 + nicklen != len)
			{
				break;
			}
			*((int*) b) = 0x3A435249;
			b += 4;
			*(b++) = ' ';
			switch (data[6]) {
			case ECHO_CONN_REASON_IRC_QUIT:
				memcpy(b, "Quits: ", 7);
				b += 7;
				break;
			case ECHO_CONN_REASON_IRC_PART:
				memcpy(b, "Parts: ", 7);
				b += 7;
				break;
			case ECHO_CONN_REASON_IRC_KICK:
				memcpy(b, "Kicked: ", 8);
				b += 8;
				break;
			case ECHO_CONN_REASON_IRC_JOIN:
				memcpy(b, "Joins: ", 7);
				b += 7;
				break;
			}
			/*copy one more here because it'll add a 0-term*/
			atoc(buf4096, buf, b - buf + 1);
			atoc(buf4096 + (b - buf), data + 8,
				(nicklen + 1) * sizeof(cell));
			echo_sendclientmessage_buf4096_filtered();
			break;
		}
		}
	}
}

