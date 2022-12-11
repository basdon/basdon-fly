<?php
/*This is ran from the plugin whenever a flight is finished, and will send a message to Discord.
See also fly-plugin/discordflightlog.c*/

include('../inc/conf.php');

if (!isset($DISCORD_FLIGHTLOG_WEBHOOK)) {
	exit(0);
}

include('../inc/__settings.php');
include('../inc/db.php');

if ($argc < 2) {
	echo "missing id argument\n";
	exit(1);
}

try {
	$id = (int) $argv[1];
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
		echo "flight does not exist\n";
		exit(1);
	}
} catch (PDOException $e) {
	echo "failed: {$e->getMessage()}\n";
	exit(1);
}

include('../templates/flightstatuses.php');
include('../templates/missiontypes.php');
include('../templates/aircraftnames.php');
$r = $r[0];
if ($r->missiontype & $SETTING__PASSENGER_MISSIONTYPES) {
	$satisfaction = ':ok_hand: ' . $r->satisfaction . '% passenger satisfaction';
} else {
	$satisfaction = 'cargo flight';
}
$status = fmt_flight_status($r->state, $r->tload);
$diff = $r->tlastupdate - $r->tstart;
$duration = sprintf('%02d:%02d', floor($diff / 60), $diff % 60);
$vehname = aircraft_name($r->vehmodel);
$distance = round($r->adistance);

$embed = new stdClass();
$embed->title = "Flight #{$id} by {$r->name} ({$status})";
$embed->url = "{$ABS_URL}/flight.php?id={$id}";
$embed->image = new stdClass();
$embed->image->url = "{$ABS_URL}/s/flightmap.php?id={$id}";
$from = new stdClass();
$from->name = 'From';
$from->value = "[{$r->fromname} ($r->fromcode)]({$ABS_URL}/article.php?title={$r->fromcode}) {$r->fromgate}";
$from->inline = true;
$to = new stdClass();
$to->name = 'To';
$to->value = "[{$r->toname} ($r->tocode)]({$ABS_URL}/article.php?title={$r->tocode}) {$r->togate}";
$to->inline = true;
$details = new stdClass();
$details->name = 'Details';
$details->value = ":straight_ruler: {$distance}m :stopwatch: {$duration} :airplane: {$vehname}\n:hearts: {$r->damage} damage taken\n{$satisfaction}";
$embed->fields = [$from, $to, $details];
/*The normal status colors are very light (because they're background),
but these must be pretty strong for the discord embed card (73 saturation instead of 12).*/
switch ($r->state) {
case 1: $embed->color = 0x4444ff; break;
case 2:
case 4:
case 32:
case 256:
case 512:
case 1024:
case 4096: $embed->color = 0xff4444; break;
case 8: $embed->color = 0x44ff44; break;
case 16:
case 128: $embed->color = 0xff9123; break;
case 64: $embed->color = 0xa144ff; break;
case 2048: $embed->color = 0xffff44; break; /*paused flights should never be posted to discord, so this is not really needed to be here*/
}

$content = new stdClass();
$content->embeds = [$embed];
$resource = fopen($DISCORD_FLIGHTLOG_WEBHOOK, 'r', false, stream_context_create([
	'http' => [
		'method' => 'POST',
		'header' => 'Content-Type: application/json',
		'content' => json_encode($content),
	]
]));
if ($resource !== false) {
	fclose($resource);
}
// TODO ideally log somewhere, although an E_WARNING should be emitted, when 'fopen' fails
