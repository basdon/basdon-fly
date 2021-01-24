// Copyright 2019-2021 basdon.net - this source is licensed under AGPL
// see the LICENSE file for more details
package annamod;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Properties;

import net.basdon.anna.api.*;
import net.basdon.anna.api.IAnna.Output;
import net.basdon.fly.services.echo.Echo;

import static java.lang.System.arraycopy;

import static net.basdon.anna.api.Constants.*;
import static net.basdon.anna.api.Util.*;
import static net.basdon.fly.services.echo.Echo.*;

public
class Mod implements IMod
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
private ArrayList<char[]> people_without_permission = new ArrayList<>();

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
	return "2";
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
	this.anna.join(this.outtarget);
	if (this.echo == null) {
		this.echo = new Echo(this.anna, this.outtarget);
		this.echo.start();
	}
}

@Override
public
void on_disable()
{
	if (echo != null) {
		echo.interrupt();
		echo.insocket.close();
		try {
			echo.join(5000);
		} catch (InterruptedException ignored) {}
		if (echo.isAlive()) {
			anna.log_error("echo thread is still active!");
			anna.privmsg(outtarget, "echo thread is still active!".toCharArray());
		}
		echo.outsocket.close();
		echo = null;
	}
}

@Override
public
boolean on_command(User user, char[] target, char[] replytarget,
                   char[] msg, char[] cmd, char[] params)
{
	if (!strcmp(this.outtarget, target)) {
		return false;
	}

	boolean may_send = false;
	ChannelUser cu = this.anna.find_user(target, user.nick);
	if (this.echo != null && cu != null) {
		may_send = has_user_mode_or_higher(cu, this.anna.get_user_channel_modes(), 'v');
		if (may_send) {
			this.echo.send_chat_or_action_to_game(
				PACK_CHAT, cu.prefix, user.nick, msg, 0, msg.length);
		}
	}
	if (strcmp(this.outtarget, target)) {
		if (strcmp(cmd, 'e','c','h','o')) {
			if (this.echo != null) {
				this.anna.privmsg(target, "echo is listening".toCharArray());
			} else {
				this.anna.privmsg(target, "echo is not running".toCharArray());
			}
			return true;
		}
		if (strcmp(cmd, 'p','l','a','y','e','r','s')) {
			if (this.echo != null) {
				if (this.echo.is_game_down) {
					String downmsg = CTRL_COLOR + SCOL_RED + "Game is down!";
					this.anna.privmsg(target, downmsg.toCharArray());
				} else {
					this.echo.request_global_status(may_send);
				}
			} else {
				this.anna.privmsg(target, "echo is not running".toCharArray());
			}
			return true;
		}
	}
	return false;
}

@Override
public
void on_topic(User user, char[] channel, char[] topic)
{
	if (this.echo != null && strcmp(channel, this.outtarget) &&
		this.anna.find_channel(channel) != null)
	{
		StringBuilder s = new StringBuilder(144);
		s.append("IRC: * ");
		if (user != null) {
			s.append(user.nick);
			s.append(" changes channel topic to: ");
		} else {
			s.append("channel topic was changed to: ");
		}
		s.append(topic);
		this.echo.send_generic_message(chars(s), Echo.PACK12_IRC_TOPIC);
	}
}

@Override
public
void on_channelmodechange(Channel chan, User user, int changec, char[] signs,
                                 char[] modes, char[] types, char[][] params, ChannelUser[] users)
{
	if (this.echo != null && strcmp(chan.name, this.outtarget)) {
		StringBuilder s = new StringBuilder(144);
		s.append("IRC: * ");
		if (user != null) {
			s.append(user.nick);
			s.append(" sets mode ");
		} else {
			s.append("mode ");
		}
		char sign = 0;
		for (int i = 0; i < changec; i++) {
			if (sign != signs[i]) {
				s.append(sign = signs[i]);
			}
			s.append(modes[i]);
		}
		for (int i = 0; i < changec; i++) {
			if (params[i] != null) {
				s.append(' ').append(params[i]);
			}
		}
		this.echo.send_generic_message(chars(s), Echo.PACK12_IRC_MODE);

		// Reset no permission message when user gets a new mode (so the message will show
		// again when the mode has been revoked). Note that the mode itself is not checked,
		// but sending a message too much won't harm.
		if (users.length > 0) {
			for (int i = 0; i < types.length; i++) {
				if (types[i] == 'u' && signs[i] == '+') {
					for (char[] nick : this.people_without_permission) {
						if (strcmp(nick, users[i].nick)) {
							this.people_without_permission.remove(nick);
							break;
						}
					}
				}
			}
		}
	}
}

