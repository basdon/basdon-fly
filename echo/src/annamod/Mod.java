// Copyright 2019 basdon.net - this source is licensed under GPL
// see the LICENSE file for more details
package annamod;

import java.io.IOException;
import java.util.Properties;

import net.basdon.anna.api.Config;
import net.basdon.anna.api.IAnna;
import net.basdon.anna.api.IAnna.Output;
import net.basdon.anna.api.IMod;
import net.basdon.anna.api.User;
import net.basdon.fly.services.echo.Echo;

import static net.basdon.anna.api.Util.*;

public class Mod implements IMod
{
private static final String OPT_ECHO_CHAN = "echo.channel";
private static final Properties defaultprops;

static
{
	defaultprops = new Properties();
	defaultprops.setProperty(OPT_ECHO_CHAN, "#basdon.echo");
}

private IAnna anna;
private Echo echo;
private char[] outtarget;

@Override
public
String getName()
{
	return "mod_bas_echo";
}

@Override
public
String getVersion()
{
	return "1a";
}

@Override
public
String getDescription()
{
	return "echo service for basdon.net aviation server";
}

@Override
public
void print_stats(Output output)
throws IOException
{
}

@Override
public
boolean on_enable(IAnna anna, char[] replytarget)
{
	this.anna = anna;
	this.outtarget = replytarget;
	anna.load_mod_conf(this);
	return true;
}

@Override
public Properties get_default_conf()
{
	return defaultprops;
}

@Override
public
void config_loaded(Config conf)
{
	String echochan = conf.getStr(OPT_ECHO_CHAN);
	if (echochan == null) {
		anna.privmsg(this.outtarget, "failed to load echo channel property".toCharArray());
		return;
	}
	this.outtarget = echochan.toCharArray();
	echo = new Echo(anna, this.outtarget);
	echo.start();
}

@Override
public
void on_disable()
{
	if (echo != null) {
		echo.interrupt();
		echo.insocket.close();
		echo.outsocket.close();
		try {
			echo.join(5000);
		} catch (InterruptedException ignored) {}
		if (echo.isAlive()) {
			anna.log_error("echo thread is still active!");
			anna.privmsg(outtarget, "echo thread is still active!".toCharArray());
		}
		echo = null;
	}
}

@Override
public void on_message(User user, char[] target, char[] replytarget, char[] message)
{
	if (this.echo != null && user != null && strcmp(this.outtarget, target)) {
		this.echo.send_chat_to_game(user.nick, message);
	}
}
}
