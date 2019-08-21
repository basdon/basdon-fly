<?php
include('../inc/bootstrap.php');

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
	if (isset($_GET['clear'], $_GET['confirm'], $_GET['cleartime']) && $clearkey === $_GET['confirm']) {
		$t = $_GET['cleartime'];
		$t++;
		$t--;
		$t = (int) $t;
		++$db_querycount;
		$db->query('UPDATE usr SET falnw='.$t.' WHERE i='.$loggeduser->i);
		$loggeduser->falnw = $t;
		$falcleared = true;
	}
}

$__script = '_account';
include('../inc/output.php');
