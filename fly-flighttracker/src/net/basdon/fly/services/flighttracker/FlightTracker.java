// Copyright 2019-2022 basdon.net - this source is licensed under AGPL
// see the LICENSE file for more details
package net.basdon.fly.services.flighttracker;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InterruptedIOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.util.HashMap;
import java.util.function.Consumer;

public class FlightTracker extends Thread
{
private final File fdr_directory;
private final Consumer<String> logreceiver;

byte[] buf = new byte[255];
final HashMap<Integer, FileOutputStream> missionFiles = new HashMap<>();

public boolean debug;
public DatagramSocket socket;
public int data_handle_err_count;
public Throwable last_handle_err;
public int invalid_packet_count;
public int invalid_packet_type_count;
public int packet_wrong_length_count;
public int handle_issue_count;
public int io_issue_count;
public IOException last_io_issue;
public long last_io_issue_time;
public long last_handle_err_time;
public long last_issue_time;
public long last_packet;
public int packets_received;
public int bytes_received;
public long boot;

public
FlightTracker(File fdr_directory, Consumer<String> logreceiver)
{
	this.logreceiver = logreceiver;
	this.fdr_directory = fdr_directory;
	this.boot = System.currentTimeMillis();
}

@Override
public
void run()
{
	try {
		for (;;) {
			try (DatagramSocket socket = new DatagramSocket(7766)) {
				this.socket = socket;
				for (;;) {
					DatagramPacket packet = new DatagramPacket(buf, buf.length);
					socket.receive(packet);
					packets_received++;
					bytes_received += packet.getLength();
					last_packet = System.currentTimeMillis();
					try {
						if (!handle(packet)) {
							handle_issue_count++;
							last_issue_time = System.currentTimeMillis();
						}
					} catch (InterruptedIOException e) {
						return;
					} catch (Throwable e) {
						last_handle_err = e;
						data_handle_err_count++;
						last_handle_err_time = last_packet;
					}
				}
			} catch (SocketException e) {
				if (this.isInterrupted()) {
					return;
				}
				logreceiver.accept("mod_bas_ft: socket issue, restarting socket");
			} catch (InterruptedIOException e) {
				return;
			} catch (IOException e) {
				logreceiver.accept("mod_bas_ft: data receive issue, restarting socket");
			}
			Thread.sleep(3000);
		}
	} catch (InterruptedException e) {}
}

/**
 * @return array with mission ids that are currently active
 */
public
Integer[] activeMissionIDs()
{
	return this.missionFiles.keySet().toArray(new Integer[0]);
}

/**
 * @return {@code false} if an IOException occurs while closing any of the open streams
 */
public
boolean closeAllFiles()
{
	boolean res = true;
	for (FileOutputStream os : missionFiles.values()) {
		try {
			os.close();
		} catch (Throwable t) {
			logreceiver.accept("mod_bas_ft: failed to close outputstream");
			res = false;
		}
	}
	missionFiles.clear();
	return res;
}

/**
 * @return {@code false} if the packet was invalid or an IOException occurred
 */
boolean handle(DatagramPacket packet)
throws InterruptedIOException
{
	InetAddress a = packet.getAddress();
	if (!a.isLoopbackAddress()) {
		logreceiver.accept("mod_bas_ft: got packet from non-loopback: " + a.getHostAddress());
		return true; // don't mark as issue, just ignore
	}
	int length = packet.getLength();
	if (debug) {
		for (int i = 0; i < length; i++) {
			System.out.printf("%02X ", buf[i]);
		}
		System.out.println();
	}
	if (length < 4) {
		packet_wrong_length_count++;
		return false;
	}
	if (buf[0] != 'F' || buf[1] != 'L' || buf[2] != 'Y') {
		invalid_packet_count++;
		return false;
	}
	switch (buf[3]) {
	case 1: return handleMissionStart(length);
	case 2: return handleMissionData(length);
	case 3: return handleMissionEndOrPause(length);
	case 4:
	case 5: closeAllFiles(); return true;
	default:
		invalid_packet_type_count++;
		last_issue_time = System.currentTimeMillis();
		return false;
	}
}

/**
 * @return {@code false} if the packet was invalid or an IOException occurred
 */
boolean handleMissionStart(int length)
throws InterruptedIOException
{
	if (length < 10) {
		packet_wrong_length_count++;
		return false;
	}
	FileOutputStream os = outputStreamForMission(4);
	if (os != null) {
		try {
			FileChannel channel = os.getChannel();
			long position = channel.position();
			if (position != 8) {
				channel.position(8);
			}
			channel.write(ByteBuffer.wrap(buf, 8, 2));
			channel.position(position);
			os.flush();
			return true;
		} catch (InterruptedIOException e) {
			throw e;
		} catch (IOException e) {
			last_io_issue = e;
			last_io_issue_time = System.currentTimeMillis();
			io_issue_count++;
		}
	}
	return false;
}

/**
 * @return {@code false} if the packet was invalid or an IOException occurred
 */
boolean handleMissionData(int length)
throws InterruptedIOException
{
	if (length != 32) {
		packet_wrong_length_count++;
		return false;
	}
	FileOutputStream os = outputStreamForMission(4);
	if (os == null) {
		return false;
	}
	try {
		int time = (int) (System.currentTimeMillis() / 1000);
		os.write(time & 0xFF);
		os.write((time >> 8) & 0xFF);
		os.write((time >> 16) & 0xFF);
		os.write((time >> 24) & 0xFF);
		os.write(buf, 8, 24);
		os.flush();
		return true;
	} catch (InterruptedIOException e) {
		throw e;
	} catch (IOException e) {
		last_io_issue = e;
		last_io_issue_time = System.currentTimeMillis();
		io_issue_count++;
	}
	return false;
}

/**
 * @return {@code false} if the packet was invalid or an IOException occurred
 */
boolean handleMissionEndOrPause(int length)
throws InterruptedIOException
{
	if (length != 8) {
		packet_wrong_length_count++;
		return false;
	}
	FileOutputStream os = outputStreamForMission(4);
	if (os != null) {
		missionFiles.remove(Integer.valueOf(i32(4)));
		try {
			os.close();
			return true;
		} catch (InterruptedIOException e) {
			throw e;
		} catch (IOException e) {
			last_io_issue = e;
			last_io_issue_time = System.currentTimeMillis();
			io_issue_count++;
		}
	}
	return false;
}

/**
 * @return an open outputstream or {@code null} when it either already existed or could not be opened
 */
FileOutputStream outputStreamForMission(int missionIdOffset)
throws InterruptedIOException
{
	int id = i32(missionIdOffset);
	Integer key = Integer.valueOf(id);
	FileOutputStream os = missionFiles.get(key);
	if (os == null) {
		File file = new File(fdr_directory, id + ".flight");
		boolean exists = file.exists();
		try {
			/**
			 * Only using 'append' mode if the file already exists, because the filechannel's
			 * position can't be set when it's opened in append mode.
			 * We need to set the filechannel's position when the flight starts, to receive
			 * the flight start packet and overwrite the existing header data.
			 * When continuing a flight, this is not needed and append mode is required
			 * because we need to append data and that's not possible by simply changing
			 * the channel's position (maybe with reading and writing the existing file
			 * contents, but that sounds stupid).
			 */
			missionFiles.put(key, os = new FileOutputStream(file, exists));
		} catch (FileNotFoundException e) {
			io_issue_count++;
			return null;
		}
		if (!exists) {
			try {
				os.write(6);
				os.write('F');
				os.write('L');
				os.write('Y');
				os.write(buf, missionIdOffset, 4);
				os.write(new byte[2]); // flags
				os.flush();
			} catch (InterruptedIOException e) {
				throw e;
			} catch (IOException e) {
				last_io_issue = e;
				last_io_issue_time = System.currentTimeMillis();
				io_issue_count++;
				return null;
			}
		}
	}
	return os;
}

int i32(int pos)
{
	return
		(buf[pos] & 0xFF) |
		((buf[pos + 1] & 0xFF) << 8) |
		((buf[pos + 2] & 0xFF) << 16) |
		((buf[pos + 3] & 0xFF) << 24);
}
} /*FlightTracker*/
