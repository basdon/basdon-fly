<?php
require '../inc/bootstrap.php';

$action = 'home';
if (isset($_GET['action']) && in_array($_GET['action'], ['home', 'acl'])) {
	$action = $_GET['action'];
}

if (!isset($loggeduser) || !group_is_admin($loggeduser->groups)) {
	$ret = 'admin.php&action='.$action;
	$__script = '_unauthorized';
	require '../inc/output.php';
	exit();
}

if ($action == 'home') {
	$db_querycount += 4;
	$healthchecks = $db->query('SELECT
		(SELECT COUNT(i) FROM usr WHERE playtime>onlinetime) playtime,
		(SELECT COUNT(i) FROM usr WHERE flighttime>playtime) flighttime,
		(SELECT COUNT(i) FROM usr WHERE distance>10000000) distance')->fetchAll()[0];
	$heartbeat = $db->query('SELECT tstart,tlast FROM heartbeat WHERE tlast>UNIX_TIMESTAMP()-35 ORDER BY tlast DESC LIMIT 1')->fetchAll();
	$lastheartbeat = $db->query('SELECT MAX(tlast) tlast FROM heartbeat')->fetchAll();
	$lastsessions = $db->query('SELECT tstart,tlast,cleanexit FROM heartbeat ORDER BY id DESC LIMIT 10')->fetchAll();
} else if ($action == 'acl') {
	$page = get_page();
	$db_querycount += 2;
	$aclentries = $db->query('SELECT _u.i,_u.name,t,j,l,type,e FROM acl LEFT OUTER JOIN usr _u ON acl.u=_u.i ORDER BY t DESC LIMIT 100 OFFSET '.(($page - 1) * 100));
	$totalrows = $db->query('SELECT COUNT(t) c FROM acl')->fetchAll()[0]->c;

	function loggedin($state)
	{
		if ($state == 0) return 'no';
		if ($state == 1) return 'yes';
		if ($state == 2) return 'guest';
		return $state;
	}

	function userlink($row)
	{
		if ($row->name == null) return 'null';
		return '<a href="user.php?id='.$row->i.'">'.$row->name.' ('.$row->i.')</a>';
	}

	$ac_types = [
		0 => 'generic',
		1 => 'veh hp nan',
		2 => 'veh hp high',
		3 => 'dialogid',
		4 => 'veh access',
		5 => 'flood',
		6 => 'work veh hp increase',
		7 => 'work too fast',
		8 => 'weird dialog input',
		9 => 'too long dialog input',
	];

	function type($t)
	{
		global $ac_types;
		if (array_key_exists($t, $ac_types)) return $ac_types[$t];
		return $t;
	}
}

function adminlink($name, $target)
{
	global $action;
	if ($action == $target) {
		echo '<strong>'.$name.'</strong>';
	} else {
		echo '<a href="admin.php?action='.$target.'">'.$name.'</a>';
	}
}

$__script = '_admin';
require '../inc/output.php';
