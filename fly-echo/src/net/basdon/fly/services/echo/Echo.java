// Copyright 2019-2021 basdon.net - this source is licensed under AGPL
// see the LICENSE file for more details
package net.basdon.fly.services.echo;

import java.io.IOException;
import java.io.InterruptedIOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Random;

import net.basdon.anna.api.Channel;
import net.basdon.anna.api.ChannelUser;
import net.basdon.anna.api.IAnna;
import net.basdon.anna.api.User;

import static java.lang.System.arraycopy;

import static net.basdon.anna.api.Constants.*;
import static net.basdon.anna.api.Util.*;

public
class Echo extends Thread
{
private static final int PORT_IN = 7767, PORT_OUT = 7768;
static final int
	PING_INTERVAL = 10000,
	PING_TIMEOUT_THRESHOLD = PING_INTERVAL + PING_INTERVAL / 2;
public static final byte
	PACK_HELLO = 2,
	PACK_IMTHERE = 3,
	PACK_BYE = 4,
	PACK_PING = 5,
	PACK_PONG = 6,
	PACK_STATUS_REQUEST = 7,
	PACK_STATUS = 8,
	PACK_CHAT = 10,
	PACK_ACTION = 11,
	PACK_GENERIC_MESSAGE = 12,
	PACK_PLAYER_CONNECTION = 30;
public static final byte
	PACK12_FLIGHT_MESSAGE = 0,
	PACK12_TRAC_MESSAGE = 1,
	PACK12_IRC_MODE = 3,
	PACK12_IRC_TOPIC = 4,
	PACK12_IRC_NICK = 5,
	PACK12_PROTIP = 6;
private static final InetAddress ADDR_OUT;

public static final byte
	CONN_REASON_GAME_TIMEOUT = 0,
	CONN_REASON_GAME_QUIT = 1,
	CONN_REASON_GAME_KICK = 2,
	CONN_REASON_GAME_CONNECTED = 3,
	CONN_REASON_IRC_QUIT = 6,
	CONN_REASON_IRC_PART = 7,
	CONN_REASON_IRC_KICK = 8,
	CONN_REASON_IRC_JOIN = 9;

static
{
	InetAddress addr = null;
	try {
		addr = Inet4Address.getByAddress(new byte[] { 127, 0, 0, 1 });
	} catch (Exception e) {
		e.printStackTrace();
	}
	ADDR_OUT = addr;
}

final IAnna anna;

private final byte[] buf = new byte[512];
private final char[] channel;
private final int[] last_ping_payloads = new int[10];
private final long[] last_ping_stamps = new long[10];

private int last_ping_idx;
private long last_pong_time;
private long my_hello_sent_time;
private boolean need_send_initial_status_message;
private char[] cached_status_msg;
private long cached_status_msg_time;
private HashSet<char[]> users_waiting_for_status_message;
private boolean status_message_requested_global;
private boolean send_status_message_response_back_to_game;
private PingThread pingThread;

public boolean ignore_self;
public DatagramSocket insocket;
public DatagramSocket outsocket;
public int packets_received, bytes_received, invalid_packet_count;
public long last_packet;
public boolean is_game_down;

public
Echo(IAnna anna, char[] channel)
{
	this.anna = anna;
	this.channel = channel;
	this.is_game_down = true;
	this.users_waiting_for_status_message = new HashSet<>();
	this.pingThread = new PingThread(this);
}

@Override
public
void run()
{
	try {
		try {
			this.outsocket = new DatagramSocket();
		} catch (SocketException e1) {
			msg("mod_bas_echo: could not create out socket? " + e1.toString());
		}
		for (;;) {
			try (DatagramSocket socket = new DatagramSocket(PORT_IN)) {
				this.insocket = socket;
				this.send_hello();
				this.need_send_initial_status_message = true;
				if (this.anna.find_channel(this.channel) != null) {
					this.send_initial_status_message();
				}
				this.pingThread.start();
				for (;;) {
					DatagramPacket packet = new DatagramPacket(buf, buf.length);
					socket.receive(packet);
					packets_received++;
					bytes_received += packet.getLength();
					last_packet = System.currentTimeMillis();
					try {
						handle(packet);
					} catch (InterruptedIOException e) {
						return;
					} catch (Throwable e) {
						e.printStackTrace();
					}
				}
			} catch (SocketException e) {
				if (this.isInterrupted()) {
					return;
				}
				msg("mod_bas_echo: socket issue, restarting socket");
			} catch (InterruptedIOException e) {
				return;
			} catch (IOException e) {
				msg("mod_bas_echo: data receive issue, restarting socket");
			}
			Thread.sleep(3000);
		}
	} catch (InterruptedException e) {
	} finally {
		this.pingThread.interrupt();
		byte[] msg = { 'F', 'L', 'Y', PACK_BYE };
		this.send(msg, msg.length);
		this.outsocket.close();
	}
}

/**
 * Call this on the anna thread.
 */
void check_ping_timeout()
{
	long time_since_last_pong = System.currentTimeMillis() - this.last_pong_time;

	if (this.is_game_down) {
		if (time_since_last_pong < PING_TIMEOUT_THRESHOLD) {
			this.is_game_down = false;
			this.anna.privmsg(this.channel, "Game is back up".toCharArray());
		}
		return;
	}

	if (time_since_last_pong < PING_TIMEOUT_THRESHOLD) {
		return;
	}

	this.is_game_down = true;
	this.users_waiting_for_status_message.clear();

	StringBuilder sb = new StringBuilder(450);
	sb.append(CTRL_COLOR).append(SCOL_RED);
	sb.append("Ping timeout, game is down!");
	Channel channel = this.anna.find_channel(this.channel);
	if (channel != null) {
		char[] user_channel_modes = this.anna.get_user_channel_modes();
		ArrayList<char[]> nickalerts = new ArrayList<>(channel.userlist.size());
		for (ChannelUser user : channel.userlist) {
			if (has_user_mode_or_higher(user, user_channel_modes, 'o') &&
				!strcmp(user.nick, 'C','h','a','n','S','e','r','v') &&
				!strcmp(user.nick, this.anna.get_anna_user().nick))
			{
				nickalerts.add(user.nick);
			}
		}
		for (char[] nick : nickalerts) {
			if (sb.length() + nick.length > 450) {
				continue;
			}
			sb.append(' ').append(nick);
		}
	}
	this.anna.privmsg(this.channel, chars(sb));
}

public
void send_status_message_to_user(User user)
{
	if (this.is_game_down) {
		this.anna.notice(user.nick, (CTRL_COLOR + SCOL_RED + "Game is down").toCharArray());
		return;
	}
	if (System.currentTimeMillis() - this.cached_status_msg_time < 1000) {
		this.send_status_message_to_user_now(user.nick);
	} else {
		this.users_waiting_for_status_message.add(user.nick);
		byte[] msg = { 'F', 'L', 'Y', PACK_STATUS_REQUEST };
		this.send(msg, msg.length);
	}
}

public
void on_user_nick_changed(char[] from, char[] to)
{
	if (this.users_waiting_for_status_message.remove(from)) {
		this.users_waiting_for_status_message.add(to);
	}
}

public
void remove_user_from_awaiting_status_message_list(char[] nick)
{
	this.users_waiting_for_status_message.remove(nick);
}

private
void send_status_message_to_user_now(char[] nick)
{
	this.anna.notice(nick, this.cached_status_msg, 0, this.cached_status_msg.length);
}

private
void send_status_message_to_awaiting_users()
{
	// This is always called from Echo thread.
	this.anna.sync_exec(() -> {
		for (char[] user : this.users_waiting_for_status_message) {
			this.send_status_message_to_user_now(user);
		}
		this.users_waiting_for_status_message.clear();
	});
}

public
void request_global_status(boolean send_response_back_to_game)
{
	byte[] msg = { 'F', 'L', 'Y', PACK_STATUS_REQUEST };
	this.send(msg, msg.length);
	this.status_message_requested_global = true;
	this.send_status_message_response_back_to_game = send_response_back_to_game;
}

private
void handle(DatagramPacket packet)
throws InterruptedIOException
{
	InetAddress a = packet.getAddress();
	if (!a.isLoopbackAddress()) {
		msg("mod_bas_echo: got packet from non-loopback: " + a.getHostAddress());
		return;
	}
	int length = packet.getLength();
	ByteBuf stream = new ByteBuf(buf);
	if (length < 4 || stream.readWord() != 0x4C46 || stream.readByte() != 'Y') {
		invalid_packet_count++;
		return;
	}
	byte packet_id = stream.readByte();
invalid_packet:
	switch (packet_id) {
	case PACK_HELLO:
		this.is_game_down = false;
		this.users_waiting_for_status_message.clear();
		this.last_pong_time = System.currentTimeMillis();
		buf[3] = PACK_IMTHERE;
		send(buf, length);
		msg("game bridge is up");
		this.send_status_message((byte) 0);
		return;
	case PACK_IMTHERE:
		this.is_game_down = false;
		this.users_waiting_for_status_message.clear();
		this.last_pong_time = System.currentTimeMillis();
		long roundtrip = System.currentTimeMillis() - this.my_hello_sent_time;
		msg("game bridge is up (" + roundtrip + "ms)");
		return;
	case PACK_BYE:
		this.is_game_down = true;
		this.users_waiting_for_status_message.clear();
		msg("game bridge is down");
		return;
	/*Game doesn't send pings.*/
	case PACK_PONG:
		long time = System.currentTimeMillis();
		this.last_pong_time = time;
		int num = stream.readDword();
		for (int i = 0; i < this.last_ping_payloads.length; i++) {
			if (num == this.last_ping_payloads[i]) {
				long delta = time - this.last_ping_stamps[i];
				if (delta > 30) {
					msg("warn: slow ping (" + delta + "ms > 30)");
				}
				return;
			}
		}
		return;
	case PACK_STATUS_REQUEST:
		this.send_status_message((byte) 1);
		return;
	case PACK_STATUS:
		if (length < 8) {
			break;
		}
		boolean is_response_to_status_request = stream.readByte() != 0;
		short statusMsgByteLength = stream.readWord();
		this.cached_status_msg_time = System.currentTimeMillis();
		CharBuf charbuf = new CharBuf(statusMsgByteLength);
		charbuf.writeUTF8(stream, statusMsgByteLength);
		this.cached_status_msg = new char[charbuf.pos];
		arraycopy(charbuf.chars, 0, this.cached_status_msg, 0, charbuf.pos);
		if (this.status_message_requested_global) {
			is_response_to_status_request = false;
		}
		if (is_response_to_status_request) {
			this.send_status_message_to_awaiting_users();
		} else {
			this.anna.sync_exec(() -> {
				this.users_waiting_for_status_message.clear();
				this.status_message_requested_global = false;
				// Don't send it back in game when a user without a voice has
				// invoked the &players command.
				this.ignore_self = !this.send_status_message_response_back_to_game;
				this.anna.privmsg(this.channel, charbuf.chars, 0, charbuf.pos);
				this.ignore_self = false;
				// Setting back to false because it shouldn't be sent back ingame
				// when receiving an unsollicited one when the server restarts.
				this.send_status_message_response_back_to_game = false;
			});
		}
		return;
	case PACK_ACTION:
	case PACK_CHAT:
	{
		if (length < 11) {
			break;
		}
		short playerid = stream.readWord(); // 5-6
		byte nicklen = stream.readByte(); // 7
		if (nicklen < 1 || 49 < nicklen) {
			break;
		}
		short msglen = stream.readWord(); // 8-9
		/*Arbitrary length limit. Echo spec says 512, but 512 is the limit of an IRC message.*/
		if (msglen < 1 || 480 < msglen) {
			break;
		}
		if (9 + nicklen + msglen != length) {
			break;
		}
		CharBuf msg = new CharBuf(512);
		msg.writeUTF8(stream, nicklen);
		msg.writeChar('(');
		msg.writeString(String.valueOf(playerid));
		msg.writeChar(')');
		if (packet_id != PACK_ACTION) {
			msg.writeChar(':');
		}
		msg.writeChar(' ');
		msg.writeUTF8Filtered(stream, msglen);
		this.anna.sync_exec(() -> {
			this.ignore_self = true;
			if (packet_id == PACK_ACTION) {
				this.anna.action(this.channel, msg.chars, 0, msg.pos);
			} else {
				this.anna.privmsg(this.channel, msg.chars, 0, msg.pos);
			}
			this.ignore_self = false;
		});
		return;
	}
	case PACK_GENERIC_MESSAGE:
	{
		if (length < 8) {
			break;
		}
		byte type = stream.readByte(); // 4
		short msglen = stream.readWord(); // 5-6
		if (msglen < 1 || 450 < msglen) {
			break;
		}
		if (length != 7 + msglen) {
			break;
		}
		CharBuf msg;
		switch(type) {
		case PACK12_FLIGHT_MESSAGE:
			msg = new CharBuf(3 + msglen);
			msg.writeChar(CTRL_COLOR);
			msg.writeString(SCOL_ORANGE);
			break;
		case PACK12_TRAC_MESSAGE:
			msg = new CharBuf(3 + msglen);
			msg.writeChar(CTRL_COLOR);
			msg.writeString(SCOL_BROWN);
			break;
		case PACK12_PROTIP:
			msg = new CharBuf(6 + msglen);
			msg.writeChar(CTRL_COLOR);
			msg.writeString(SCOL_CYAN);
			msg.writeChar(',');
			msg.writeString(SCOL_BLACK);
			break;
		default:
			break invalid_packet;
		}
		msg.writeUTF8Filtered(stream, msglen);
		this.anna.sync_exec(() -> {
			this.ignore_self = true;
			this.anna.privmsg(this.channel, msg.chars, 0, msg.pos);
			this.ignore_self = false;
		});
		return;
	}
	case PACK_PLAYER_CONNECTION:
		if (length < 9) {
			break;
		}
		short playerid = stream.readWord(); // 4-5
		byte reason = stream.readByte(); // 6
		byte nicklen = stream.readByte(); // 7
		if (nicklen < 1 || 50 < nicklen) {
			break;
		}
		if (8 + nicklen != length) {
			break;
		}
		CharBuf msg = new CharBuf(256);
		msg.writeChar(CTRL_COLOR);
		msg.writeString(SCOL_GREY);
		if (reason == CONN_REASON_GAME_CONNECTED) {
			msg.writeString("-> ");
		} else {
			msg.writeString("<- ");
		}
		msg.writeUTF8(stream, nicklen);
		msg.writeChar('(');
		msg.writeString(String.valueOf(playerid));
		msg.writeChar(')');
		if (reason == CONN_REASON_GAME_CONNECTED) {
			msg.writeString(" connected to the server");
		} else {
			msg.writeString(" disconnected from the server");
			switch (reason) {
			case CONN_REASON_GAME_TIMEOUT: msg.writeString(" (ping timeout)"); break;
			case CONN_REASON_GAME_QUIT: msg.writeString(" (quit)"); break;
			case CONN_REASON_GAME_KICK: msg.writeString(" (kicked)"); break;
			}
		}
		this.anna.sync_exec(() -> {
			this.ignore_self = true;
			this.anna.privmsg(this.channel, msg.chars, 0, msg.pos);
			this.ignore_self = false;
		});
		return;
	}
	invalid_packet_count++;
}

private
void send(byte[] buf, int len)
{
	if (this.outsocket != null) {
		try {
			this.outsocket.send(new DatagramPacket(buf, len, ADDR_OUT, PORT_OUT));
		} catch (IOException e) {
			// slurp
		}
	}
}

private
void msg(String message)
{
	this.anna.sync_exec(() -> {
		this.anna.privmsg(this.channel, message.toCharArray());
	});
}

/**
 * Send irc message to game.
 * Escaping should be done at the game side.
 *
 * @param isaction {@code true} if it's an action message
 * @param prefix prefix of user, or {@code 0}
 * @param nickname irc user nick name
 * @param message message
 * @param off offset in message
 * @param len length of message
 */
public
void send_chat_or_action_to_game(byte packet_type, char prefix, char[] nickname,
                                 char[] message, int off, int len)
{
	byte nicklen = (byte) Math.min(nickname.length, 49);
	if (prefix != 0) {
		nicklen++;
	}
	int msgLenChars = Math.min(len, 512);

	ByteBuf buf = new ByteBuf(9 + nicklen + msgLenChars * 4, packet_type);
	buf.writeWord(0); // playerid, 0 as per spec
	buf.writeByte(nicklen);
	buf.markAndSkip(2); // msglen - to be set after writing the msg
	if (prefix != 0) {
		buf.writeByte(prefix);
	}
	buf.writeASCII(nickname, 0, nickname.length);
	int msgLenBytes = buf.writeUTF8(message, off, msgLenChars);
	int bufLen = buf.pos;
	buf.exchangeMarkAndPos();
	buf.writeWord(msgLenBytes);

	if (bufLen > 510) {
		bufLen = 510;
	}
	this.send(buf.buf, bufLen);
}

/**
 * Send irc user connection to game.
 *
 * @param user user of which the connection changed
 * @param reason reason as defined in {@code CONN_REASON_IRC_*} constants
 */
public
void send_player_connection(ChannelUser user, byte reason)
{
	byte nicklen = (byte) Math.min(user.nick.length, 49);
	if (user.prefix != 0) {
		nicklen++;
	}

	ByteBuf buf = new ByteBuf(8 + nicklen, PACK_PLAYER_CONNECTION);
	buf.writeWord(0); // playerid, 0 as per spec
	buf.writeByte(reason);
	buf.writeByte(nicklen);
	if (user.prefix != 0) {
		buf.writeByte(user.prefix);
	}
	buf.writeASCII(user.nick, 0, user.nick.length);

	this.send(buf.buf, buf.pos);
}

/**
 * Send ping packet to game.
 * Stores the ping id (4 random bytes) so the round-trip time can be calculated when receiving a
 * pong back.
 */
void send_ping()
{
	int randomNumber = new Random().nextInt();
	randomNumber |= 0x02020202; // as per packet spec, bytes can't contain 0 or 1

	ByteBuf msg = new ByteBuf(8, PACK_PING);
	msg.writeDword(randomNumber);
	this.send(msg.buf, msg.buf.length);

	this.last_ping_idx++;
	if (this.last_ping_idx >= this.last_ping_payloads.length) {
		this.last_ping_idx = 0;
	}
	this.last_ping_payloads[this.last_ping_idx] = randomNumber;
	this.last_ping_stamps[this.last_ping_idx] = System.currentTimeMillis();
}

private
void send_hello()
{
	byte[] msg = { 'F', 'L', 'Y', PACK_HELLO, 7, 3, 3, 1 };
	this.my_hello_sent_time = System.currentTimeMillis();
	this.send(msg, msg.length);
}

public
void send_generic_message(char[] message, byte type)
{
	int msgLenChars = message.length;
	if (msgLenChars > 450) {
		msgLenChars = 450;
	}

	ByteBuf buf = new ByteBuf(7 + msgLenChars * 4, PACK_GENERIC_MESSAGE);
	buf.writeByte(type);
	buf.markAndSkip(2); // msglen - to be set after writing the msg
	int msgLenBytes = buf.writeUTF8(message, 0, msgLenChars);
	int bufLen = buf.pos;
	buf.exchangeMarkAndPos();
	buf.writeWord(msgLenBytes);

	if (bufLen > 510) {
		bufLen = 510;
	}
	this.send(buf.buf, bufLen);
}

public
void send_initial_status_message()
{
	if (this.need_send_initial_status_message) {
		this.need_send_initial_status_message = false;
		this.send_status_message((byte) 0);
	}
}

private
void send_status_message(byte is_response_to_status_request)
{
	String server = this.anna.get_anna_conf().getStr("server.host");
	Channel channel = this.anna.find_channel(this.channel);

	ByteBuf buf = new ByteBuf(512, PACK_STATUS);
	buf.writeByte(is_response_to_status_request);
	buf.markAndSkip(2); // msglen - to be set after writing the msg
	int msgLen = buf.pos;
	buf.writeString("IRC status: connected to ");
	buf.writeASCII(this.channel, 0, this.channel.length);
	buf.writeString(" on ");
	if (server == null) {
		buf.writeString("<unknown server>");
	} else {
		buf.writeString(server);
	}
	buf.writeString(". ");
	if (channel == null) {
		buf.writeString("Failed to get channel users.");
	} else {
		char[] user_channel_models = this.anna.get_user_channel_modes();
		int unfilteredSize = channel.userlist.size();
		ArrayList<ChannelUser> oppedUsers = new ArrayList<>(unfilteredSize);
		ArrayList<ChannelUser> voicedUsers = new ArrayList<>(unfilteredSize);
		ArrayList<ChannelUser> normalUsers = new ArrayList<>(unfilteredSize);
		int num_users = 0;
		for (ChannelUser user : channel.userlist) {
			if (!strcmp(user.nick, 'C','h','a','n','S','e','r','v') &&
				!strcmp(user.nick, this.anna.get_anna_user().nick))
			{
				num_users++;
				if (has_user_mode_or_higher(user, user_channel_models, '@')) {
					oppedUsers.add(user);
				} else if (has_user_mode_or_higher(user, user_channel_models, '+')) {
					voicedUsers.add(user);
				} else {
					normalUsers.add(user);
				}
			}
		}
		if (num_users == 0) {
			buf.writeString("No users in channel.");
		} else {
			buf.writeString(String.valueOf(num_users));
			buf.writeString(" user(s):");
			int num_printedUsers = 0;
			ArrayList<ChannelUser> list = oppedUsers;
			int list_elements_count = list.size();
			for (int i = 0; ; i++) {
				if (i >= list_elements_count) {
					if (list == oppedUsers) {
						list = voicedUsers;
					} else if (list == voicedUsers) {
						list = normalUsers;
					} else {
						break;
					}
					list_elements_count = list.size();
					i = -1;
					continue;
				}
				ChannelUser user = list.get(i);
				// Length of the message should be under 2 samp chat lines.
				// Message should also fit "and xx others." after the limit checked here.
				if (buf.pos + user.nick.length > 285 - 15) {
					break;
				}
				num_printedUsers++;
				buf.writeByte(' ');
				if (user.prefix != 0) {
					buf.writeByte(user.prefix);
				}
				buf.writeASCII(user.nick, 0, user.nick.length);
			}
			if (num_users != num_printedUsers) {
				buf.writeString(" and ");
				buf.writeString(String.valueOf(num_users - num_printedUsers));
				buf.writeString(" more");
			}
		}
	}
	msgLen = buf.pos - msgLen;
	int bufLen = buf.pos;
	buf.exchangeMarkAndPos();
	buf.writeWord(msgLen);

	this.send(buf.buf, bufLen);
}
}
