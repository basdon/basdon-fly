<?php
require '../inc/bootstrap.php';


require '../inc/queryflightlist.php'; // required to render flight list
require '../inc/aircraftnames.php'; // required to render flight list
require '../inc/flightstatuses.php'; // required to render flight list
require '../inc/missiontypes.php'; // required to render flight list

$flight_list = homepage_flight_list_query();

$db_querycount += 6;
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
require '../inc/output.php';
