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
			closeAllFiles();
		}
	}

	static final byte[] buf = new byte[255];
	static final HashMap<Integer, FileOutputStream> missionFiles = new HashMap<>(); 

	static File directory;
	static boolean exit;

	static void closeAllFiles()
	{
		for (FileOutputStream os : missionFiles.values()) {
			try {
				os.close();
			} catch (Throwable t) {
				Log.error("main: failed to close outputstream");
			}
		}
		missionFiles.clear();
	}

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
		case 4:
		case 5: closeAllFiles(); return;
		default:
			Log.warn("unknown packet type: " + Integer.toHexString(buf[3]));
		}
	}

	static void handleMissionStart(int length)
	{
		if (length < 14) {
			Log.warn("missionstart: packet should be at least len 14, is: " + length);
			return;
		}
		FileOutputStream os = outputStreamForMission(4);
		if (os == null) {
			return;
		}
		byte nameLen = buf[12];
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
			channel.write(ByteBuffer.wrap(buf, 12, nameLen + 1));
			channel.position(position);
			os.flush();
		} catch (IOException e) {
			Log.error("missionstart: failed to write", e);
		}
	}

	static void handleMissionData(int length)
	{
		if (length != 26) {
			Log.warn("missiondata: packet should len 26, is: " + length);
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
			for (int i = 0; i < 7; i++) {
				buf[12 + i] = (byte)
					((buf[12 + 2 * i] & 0xF) |
					((buf[13 + 2 * i] & 0xF) << 4));
			}
			os.write(buf, 12, 7);
			os.flush();
		} catch (IOException e) {
			Log.error("missiondata: failed to write", e);
		}
	}

	static void handleMissionEnd(int length)
	{
		if (length != 12) {
			Log.warn("missionend: packet should len 12, is: " + length);
			return;
		}
		FileOutputStream os = outputStreamForMission(4);
		if (os != null) {
			missionFiles.remove(new Integer(i32ln(4)));
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
		int id = i32ln(missionIdOffset);
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
				os.write(id & 0xFF);
				os.write((id >> 8) & 0xFF);
				os.write((id >> 16) & 0xFF);
				os.write((id >> 24) & 0xFF);
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

	static short i16ln(int pos)
	{
		return (short)
			((buf[pos] & 0xF) |
			((buf[pos + 1] & 0xF) << 4) |
			((buf[pos + 2] & 0xF) << 8) |
			((buf[pos + 3] & 0xF) << 12));
	}

	static int i32ln(int pos)
	{
		return
			(buf[pos] & 0xF) |
			((buf[pos + 1] & 0xF) << 4) |
			((buf[pos + 2] & 0xF) << 8) |
			((buf[pos + 3] & 0xF) << 12) |
			((buf[pos + 4] & 0xF) << 16) |
			((buf[pos + 5] & 0xF) << 20) |
			((buf[pos + 6] & 0xF) << 24) |
			((buf[pos + 7] & 0xF) << 28);
	}
}
