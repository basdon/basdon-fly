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

$db_querycount += 3;
$heartbeat = $db->query('SELECT tstart,tlast FROM heartbeat WHERE tlast>UNIX_TIMESTAMP()-35 ORDER BY tlast DESC LIMIT 1')->fetchAll();
$lastheartbeat = $db->query('SELECT MAX(tlast) tlast FROM heartbeat')->fetchAll();
$lastsessions = $db->query('SELECT tstart,tlast,cleanexit FROM heartbeat ORDER BY id DESC LIMIT 5')->fetchAll();

include('../inc/output.php');
