// Copyright 2019 basdon.net - this source is licensed under GPL
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
private static final Properties defaultprops;

static
{
	defaultprops = new Properties();
	defaultprops.setProperty(OPT_FDR_PATH, "/path/to/fdr");
}

private IAnna anna;
private char[] outtarget;
private String fdr_path;
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

	o.print(" running: " + (ft.isAlive() ? "yes\n" : "no\n"));
	o.print(" uncaught exceptions: " + ft.data_handle_issue_count);
	Throwable t = ft.last_handle_issue;
	o.print(", last: " + (t == null ? "N/A" : t.getClass().getSimpleName()) + " @ ");
	o.print((ft.last_issue == 0 ? "N/A" : format_time(ft.last_issue)) + "\n");
	t = ft.last_io_issue;
	o.print(" i/o exceptions: " + ft.io_issue_count);
	o.print(", last: " + (t == null ? "N/A" : t.getClass().getSimpleName()));
	o.print(": " + t.getMessage() + "\n");
	int c =
		ft.data_handle_issue_count +
		ft.invalid_packet_count +
		ft.invalid_packet_type_count +
		ft.mission_file_already_existed_count +
		ft.packet_wrong_length_count;
	o.print(" other issues count: " + c + "\n");
	Integer[] m = ft.activeMissionIDs();
	o.print(" active flights: " + m.length + "\n");
	if (m.length > 0) {
		o.print("  ");
		o.print(String.valueOf(m[0].intValue()));
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
	File fdrdir;
	if (!anna.load_mod_conf(this) ||
		defaultprops.getProperty(OPT_FDR_PATH).equals(this.fdr_path) ||
		(!(fdrdir = new File(this.fdr_path)).exists() && !fdrdir.mkdirs()))
	{
		anna.privmsg(replytarget, "mod_bas_ft: check fdr.path in config".toCharArray());
		return false;
	}

	this.anna = anna;
	this.outtarget = replytarget;
	this.ft = new FlightTracker(this);
	this.ft.run();
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
		try {
			ft.join(5000);
		} catch (InterruptedException e) {
			e.printStackTrace();
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
