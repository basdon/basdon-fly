// Copyright 2019 basdon.net - this source is licensed under AGPL
// see the LICENSE file for more details
package annamod;

import java.io.File;
import java.io.IOException;
import java.util.Properties;
import java.util.function.Consumer;

import net.basdon.anna.api.*;
import net.basdon.anna.api.IAnna.Output;
import net.basdon.fly.services.flighttracker.FlightTracker;

import static net.basdon.anna.api.Util.*;

public class Mod implements IMod, Consumer<String>
{
private static final String OPT_FDR_PATH = "fdr.path";
private static final String OPT_MSG_CHAN = "critical.output.channel";
private static final Properties defaultprops;

static
{
	defaultprops = new Properties();
	defaultprops.setProperty(OPT_FDR_PATH, "/path/to/fdr");
	defaultprops.setProperty(OPT_MSG_CHAN, "");
}

private IAnna anna;
private char[] outtarget;
private String fdr_path;
private File fdr_dir;
private FlightTracker ft;

@Override
public
String getName()
{
	return "mod_bas_ft";
}

@Override
public
String getVersion()
{
	return "1";
}

@Override
public
String getDescription()
{
	return "flight tracker for basdon.net aviation server";
}

@Override
public
void print_stats(Output o)
throws IOException
{
	if (ft == null) {
		o.print(" thread is null!!");
		return;
	}

	o.print(" boot: " + format_time(ft.boot) + "\n");
	o.print(" running: " + (ft.isAlive() ? "yes\n" : "no\n"));
	o.print(" packets recv: " + ft.packets_received + ", bytes: " + ft.bytes_received + "\n");
	o.print(" last packet: " + (ft.last_packet == 0 ? "N/A" : format_time(ft.last_packet)) + "\n");
	o.print(" uncaught exceptions: " + ft.data_handle_err_count);
	Throwable t = ft.last_handle_err;
	o.print(", last: " + (t == null ? "N/A" : t.getClass().getSimpleName()) + " @ ");
	o.print((ft.last_handle_err_time == 0 ? "N/A" : format_time(ft.last_handle_err_time)) + "\n");
	t = ft.last_io_issue;
	o.print(" i/o exceptions: " + ft.io_issue_count);
	o.print(", last: " + (t == null ? "N/A" : t.getClass().getSimpleName()));
	o.print(": " + (t == null ? "N/A" : t.getMessage()) + " @ ");
	o.print((ft.last_io_issue_time == 0 ? "N/A" : format_time(ft.last_io_issue_time)) + "\n");
	int c =
		ft.handle_issue_count +
		ft.invalid_packet_count +
		ft.invalid_packet_type_count +
		ft.mission_file_already_existed_count +
		ft.packet_wrong_length_count;
	o.print(" other issues count: " + c + ", last: ");
	o.print((ft.last_issue_time == 0 ? "N/A" : format_time(ft.last_issue_time)) + "\n");
	Integer[] m = ft.activeMissionIDs();
	o.print(" active flights: " + m.length + "\n");
	if (m.length > 0) {
		o.print("  ");
		o.print(m[0].toString());
		for (int i = 1; i < m.length; i++) {
			o.print(", ");
			o.print(String.valueOf(m[i].intValue()));
		}
		o.print("\n");
	}
}

@Override
public
boolean on_enable(IAnna anna, char[] replytarget)
{
	if (!anna.load_mod_conf(this) ||
		defaultprops.getProperty(OPT_FDR_PATH).equals(this.fdr_path) ||
		(!(this.fdr_dir = new File(this.fdr_path)).isDirectory() && !this.fdr_dir.mkdirs()))
	{
		anna.privmsg(replytarget, "mod_bas_ft: check fdr.path in config".toCharArray());
		return false;
	}

	this.anna = anna;
	this.outtarget = replytarget;
	this.ft = new FlightTracker(this.fdr_dir, this);
	this.ft.start();
	return true;
}

@Override
public Properties get_default_conf()
{
	return defaultprops;
}

@Override
public
void on_disable()
{
	if (ft != null) {
		ft.interrupt();
		ft.socket.close();
		try {
			ft.join(5000);
		} catch (InterruptedException ignored) {}
		if (ft.isAlive()) {
			anna.log_error("flighttracker is still active!");
			anna.privmsg(outtarget, "flighttracker is still active!".toCharArray());
		}
		ft.closeAllFiles();
		ft = null;
	}
}

@Override
public
void config_loaded(Config conf)
{
	this.fdr_path = conf.getStr(OPT_FDR_PATH);
	String msgchan = conf.getStr(OPT_MSG_CHAN);
	if (msgchan != null && !msgchan.isEmpty()) {
		this.outtarget = msgchan.toCharArray();
	}
}

@Override
public
boolean on_command(User user, char[] target, char[] replytarget,
                   char[] message, char[] cmd, char[] params)
{
	if (strcmp(cmd, 'f','t')) {
		if (ft == null || !ft.isAlive()) {
			anna.privmsg(replytarget, "flighttracker is dead, rebooting".toCharArray());
			this.ft = new FlightTracker(this.fdr_dir, this);
			this.ft.start();
		}
		StringBuilder sb = new StringBuilder("flighttracker is ");
		if (this.ft.isAlive()) {
			sb.append("running");
		} else {
			sb.append("dead");
		}
		Integer[] ids = this.ft.activeMissionIDs();
		sb.append(", ").append(ids.length).append(" active flights");
		if (ids.length > 0) {
			sb.append(": ").append(ids[0].toString());
			for (int i = 1; i < ids.length; i++) {
				sb.append(", ").append(ids[i].toString());
			}
		}
		anna.privmsg(replytarget, chars(sb));
		return true;
	}
	if (strcmp(cmd, 'f','t','-','d','e','b','u','g') && anna.is_owner(user)) {
		if (ft != null) {
			String msg = "debug is now " + ((ft.debug = !ft.debug) ? "on" : "off");
			anna.privmsg(replytarget, msg.toCharArray());
		}
		return true;
	}
	return false;
}

@Override
public
void /*Consumer.*/accept(String t)
{
	this.anna.sync_exec(() -> {
		this.anna.privmsg(outtarget, t.toCharArray());
	});
}
} /*Mod*/
