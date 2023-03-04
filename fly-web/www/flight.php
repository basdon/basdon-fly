<?php
require '../inc/bootstrap.php';

$id = -1;
if (isset($_GET['id'])) {
	$id = intval($_GET['id']);
}
$returnpage = -1;
if (isset($_GET['rp'])) {
	$returnpage = intval($_GET['rp']);
}

function aircraftowner($ownername)
{
	if ($ownername == null) {
		return 'public';
	} else {
		$usr = new stdClass();
		$usr->name = $ownername;
		$usr->i = -1; // guest shouldn't own vehicles, so this _should_ be fine
		return linkuser($usr);
	}
}

$META_TAGS['og:title'] = 'Unknown flight';

++$db_querycount;
try {
	$r = $db->query(
		'SELECT _f.*,_u.name,_u.i,_a.n fromname,_a.c fromcode,_b.n toname,_b.c tocode,_v.m vehmodel,
		       _m.name fromgate,_n.name togate,_o.name ownername
		FROM flg _f
		JOIN usr _u ON _f.player=_u.i
		JOIN apt _a ON _f.fapt=_a.i
		JOIN apt _b ON _f.tapt=_b.i
		JOIN veh _v ON _f.vehicle=_v.i
		JOIN msp _m ON _f.fmsp = _m.i
		JOIN msp _n ON _f.tmsp = _n.i
		LEFT OUTER JOIN usr _o ON _v.ownerplayer=_o.i
		WHERE id='.$id
	);
	if ($r === false || ($r = $r->fetchAll()) === false || empty($r)) {
		unset($r);
	} else {
		include '../inc/flightstatuses.php';
		include '../inc/missiontypes.php';
		include '../inc/aircraftnames.php';
		include '../inc/vehiclefuelcap.php';
		$r = $r[0];
		if ($r->missiontype & $SETTING__PASSENGER_MISSIONTYPES) {
			$satisfaction = $r->satisfaction . '% passenger satisfaction';
		} else {
			$satisfaction = 'cargo';
		}
		$r->is_finished = $r->state != 1 /*inprogress*/ && $r->state != 2048 /*paused*/;
		$status = fmt_flight_status($r->state, $r->tload);
		$diff = $r->tlastupdate - $r->tstart;
		$duration = sprintf('%02d:%02d', floor($diff / 60), $diff % 60);
		$vehname = aircraft_name($r->vehmodel);
		$META_TAGS['og:title'] = "Flight #{$id} by {$r->name} ({$status})";
		$META_TAGS['og:image'] = "{$ABS_URL}/s/flightmap.php?id={$id}";
		$META_TAGS['twitter:card'] = 'summary_large_image';
		$META_TAGS['og:description'] = "{$r->fromname} -&gt; {$r->toname} ({$r->adistance}m) ({$duration})\n{$vehname}, {$satisfaction}";
		/*The normal status colors are very light (because they're background),
		but these must be pretty strong for the discord embed card (73 saturation instead of 12).*/
		switch ($r->state) {
		case 1: $META_TAGS['theme-color'] = '#4444ff'; break;
		case 2:
		case 4:
		case 32:
		case 256:
		case 512:
		case 1024:
		case 4096: $META_TAGS['theme-color'] = '#ff4444'; break;
		case 8: $META_TAGS['theme-color'] = '#44ff44'; break;
		case 16:
		case 128: $META_TAGS['theme-color'] = '#ff9123'; break;
		case 64: $META_TAGS['theme-color'] = '#a144ff'; break;
		case 2048: $META_TAGS['theme-color'] = '#ffff44'; break;
		}
	}
} catch (PDOException $e) {
	$r = false;
}

$__template = '_flight';
require '../inc/output.php';
