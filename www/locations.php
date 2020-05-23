<?php
include('../inc/bootstrap.php');

if (isset($_GET['code'])) {
	$code = $_GET['code'];
	$stmt = $db->prepare('SELECT i,c,n,flags FROM apt WHERE c=? LIMIT 1');
	$stmt->bindValue(1, $code);
	if (!$stmt->execute() || !($r = $stmt->fetchAll()) || !count($r)) {
		$location_not_found_message = 'No location with code ' . $code . ' found.';
		goto notfound;
	}
	$apt = $r[0];
	++$db_querycount;
	$inbounds = $db->query('SELECT f.id,f.tunload,v.m vehmodel,m.name missionpointname,a.c fromcode FROM flg f '.
				'JOIN veh v ON f.vehicle=v.i '.
				'JOIN msp m ON f.tmsp=m.i '.
				'JOIN apt a ON f.fapt=a.i '.
				'WHERE tunload>0 AND f.tapt='.$apt->i.
				' ORDER BY tunload DESC LIMIT 5')->fetchAll();
	++$db_querycount;
	$outbounds = $db->query('SELECT f.id,f.tload,v.m vehmodel,m.name missionpointname,a.c tocode FROM flg f '.
				'JOIN veh v ON f.vehicle=v.i '.
				'JOIN msp m ON f.fmsp=m.i '.
				'JOIN apt a ON f.tapt=a.i '.
				'WHERE tload>0 AND f.fapt='.$apt->i.
				' ORDER BY tload DESC LIMIT 5')->fetchAll();
	$stat_num_inbound = $stat_num_outbound = 0;
	++$db_querycount;
	if (($r = $db->query('SELECT (SELECT COUNT(*) FROM flg WHERE tapt='.$apt->i.') inb, '.
				'(SELECT COUNT(*) FROM flg WHERE fapt='.$apt->i.') outb')) && $r = $r->fetchAll())
	{
		$stat_num_inbound = $r[0]->inb;
		$stat_num_outbound = $r[0]->outb;
	}
	$stat_refuels = $stat_fuel_loaded = $stat_refuel_spending = 0;
	++$db_querycount;
	if (($r = $db->query('SELECT COUNT(*) num,SUM(paid) spending, SUM(fuel) fuel '.
			'FROM refuellog r JOIN svp s ON s.id=r.svp WHERE s.apt='.$apt->i)) && $r = $r->fetchAll())
	{
		$stat_refuels = $r[0]->num;
		$stat_refuel_spending = $r[0]->spending;
		$stat_fuel_loaded = $r[0]->fuel;
	}
	$stat_repairs = $stat_dmg_repaired = $stat_repair_spending = 0;
	++$db_querycount;
	if (($r = $db->query('SELECT COUNT(*) num,SUM(paid) spending, SUM(damage) dmg '.
			'FROM repairlog r JOIN svp s ON s.id=r.svp WHERE s.apt='.$apt->i)) && $r = $r->fetchAll())
	{
		$stat_repairs = $r[0]->num;
		$stat_repair_spending = $r[0]->spending;
		$stat_dmg_repaired = $r[0]->dmg;
	}
	$__script = '_location';
} else {
notfound:
	$__script = '_locations';
	$airports_mainland = [];
	$airports_ne = [];
	$airports_nw = [];
	$airports_se = [];
	$airports_sw = [];
	++$db_querycount;
	foreach ($db->query('SELECT i,c,n,x,y,e FROM apt') as $apt) {
		if (-3000 < $apt->x && $apt->x < 3000 &&
			-3000 < $apt->y && $apt->y < 3000)
		{
			$airports_mainland[] = $apt;
		} else if ($y > 0) {
			if ($x > 0) {
				$airports_ne[] = apt;
			} else {
				$airports_nw[] = apt;
			}
		} else {
			if ($x > 0) {
				$airports_se[] = apt;
			} else {
				$airports_sw[] = apt;
			}
		}
	}
}

include('../inc/output.php');
