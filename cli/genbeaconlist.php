<?php

require '../inc/conf.php';
require '../inc/db.php';


$apts = $db->query('SELECT i,c FROM apt')->fetchAll();

ob_start();
echo '<table class="beaconlist">';
echo '<tr>';
echo '<th>Beacon</th>';
echo '<th>End 1</th>';
echo '<th>Code</th>';
echo '<th>Nav</th>';
echo '<th>End 2</th>';
echo '<th>Code</th>';
echo '<th>Nav</th>';
echo '</tr>';
nextap:

$apt = array_pop($apts);
if ($apt == null) {
	echo '</table>';
	file_put_contents('../articles/gen/beaconlist.html', ob_get_contents());
	ob_end_clean();
	exit();
}

$runways = [];
$beacon = $apt->c;
foreach ($db->query('SELECT i,h,s,n FROM rnw WHERE a='.$apt->i.' AND type=1 ORDER BY i') as $r) {
	if (!isset($runways[$r->i])) {
		$runways[$r->i] = [$r];
	} else {
		$runways[$r->i][] = $r;
	}
}

if (!count($runways)) {
	$runways[] = [];
}

/*
beacon
runway angle 1
code
vor
ils
runway angle 2
code
vor
ils
*/

foreach ($runways as $k => $r) {
	echo '<tr>';
	echo '<td>';
	if ($k == 0) {
		echo '<a href="article.php?title=';
		echo $beacon;
		echo '">';
		echo $beacon;
		echo '</a>';
	}
	echo '</td>';
	foreach ($r as $end) {
		echo '<td>';
		echo $end->h;
		echo '&deg;</td>';
		echo '<td>';
		$h = ceil($end->h / 10);
		if ($h < 10) {
			echo '0';
		}
		echo $h;
		echo $end->s;
		echo '</td>';
		echo '<td>';
		if ($end->n & 2) {
			echo '+VOR';
		}
		if ($end->n & 4) {
			echo ' +ILS';
		}
		echo '</td>';
	}
	if (count($r) < 2) {
		echo '<td></td><td></td><td></td><td></td>';
	}
	if (count($r) < 1) {
		echo '<td></td><td></td><td></td><td></td>';
	}
	echo '</tr>';
}

goto nextap;
