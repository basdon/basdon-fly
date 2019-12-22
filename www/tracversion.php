<?php
include('../inc/bootstrap.php');
include('../inc/trac_constants.php');

function trac_releasetime($t)
{
	return date('YmdHis', $t);
}

if (group_is_owner($usergroups) && isset($_GET['rel'], $_GET['dorelease'])) {
	$rel = intval($_GET['rel']);
	if ($db->query('SELECT COUNT(id) c FROM tract WHERE released='.$rel)->fetchAll()[0]->c) {
		$__msgs[] = 'Duplicate release!';
		goto skiprelease;
	}
	if (!$db->query('SELECT COUNT(id) c FROM tract WHERE status=3')->fetchAll()[0]->c) {
		$__msgs[] = 'No tickets!';
		goto skiprelease;
	}

	// THIS MUST BE IN SYNC WITH tracview.php WHEN UPDATING!

	++$db_querycount;
	$tickets = $db->query('SELECT id FROM tract WHERE status=3')->fetchAll();

	$stmt = $db->prepare('UPDATE tract SET updated='.$rel.',released='.$rel.',status=4 WHERE id=?');
	foreach ($tickets as $t) {
		++$db_querycount;
		$stmt->bindValue(1, $t->id);
		$stmt->execute();
	}

	$stmt = $db->prepare('INSERT INTO tracc(parent,usr,ip,stamp,type,comment) VALUES(?,'.$loggeduser->i.',?,'.$rel.',1,?)');
	$stmt->bindValue(2, $__clientip);
	$stmt->bindValue(3, 'status: <span class="status3">'.$trac_statuses[3].'</span> => <span class="status4">'.$trac_statuses[4].'</span>');
	foreach ($tickets as $t) {
		++$db_querycount;
		$stmt->bindValue(1, $t->id);
		$stmt->execute();
	}

	include('../inc/echo.php');
	$gamemsg = 'TRAC: '.$loggeduser->name.' released version '.trac_releasetime($rel).', closing '.count($tickets).' tickets';
	$ircmsg = $gamemsg.' -> '.$BASEPATH.'/tracversion.php?rel='.$rel;
	echo_send_message_irc_game(1, $ircmsg, $gamemsg);
} skiprelease:

$page = get_page();
$db_querycount += 2;
$releasecount = $db->query('SELECT COUNT(DISTINCT released) c FROM tract')->fetchAll()[0]->c; // NULL is not included in this count
$releases = $db->query('SELECT DISTINCT released FROM tract ORDER BY released DESC LIMIT 10 OFFSET '.(($page - 1) * 10))->fetchAll();

$releaseids = [];
$mapping = [];
foreach ($releases as $r) {
	if ($r->released != null) {
		$releaseids[] = $r->released;
		$mapping[$r->released] = [];
	}
}
// TODO: only use release from given rel GET param when present

if (count($releaseids)) {
	++$db_querycount;
	foreach ($db->query('SELECT id,released,severity,updated,summary FROM tract WHERE (visibility&'.$usergroups.' OR op='.$userid.') AND released IN ('.implode(',', $releaseids).') ORDER BY released DESC, severity DESC') as $r)
	{
		$mapping[$r->released][] = $r;
	}
}

$readytickets = $db->query('SELECT id,severity,updated,summary FROM tract WHERE status=3 AND (visibility&'.$usergroups.' OR op='.$userid.') ORDER BY severity DESC')->fetchAll();

$__script = '_tracversion';
include('../inc/output.php');
