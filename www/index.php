<?php
include('../inc/bootstrap.php');

$db_querycount += 3;
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

if ($onlineplayers !== false && ($onlineplayers = $onlineplayers->fetchAll()) !== false) {
	$onlineplayers = $onlineplayers;
} else {
	$onlineplayers = [];
}

$__script = '_index';
include('../inc/output.php');
