<?php
// treat all notices/warnings/errors as errors
function exception_error_handler($severity, $message, $file, $line)
{
	throw new ErrorException($message, 0, $severity, $file, $line);
}
set_error_handler("exception_error_handler");

require '../inc/conf.php';
require '../inc/db.php';

$aptcodes = [];
if (isset($_GET['id'])) {
	$aptcodes[] = $db->query('SELECT c FROM apt WHERE i='.$_GET['id'])->fetchAll()[0]->c;
} else {
	foreach ($db->query('SELECT c FROM apt') as $r) {
		$aptcodes[] = $r->c;
	}
}

nextap:

$apcode = array_pop($aptcodes);
if ($apcode == null) {
	exit();
}

$apt = $db->query('SELECT i,c,n,x,y,z FROM apt WHERE c=\''.$apcode.'\'')->fetchAll()[0];
$apid = $apt->i;

$runway_ends = [];
foreach ($db->query('SELECT x,y,z,w,i,h,s,n,surf FROM rnw WHERE a='.$apid.' ORDER BY i') as $r) {
	if (!isset($runway_ends[$r->i])) {
		$runway_ends[$r->i] = [$r];
	} else {
		$runway_ends[$r->i][] = $r;
	}
}
$runways = [];
$helipads = [];
foreach ($runway_ends as $r) {
	if (count($r) == 2) {
		$runways[] = $r;
	} else {
		$helipads[] = $r[0];
	}
}

$missionpointcount = $db->query('SELECT COUNT(*) c FROM msp WHERE a='.$apid)->fetchAll()[0]->c;
$servicepointcount = $db->query('SELECT COUNT(*) c FROM svp WHERE apt='.$apid)->fetchAll()[0]->c;
$vehiclecount = $db->query('SELECT COUNT(*) c FROM veh WHERE e=1 AND ap='.$apid)->fetchAll()[0]->c;

$types = ['Public', 'Civil', 'Military', 'Civil/Military'];
$t = 0;
foreach ($db->query('SELECT t FROM msp WHERE a='.$apid) as $msp) {
	$t |= $msp->t;
}
if ($t & ~256) {
	// ignore military heli for now if there are other types
	$t &= ~256;
}
$type = 0;
if ($t & (512 | 1024)) {
	$type |= 2;
} else {
	$type |= 1;
}

function rnw($heading, $specifier)
{
	return sprintf('%02d', round($heading / 10)) . $specifier;
}

function surf($surf)
{
	$surf_n = ['', 'Asphalt', 'Dirt', 'Concrete', 'Grass', 'Water'];
	$surf_bg = ['', '333', '6a3300', 'aaa', '256c00', '004aa6'];
	$surf_fg = ['', 'fff', 'fff', '000', 'fff', 'fff'];
	echo '<td style="color:#' . $surf_fg[$surf] . ';background:#' . $surf_bg[$surf] . '">';
	echo $surf_n[$surf];
	echo '</td>';
}

ob_start();
$nav = [];
echo '<table style="width:220px;font-size:90%">';
echo '<tbody>';
echo '<tr class="fsh"><td colspan="3">' . $apt->n . '</td></tr>';
echo '<tr><td colspan="3" style="padding:0"><img src="/s/articles/' . strtolower($apt->c) . '.jpg" style="width:100%"/></td></tr>';
echo '<tr><td colspan="3" style="font-weight:bold;text-align:center">Airport Code: ' . $apt->c . '</td></tr>';
echo '<tr class="fsh"><td colspan="3">Summary</td></tr>';
echo '<tr><td colspan="2"><strong>Type</strong></td><td>' . $types[$type] . '</td></tr>';
echo '<tr><td colspan="2"><strong>Elevation</strong></td><td>' . round($apt->z) . 'ft</td></tr>';
echo '<tr><td colspan="2"><strong>Mission points</strong></td><td>' . $missionpointcount . '</td></tr>';
echo '<tr><td colspan="2"><strong>Service points</strong></td><td>' . $servicepointcount . '</td></tr>';
echo '<tr><td colspan="2"><strong>Vehicle spawns</strong></td><td>' . $vehiclecount . '</td></tr>';
echo '<tr class="fsh"><td colspan="3">Location</td></tr>';
echo '<tr class="center"><td><strong>X:</strong> ' . round($apt->x) . '</td><td><strong>Y:</strong> ' . round($apt->y) . '</td><td><strong>Z:</strong> ' . round($apt->z) . '</td></tr>';
if (count($runways)) {
	echo '<tr class="fsh"><td colspan="3">Runways</td></tr>';
	echo '<tr style="text-align:center;background:#dddddd;font-size:90%"><td><strong>Direction</strong></td><td><strong>Length/Width</strong></td><td><strong>Surface</strong></td></tr>';
	foreach ($runways as $r) {
		$rnw0 = rnw($r[0]->h, $r[0]->s);
		$rnw1 = rnw($r[1]->h, $r[1]->s);
		if ($r[0]->n & 2) {
			$nav[] = $rnw0 . (($r[0]->n & 4) ? ' <strong>+ILS</strong>' : '');
		}
		if ($r[1]->n & 2) {
			$nav[] = $rnw1 . (($r[1]->n & 4) ? ' <strong>+ILS</strong>' : '');
		}
		echo '<tr style="text-align:center"><td>';
		echo $rnw0 . '/' . $rnw1;
		echo '</td><td>';
		$dx = $r[1]->x - $r[0]->x;
		$dy = $r[1]->y - $r[0]->y;
		echo round(sqrt($dx * $dx + $dy * $dy)) . 'm/' . round($r[0]->w) . 'm';
		echo '</td>';
		surf($r[0]->surf);
		echo '</tr>';
	}
}
if (count($helipads)) {
	echo '<tr class="fsh"><td colspan="3">Helipads/platforms</td></tr>';
	echo '<tr style="text-align:center;background:#dddddd;font-size:90%"><td colspan="2"><strong>Size</strong></td><td><strong>Surface</strong></td></tr>';
	foreach ($helipads as $h) {
		echo '<tr style="text-align:center"><td colspan="2">';
		echo round($h->w) . 'x' . round($h->w);
		echo 'm</td>';
		surf($h->surf);
		echo '</tr>';
	}
}
echo '<tr class="fsh"><td colspan="3">Navigation</td></tr>';
echo '<tr><td><strong>NDB</strong></td><td colspan="2">' . $apt->c . '</td></tr>';
if (count($nav)) {
	echo '<tr><td valign="top"><strong>VOR</strong></td><td colspan="2">';
	foreach ($nav as $n) {
		echo $apt->c . $n . '<br/>';
	}
	echo '</td></tr>';
}
echo '</tbody>';
echo '</table>';

file_put_contents('../gen/apfactsheet_'.$apt->c.'.html', ob_get_contents());
if (isset($_GET['web'])) {
	echo '<link rel="stylesheet" href="http://localhost:8080/sapstatic/article.css" type="text/css" />';
	ob_end_flush();
} else {
	ob_end_clean();
}

goto nextap;
