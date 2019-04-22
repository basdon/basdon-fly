// Copyright 2019 basdon.net - this source is licensed under GPL
// see the LICENSE file for more details
package net.basdon.fly.services.flighttracker;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.List;

import net.basdon.Log;
import net.basdon.Nullable;

public class FlightTracker
{
	public static void main(String[] args)
	{
		try (Log unused = new Log()) {
			try {
				Path path = Paths.get(".", "flighttracker.path").toAbsolutePath();
				List<String> l = Files.readAllLines(path);
				if (l.isEmpty() || !(directory = new File(l.get(0))).isDirectory()) {
					Log.error("no path in flighttracker.path is not a directory");
				} else {
					run();
				}
			} catch (IOException e) {
				Log.error("error reading flighttracker.path file", e);
			}
		} finally {
			for (FileOutputStream os : missionFiles.values()) {
				try {
					os.close();
				} catch (Throwable t) {
					Log.error("main: failed to close outputstream");
				}
			}
		}
	}

	static final byte[] buf = new byte[255];
	static final HashMap<Integer, FileOutputStream> missionFiles = new HashMap<>(); 

	static File directory;
	static boolean exit;

	static void run()
	{
		for (;;) {
			try (DatagramSocket socket = new DatagramSocket(7766)) {
				Log.info("we have a socket");
				while (!exit) {
					DatagramPacket packet = new DatagramPacket(buf, buf.length);
					socket.receive(packet);
					try {
						handle(packet);
					} catch (Throwable e) {
						Log.error("run: data handling issue", e);
					}
				}
			} catch (SocketException e) {
				Log.error("run: socket issue, restarting socket", e);
			} catch (IOException e) {
				Log.error("run: data receive issue, restarting socket", e);
			}
			if (exit) {
				break;
			}
			try {
				Thread.sleep(3000);
			} catch (InterruptedException e) {}
		}
	}

	static void handle(DatagramPacket packet)
	{
		InetAddress addr = packet.getAddress();
		if (!addr.isLoopbackAddress()) {
			Log.warn("received packet from non-loopback: " + addr.getHostAddress());
			return;
		}
		int length = packet.getLength();
		if (length < 4) {
			Log.warn("packet len too short: " + length);
			return;
		}
		if (buf[0] != 'F' || buf[1] != 'L' || buf[2] != 'Y') {
			Log.warn(
				"unknown packet, magic is: "
				+ String.format("%02X %02X %02X", buf[0], buf[1], buf[2])
			);
			return;
		}
		switch (buf[3]) {
		case 1: handleMissionStart(length); return;
		case 2: handleMissionData(length); return;
		case 3: handleMissionEnd(length); return;
		default:
			Log.warn("unknown packet type: " + Integer.toHexString(buf[3]));
		}
	}

	static void handleMissionStart(int length)
	{
		if (length < 11) {
			Log.warn("missionstart: packet should be at least len 11, is: " + length);
			return;
		}
		FileOutputStream os = outputStreamForMission(4);
		if (os == null) {
			return;
		}
		byte nameLen = buf[8];
		if (nameLen < 1 || 24 < nameLen) {
			Log.warn("missionstart: invalid name length: " + nameLen);
			return;
		}
		try {
			FileChannel channel = os.getChannel();
			long position = channel.position();
			if (position != 5) {
				channel.position(5);
			}
			channel.write(ByteBuffer.wrap(buf, 8, nameLen + 1));
			channel.position(position);
			os.flush();
		} catch (IOException e) {
			Log.error("missionstart: failed to write", e);
		}
	}

	static void handleMissionData(int length)
	{
		if (length != 15) {
			Log.warn("missiondata: packet should len 15, is: " + length);
			return;
		}
		FileOutputStream os = outputStreamForMission(4);
		if (os == null) {
			return;
		}
		try {
			int time = (int) (System.currentTimeMillis() / 1000);
			os.write(time & 0xFF);
			os.write((time >> 8) & 0xFF);
			os.write((time >> 16) & 0xFF);
			os.write((time >> 24) & 0xFF);
			os.write(buf, 8, 7);
			os.flush();
		} catch (IOException e) {
			Log.error("missiondata: failed to write", e);
		}
	}

	static void handleMissionEnd(int length)
	{
		if (length != 8) {
			Log.warn("missionend: packet should len 8, is: " + length);
			return;
		}
		FileOutputStream os = outputStreamForMission(4);
		if (os != null) {
			missionFiles.remove(new Integer(i32(4)));
			try {
				os.close();
			} catch (IOException e) {
				Log.error("missionend: failed to close outpustream", e);
			}
		} else {
			Log.warn("missionend: no existing outputstream");
		}
	}

	@Nullable
	static FileOutputStream outputStreamForMission(int missionIdOffset)
	{
		int id = i32(missionIdOffset);
		Integer key = new Integer(id);
		FileOutputStream os = missionFiles.get(key);
		if (os == null) {
			File file = new File(directory, id + ".flight");
			if (file.exists()) {
				Log.warn("flight file already exists, not opening new outpustream");
				return null;
			}
			try {
				missionFiles.put(key, os = new FileOutputStream(file));
			} catch (FileNotFoundException e) {
				Log.error("could not open file outputstream", e);
				return null;
			}
			try {
				os.write(1);
				os.write(buf, missionIdOffset, 4);
				os.write(1);
				os.write('?');
				os.write(new byte[23]);
			} catch (IOException e) {
				Log.error("could not write file header", e);
				return null;
			}
		}
		return os;
	}

	static int i16(int pos)
	{
		return buf[pos] | (buf[pos + 1] << 8);
	}

	static int i32(int pos)
	{
		return buf[pos] | (buf[pos + 1] << 8) | (buf[pos + 2] << 16) | (buf[pos + 3] << 24);
	}
}
