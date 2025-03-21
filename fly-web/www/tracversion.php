<?php
require '../inc/bootstrap.php';
require '../inc/trac_constants.php';

function trac_releasetime($t)
{
	return date('YmdHis', $t);
}

if (group_is_owner($usergroups) && isset($_GET['rel'], $_GET['dorelease'])) {
	$rel = intval($_GET['rel']);
	++$db_querycount;
	if ($db->query('SELECT COUNT(id) c FROM tract WHERE released='.$rel)->fetchAll()[0]->c) {
		$__msgs[] = 'Duplicate release, aborting!';
		goto skiprelease;
	}
	++$db_querycount;
	$tickets = $db->query('SELECT id FROM tract WHERE state=3')->fetchAll();
	if (!count($tickets)) {
		$__msgs[] = 'No tickets, aborting!';
		goto skiprelease;
	}

	// THIS MUST BE IN SYNC WITH tracview.php WHEN UPDATING!

	$ticketids = [];
	foreach ($tickets as $t) {
		$ticketids[] = $t->id;
	}
	++$db_querycount;
	$db->query('UPDATE tract SET updated='.$rel.',released='.$rel.',state=4 WHERE id IN ('.implode(',', $ticketids).')');

	$comment = 'status: <span class="state3">'.$trac_states[3].'</span> => <span class="state4">'.$trac_states[4].'</span>';
	$comment .= '<br/>release: <a href="tracversion.php?rel='.$rel.'">'.trac_releasetime($rel).'</a>';
	$stmt = $db->prepare('INSERT INTO tracc(parent,usr,ip,stamp,type,comment) VALUES(?,'.$loggeduser->i.',?,'.$rel.',1,?)');
	$stmt->bindValue(2, $__clientip);
	$stmt->bindValue(3, $comment);
	foreach ($tickets as $t) {
		++$db_querycount;
		$stmt->bindValue(1, $t->id);
		$stmt->execute();
	}

	include '../inc/echo.php';
	$gamemsg = 'TRAC: '.$loggeduser->name.' released version '.trac_releasetime($rel).', closing '.count($tickets).' tickets';
	$ircmsg = $gamemsg.' -> '.$ABS_URL.'/tracversion.php?rel='.$rel;
	echo_send_message_irc_game(1, $ircmsg, $gamemsg);
} skiprelease:

$releaseids = [];
$mapping = [];
if (isset($_GET['rel'])) { // rel = timestamp of release
	$selectedrelease = intval($_GET['rel']);
haverel:
	$releaseids[] = $selectedrelease;
	$release_repo = $selectedrelease < 1609459200 ? 'fly-plugin' : 'basdon-fly';
} else if (isset($_GET['release'])) { // release = full release number (linked from github releases etc)
	$strtime = $_GET['release'];
	if (strlen($strtime) == 14) {
		$year = intval(substr($strtime, 0, 4));
		$month = intval(substr($strtime, 4, 2));
		$day = intval(substr($strtime, 6, 2));
		$hour = intval(substr($strtime, 8, 2));
		$minute = intval(substr($strtime, 10, 2));
		$second = intval(substr($strtime, 12, 2));
		$selectedrelease = mktime($hour, $minute, $second, $month, $day, $year);
		goto haverel;
	}
} else {
	$page = get_page();
	$db_querycount += 2;
	$releasecount = $db->query('SELECT COUNT(DISTINCT released) c FROM tract')->fetchAll()[0]->c; // NULL is not included in this count
	$releases = $db->query('SELECT DISTINCT released FROM tract ORDER BY released DESC LIMIT 10 OFFSET '.(($page - 1) * 10))->fetchAll();

	foreach ($releases as $r) {
		if ($r->released != null) {
			$releaseids[] = $r->released;
			$mapping[$r->released] = [];
		}
	}

	$readytickets = $db->query('SELECT id,severity,state,stamp,updated,summary FROM tract WHERE state=3 AND (visibility&'.$usergroups.' OR op='.$userid.') ORDER BY severity DESC')->fetchAll();
}

if (count($releaseids)) {
	++$db_querycount;
	foreach ($db->query('SELECT id,released,severity,state,stamp,updated,summary FROM tract WHERE (visibility&'.$usergroups.' OR op='.$userid.') AND released IN ('.implode(',', $releaseids).') ORDER BY released DESC, severity DESC') as $r)
	{
		$mapping[$r->released][] = $r;
	}
}

$__template = '_tracversion';
require '../inc/output.php';
