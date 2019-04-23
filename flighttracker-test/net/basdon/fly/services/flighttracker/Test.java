// Copyright 2019 basdon.net - this source is licensed under GPL
// see the LICENSE file for more details
package net.basdon.fly.services.flighttracker;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.UnknownHostException;

import static java.lang.Thread.sleep;

public class Test
{
	static byte[] buf = new byte[256];
	static InetAddress address;

	static {
		try {
			address = InetAddress.getByAddress(new byte[] { 127, 0, 0, 1});
		} catch (UnknownHostException e) {
			e.printStackTrace();
		}
	}

	public static void main(String[] args) throws Exception
	{
		try (DatagramSocket socket = new DatagramSocket()) {
			buf[0] = 'F';
			buf[1] = 'L';
			buf[2] = 'Y';

			socket.send(start(4, "Name"));
			sleep(1000);
			socket.send(data(4, (short) 20, (short) 30, (byte) 40, (short) 50));
			sleep(1000);
			socket.send(data(4, (short) 60, (short) -20, (byte) 80, (short) 90));
			sleep(1000);
			socket.send(end(4));
			sleep(1000);

			socket.send(data(6, (short) 20, (short) 30, (byte) 40, (short) 50));
			sleep(1000);
			socket.send(start(6, "Hei"));
			sleep(1000);
			socket.send(end(6));
			sleep(1000);

			socket.send(data(7, (short) 20, (short) 30, (byte) 40, (short) 50));
			sleep(1000);
			socket.send(end(7));
			sleep(1000);
			socket.send(data(7, (short) 20, (short) 30, (byte) 40, (short) 50));
			sleep(1000);

			socket.send(start(8, "boo"));
			sleep(1000);
			socket.send(end(8));
			sleep(1000);

			socket.close();
		}
	}

	static DatagramPacket start(int missionid, String name)
	{
		DatagramPacket packet = new DatagramPacket(buf, buf.length, address, 7766);
		buf[3] = 1;
		i32ln(missionid, 4);
		buf[12] = (byte) name.length();
		char[] c = name.toCharArray();
		for (int i = 0; i < c.length; i++) {
			buf[13 + i] = (byte) c[i];
		}
		packet.setLength(14 + c.length);
		return packet;
	}

	static DatagramPacket data(int missionid, short spd, short alt, byte sat, short hp)
	{
		DatagramPacket packet = new DatagramPacket(buf, buf.length, address, 7766);
		buf[3] = 2;
		i32ln(missionid, 4);
		i16ln(spd, 12);
		i16ln(alt, 16);
		i8ln(sat, 20);
		i16ln(hp, 22);
		packet.setLength(26);
		return packet;
	}

	static DatagramPacket end(int missionid)
	{
		DatagramPacket packet = new DatagramPacket(buf, buf.length, address, 7766);
		buf[3] = 3;
		i32ln(missionid, 4);
		packet.setLength(12);
		return packet;
	}

	static void i32ln(int i, int pos)
	{
		buf[pos++] = (byte) (0x40 | (i & 0xF));
		buf[pos++] = (byte) (0x40 | ((i >> 4) & 0xF));
		buf[pos++] = (byte) (0x40 | ((i >> 8) & 0xF));
		buf[pos++] = (byte) (0x40 | ((i >> 12) & 0xF));
		buf[pos++] = (byte) (0x40 | ((i >> 16) & 0xF));
		buf[pos++] = (byte) (0x40 | ((i >> 20) & 0xF));
		buf[pos++] = (byte) (0x40 | ((i >> 24) & 0xF));
		buf[pos] = (byte) (0x40 | ((i >> 28) & 0xF));
	}

	static void i16ln(short i, int pos)
	{
		buf[pos++] = (byte) (0x40 | (i & 0xF));
		buf[pos++] = (byte) (0x40 | ((i >> 4) & 0xF));
		buf[pos++] = (byte) (0x40 | ((i >> 8) & 0xF));
		buf[pos] = (byte) (0x40 | ((i >> 12) & 0xF));
	}

	static void i8ln(byte i, int pos)
	{
		buf[pos++] = (byte) (0x40 | (i & 0xF));
		buf[pos] = (byte) (0x40 | ((i >> 4) & 0xF));
	}
}
