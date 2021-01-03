<?php
include('../inc/bootstrap.php');

$db_querycount += 9;
$activeflights = $db->query("
	SELECT id,_u.name,_u.i,_a.c f,_b.c t,tstart,state,tload,veh.m vehmodel
	FROM flg _f
	JOIN usr _u ON _f.player=_u.i
	JOIN apt _a ON _a.i=_f.fapt
	JOIN apt _b ON _b.i=_f.tapt
	JOIN veh ON veh.i=_f.vehicle
	WHERE state=1
	ORDER BY tstart DESC
	LIMIT 10");
$lastflights = $db->query(
	"SELECT id,_u.name,_u.i,_a.c f,_b.c t,state,tload,adistance,tlastupdate,veh.m vehmodel
	FROM flg _f
	JOIN usr _u ON _f.player=_u.i
	JOIN apt _a ON _a.i=_f.fapt
	JOIN apt _b ON _b.i=_f.tapt
	JOIN veh ON veh.i=_f.vehicle
	WHERE state=8
	ORDER BY id DESC
	LIMIT 7");
$onlineplayers = $db->query("
	SELECT _u.name,_u.i,_u.score
	FROM ses _s
	JOIN usr _u ON _s.u=_u.i
	WHERE _s.e>UNIX_TIMESTAMP()-40");
$last48 = $db->query("
	SELECT DISTINCT _u.i,_u.name,_u.score
	FROM ses _s
	JOIN usr _u ON _s.u=_u.i
	WHERE _s.e>UNIX_TIMESTAMP()-48*3600");
$guestcount = $db->query('SELECT COUNT(s.i) c FROM ses s JOIN usr u ON s.i=u.i WHERE u.groups&'.$GROUP_GUEST);
$membercount = $db->query('SELECT COUNT(i) c FROM usr WHERE groups&'.$GROUP_GUEST.'=0');
$flightcount = $db->query('SELECT COUNT(id) c FROM flg');
$latestmember = $db->query('SELECT name,i FROM usr WHERE groups&'.$GROUP_GUEST.'=0 ORDER BY i DESC LIMIT 1');
$combinedstats = $db->query('SELECT SUM(onlinetime/3600) o,SUM(flighttime/3600) f,SUM(distance) d,SUM(cash) c FROM usr');
$last_update_articles = $db->query('SELECT name,title FROM art WHERE cat=8 ORDER BY name DESC LIMIT 5');

if ($onlineplayers !== false && ($onlineplayers = $onlineplayers->fetchAll()) !== false) {
	$onlineplayers = $onlineplayers;
} else {
	$onlineplayers = [];
}
function ordef($r, $def)
{
	if ($r !== false && ($r = $r->fetchAll()) && count($r)) {
		return $r[0];
	} else {
		return $def;
	}
}
$def = new stdClass();
$def->c = 0;
$guestcount = ordef($guestcount, $def);
$membercount = ordef($membercount, $def);
$flightcount = ordef($flightcount, $def);
$latestmember = ordef($latestmember, null);
$combinedstats = ordef($combinedstats, null);

if ($last48 !== false && ($last48 = $last48->fetchAll()) !== false) {
	$last48 = $last48;
} else {
	$last48 = [];
}

$__script = '_index';
include('../inc/output.php');
