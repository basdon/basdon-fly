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
		i32(missionid, 4);
		buf[8] = (byte) name.length();
		char[] c = name.toCharArray();
		for (int i = 0; i < c.length; i++) {
			buf[9 + i] = (byte) c[i];
		}
		packet.setLength(10 + c.length);
		return packet;
	}

	static DatagramPacket data(int missionid, short spd, short alt, byte sat, short hp)
	{
		DatagramPacket packet = new DatagramPacket(buf, buf.length, address, 7766);
		buf[3] = 2;
		i32(missionid, 4);
		i16(spd, 8);
		i16(alt, 10);
		buf[12] = sat;
		i16(hp, 13);
		packet.setLength(15);
		return packet;
	}

	static DatagramPacket end(int missionid)
	{
		DatagramPacket packet = new DatagramPacket(buf, buf.length, address, 7766);
		buf[3] = 3;
		i32(missionid, 4);
		packet.setLength(8);
		return packet;
	}

	static void i32(int i, int pos)
	{
		buf[pos++] = (byte) (i & 0xFF);
		buf[pos++] = (byte) ((i >> 8) & 0xFF);
		buf[pos++] = (byte) ((i >> 16) & 0xFF);
		buf[pos] = (byte) ((i >> 24) & 0xFF);
	}

	static void i16(short i, int pos)
	{
		buf[pos++] = (byte) (i & 0xFF);
		buf[pos] = (byte) ((i >> 8) & 0xFF);
	}
}