@Override
public
void on_nickchange(User user, char[] oldnick, char[] newnick)
{
	if (this.echo != null && this.anna.find_user(this.outtarget, newnick) != null) {
		this.echo.on_user_nick_changed(oldnick, newnick);
		StringBuilder s = new StringBuilder(144);
		s.append("IRC: * ");
		s.append(oldnick);
		s.append(" is now known as ");
		s.append(newnick);
		this.echo.send_generic_message(chars(s), Echo.PACK12_IRC_NICK);
	}
}

@Override
public
void on_action(User user, char[] target, char[] replytarget, char[] action)
{
	if (this.echo != null && user != null && strcmp(this.outtarget, target)) {
		ChannelUser cu = anna.find_user(target, user.nick);
		if (cu != null &&
			has_user_mode_or_higher(cu, this.anna.get_user_channel_modes(), 'v'))
		{
			this.echo.send_chat_or_action_to_game(
				PACK_ACTION, cu.prefix, user.nick,
				action, 0, action.length);
		}
	}
}

@Override
public
void on_kick(User user, char[] channel, char[] kickeduser, char[] msg)
{
	if (this.echo != null) {
		ChannelUser cu = this.anna.find_user(this.outtarget, kickeduser);
		if (cu != null) {
			this.echo.send_player_connection(cu, CONN_REASON_IRC_KICK);
		}
	}
}

@Override
public
void on_part(User user, char[] channel, char[] msg)
{
	if (strcmp(this.outtarget, channel)) {
		this.send_player_connection(user, CONN_REASON_IRC_PART);
	}
}

@Override
public
void on_quit(User user, char[] msg)
{
	this.send_player_connection(user, CONN_REASON_IRC_QUIT);
}

@Override
public
void on_join(User user, char[] channel)
{
	if (strcmp(this.outtarget, channel)) {
		if (user != null && strcmp(user.nick, this.anna.get_anna_user().nick)) {
			return;
		}
		this.send_player_connection(user, CONN_REASON_IRC_JOIN);
		// Following call must be placed after send_player_connection,
		// because it'd remove the user from the list of users to send the msg to.
		this.echo.send_status_message_to_user(user);
	}
}

private
void send_player_connection(User user, byte reason)
{
	if (user != null) {
		ChannelUser cu = this.anna.find_user(this.outtarget, user.nick);
		if (cu != null) {
			this.echo.remove_user_from_awaiting_status_message_list(user.nick);
			this.echo.send_player_connection(cu, reason);
		}
	}
}

@Override
public
void on_selfjoin(Channel channel)
{
	if (this.echo != null && strcmp(channel.name, this.outtarget)) {
		this.echo.send_initial_status_message();
	}
}

@Override
public
void on_message(User user, char[] target, char[] replytarget, char[] msg)
{
	ChannelUser cu;
	if (this.echo != null && user != null && strcmp(this.outtarget, target) &&
		(cu = this.anna.find_user(target, user.nick)) != null)
	{
		if (has_user_mode_or_higher(cu, this.anna.get_user_channel_modes(), 'v')) {
			this.echo.send_chat_or_action_to_game(
				PACK_CHAT, cu.prefix, user.nick, msg, 0, msg.length);
		} else {
			for (char[] peep : this.people_without_permission) {
				if (strcmp(peep, user.nick)) {
					return;
				}
			}
			this.people_without_permission.add(user.nick);

			String noticemsg =
				" :your message won't be relayed in-game unless you "
				+ "have a +v channel mode";
			char[] notice = new char[7 + user.nick.length + 73];
			set(notice, 0, 'N','O','T','I','C','E',' ');
			arraycopy(user.nick, 0, notice, 7, user.nick.length);
			arraycopy(noticemsg.toCharArray(), 0, notice, 7 + user.nick.length, 73);
			this.anna.send_raw(notice, 0, notice.length);
		}
	}
}

@Override
public
void on_selfmessage(char[] target, char[] text, int offset, int len)
{
	if (this.echo != null && !this.echo.ignore_self && strcmp(this.outtarget, target)) {
		User user = anna.get_anna_user();
		ChannelUser cu = anna.find_user(target, user.nick);
		char prefix = cu == null ? 0 : cu.prefix;
		this.echo.send_chat_or_action_to_game(PACK_CHAT, prefix, user.nick, text, offset, len);
	}
}

@Override
public
void on_selfaction(char[] target, char[] text, int offset, int len)
{
	if (this.echo != null && !this.echo.ignore_self) {
		User user = anna.get_anna_user();
		ChannelUser cu = anna.find_user(target, user.nick);
		char prefix = cu == null ? 0 : cu.prefix;
		this.echo.send_chat_or_action_to_game(PACK_ACTION, prefix, user.nick, text, offset, len);
	}
}
}
