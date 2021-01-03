<?php
function echo_send_message_irc_game($type, $ircmsg, $gamemsg)
{
	$gamemsglen = strlen($gamemsg);
	$ircmsglen = strlen($ircmsg);
	$gamemsg = 'FLY____' . $gamemsg;
	$ircmsg = 'FLY____' . $ircmsg;
	$gamemsg[3] = $ircmsg[3] = chr(12);
	$gamemsg[4] = $ircmsg[4] = chr($type);
	$gamemsg[5] = chr($gamemsglen & 0xFF);
	$gamemsg[6] = chr(($gamemsglen >> 8) & 0xFF);
	$ircmsg[5] = chr($ircmsglen & 0xFF);
	$ircmsg[6] = chr(($ircmsglen >> 8) & 0xFF);
	$sock = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
	socket_sendto($sock, $ircmsg, $ircmsglen + 7, 0, '127.0.0.1', 7767);
	socket_sendto($sock, $gamemsg, $gamemsglen + 7, 0, '127.0.0.1', 7768);
	socket_close($sock);
}
