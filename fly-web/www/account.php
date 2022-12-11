<?php
require '../inc/bootstrap.php';

$action = 'publicprofile';
if (isset($_GET['action']) && in_array($_GET['action'], ['publicprofile', 'fal'])) {
	$action = $_GET['action'];
}

if (!isset($loggeduser)) {
	$targetaction = $action;
	$action = 'notloggedin';
} else if ($action == 'publicprofile') {
	// flight list vars
	$id = $loggeduser->i;
	$name = $loggeduser->name;
	$paginationbaseurl = 'account.php?action=publicprofile';
} else if ($action == 'fal') {
	$clearkey = md5($__sesid . $SECRET1);
	if (isset($_GET['cleartime'], $_GET['confirm']) && $clearkey === $_GET['confirm']) {
		$t = intval($_GET['cleartime']);
		if ($t > $loggeduser->falnw) {
			++$db_querycount;
			$db->query('UPDATE usr SET falnw='.$t.',falng='.$t.' WHERE i='.$loggeduser->i);
			$loggeduser->hasfailedlogins = $t < $loggeduser->lastfal;
			$loggeduser->falnw = $t;
			$falcleared = true;
		}
	}
	$lastseen = $loggeduser->falnw;
	$page = get_page();
	$db_querycount += 3;
	$data = $db->query('SELECT COUNT(u) AS numfal, MAX(stamp) AS cleartime FROM fal WHERE u='.$loggeduser->i)->fetchAll()[0];
	$numfal = $data->numfal;
	$cleartime = $data->cleartime;
	$failedlogins = $db->query('SELECT stamp,ip,isweb FROM fal WHERE u='.$loggeduser->i.' ORDER BY stamp DESC LIMIT 50 OFFSET '.(($page - 1) * 50));
	$firstunseen = $db->query('SELECT stamp FROM fal WHERE u='.$loggeduser->i.' AND stamp>'.$lastseen.' ORDER BY stamp ASC LIMIT 1')->fetchAll();
	if (count($firstunseen)) {
		$firstunseen = $firstunseen[0]->stamp;
	}
	$pagination = simple_pagination('account.php?action=fal&amp;page=', $page, $numfal, 50);
}

function acclink($name, $target)
{
	global $action;
	if ($action == $target) {
		echo '<strong>'.$name.'</strong>';
	} else {
		echo '<a href="account.php?action='.$target.'">'.$name.'</a>';
	}
}

// required to render flight list on public profile
include '../inc/aircraftnames.php';
include '../inc/flightstatuses.php';

$__script = '_account';
require '../inc/output.php';
