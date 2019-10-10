// Copyright 2019 basdon.net - this source is licensed under GPL
// see the LICENSE file for more details
package net.basdon.fly.services.echo;

import java.io.IOException;
import java.io.InterruptedIOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.SocketException;
import java.nio.charset.StandardCharsets;

import net.basdon.anna.api.IAnna;

public class Echo extends Thread
{
private static final int PORT_IN = 7767, PORT_OUT = 7768;
private static final char
	PACK_HELLO = 0,
	PACK_IMTHERE = 1,
	PACK_PING = 2,
	PACK_PONG = 3,
	PACK_CHAT = 10;
private static final InetAddress ADDR_OUT;

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

private final byte[] buf = new byte[160];
private final char[] channel;
private final IAnna anna;

public DatagramSocket insocket;
public DatagramSocket outsocket;
public int packets_received, bytes_received, invalid_packet_count;
public long last_packet;

public Echo(IAnna anna, char[] channel)
{
	this.anna = anna;
	this.channel = channel;
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
	} catch (InterruptedException e) {}
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
	if (length < 4 || buf[0] != 'F' || buf[1] != 'L' || buf[2] != 'Y') {
		invalid_packet_count++;
		return;
	}
	switch (buf[3]) {
	case PACK_HELLO:
		buf[3] = PACK_IMTHERE;
		send(buf, length);
		msg("server says hello");
		return;
	case PACK_PING:
		buf[3] = PACK_PONG;
		send(buf, length);
		return;
	case PACK_CHAT:
		byte nicklen, msglen;
		if (length > 8 &&
			(nicklen = buf[6]) > 0 && nicklen < 50 &&
			(msglen = buf[7]) > 0 && msglen < 144 &&
			6 + nicklen + msglen == length)
		{
			int pid = (buf[4] & 0xFF) | ((buf[5] & 0xFF) << 8);
			StringBuilder sb = new StringBuilder(225);
			sb.append(new String(buf, 8, nicklen, StandardCharsets.US_ASCII));
			sb.append(' ').append('(').append(pid).append(')').append(':').append(' ');
			sb.append(new String(buf, 9 + nicklen, msglen, StandardCharsets.US_ASCII));
			msg(sb.toString());
			return;
		}
		break;
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
 *
 * @param prefix prefix of user, or {@code 0}
 * @param nickname irc user nick name
 * @param message message
 */
public
void send_chat_to_game(char prefix, char[] nickname, char[] message)
{
	// TODO: escape embedded colors?
	byte nicklen = (byte) Math.min(nickname.length, 49);
	if (prefix != 0) {
		nicklen++;
	}
	byte msglen = (byte) Math.min(message.length, 144);
	byte[] msg = new byte[8 + nicklen + msglen];
	msg[0] = 'F';
	msg[1] = 'L';
	msg[2] = 'Y';
	msg[3] = PACK_CHAT;
	msg[4] = msg[5] = 0; // as per spec
	msg[6] = (byte) nickname.length;
	msg[7] = (byte) message.length;
	// escape boii
	int j = 8;
	if (prefix != 0) {
		msg[j] = (byte) prefix;
		j++;
	}
	for (int i = 0; i < nicklen; i++, j++) {
		msg[j] = (byte) nickname[i];
		if (msg[j] == '%') {
			msg[j] = '#';
		}
	}
	j = 9 + nicklen;
	for (int i = 0; i < msglen; i++, j++) {
		msg[j] = (byte) message[i];
		if (msg[j] == '%') {
			msg[j] = '#';
		}
	}
	this.send(msg, msg.length);
}
}
