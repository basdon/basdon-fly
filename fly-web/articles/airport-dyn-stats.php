<?php
if (($r = $db->query('SELECT i,c,n,flags FROM apt WHERE c=\'' . $code . '\' LIMIT 1')->fetchAll()) && count($r)) {
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
	if (($r = $db->query('SELECT (SELECT COUNT(*) FROM flg WHERE tapt='.$apt->i.' AND tunload>0) inb, '.
				'(SELECT COUNT(*) FROM flg WHERE fapt='.$apt->i.' AND tload>0) outb')) && $r = $r->fetchAll())
	{
		$stat_num_inbound = $r[0]->inb;
		$stat_num_outbound = $r[0]->outb;
	}
	$stat_refuels = $stat_refuel_spending = 0;
	++$db_querycount;
	if (($r = $db->query('SELECT COUNT(*) num,SUM(paid) spending '.
			'FROM refuellog r JOIN svp s ON s.id=r.svp WHERE s.apt='.$apt->i)) && $r = $r->fetchAll())
	{
		$stat_refuels = $r[0]->num;
		$stat_refuel_spending = $r[0]->spending;
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
	++$db_querycount;
	$missionpoints = $db->query('SELECT i,name,t FROM msp WHERE a='.$apt->i);
	$__template = '^airport_article_dyn_stats';
	require '../inc/output.php';
} else {
	echo '<p>failed to get info for airport ' . $code . '</p>';
}
