<?php
include('../inc/bootstrap.php');

if (!isset($loggeduser) || !group_is_admin($loggeduser->groups)) {
	$ret = 'admin.php';
	$__script = '_unauthorized';
} else {
	$__script = '_admin';
}

++$db_querycount;
$healthchecks = $db->query('SELECT
	(SELECT COUNT(i) FROM usr WHERE playtime>onlinetime) playtime,
	(SELECT COUNT(i) FROM usr WHERE flighttime>playtime) flighttime,
	(SELECT COUNT(i) FROM usr WHERE distance>10000000) distance')->fetchAll()[0];

function acclink($name, $target)
{
	global $action, $BASEPATH;
	if ($action == $target) {
		echo '<strong>'.$name.'</strong>';
	} else {
		echo '<a href="account.php?action='.$target.'">'.$name.'</a>';
	}
}

include('../inc/output.php');
