<?php

require '../inc/conf.php';
require '../inc/db.php';

if (!isset($conf_maps_dir)) {
	fwrite(STDERR, "warn: not generating island maps because 'conf_maps_dir' conf.php variable is not set\n");
	exit();
}

$aptids = [];
$aptcodes = [];
if (isset($_GET['id'])) {
	$aptids[] = $_GET['id'];
	$aptcodes[] = $db->query('SELECT c FROM apt WHERE i='.$_GET['id'])->fetchAll()[0]->c;
} else {
	foreach ($db->query('SELECT DISTINCT i,c FROM apt') as $r) {
		$aptids[] = $r->i;
		$aptcodes[] = $r->c;
	}
}

nextap:

$aptid = array_pop($aptids);
if ($aptid == null) {
	exit();
}
$aptcode = array_pop($aptcodes);

$maxx = -100000;
$maxy = -100000;
$minx = 100000;
$miny = 100000;

$zones = [];

foreach($db->query('SELECT filename FROM map WHERE ap=\''.$aptid.'\'')->fetchAll() as $maprow) {
	$filename = "{$conf_maps_dir}/{$maprow->filename}.map";
	$filesize = filesize($filename);
	if ($filesize < 28) {
		echo "invalid map file: {$filename}";
		continue;
	}
	$f = fopen($filename, 'rb');
	if (!$f) {
		continue;
	}
	$data = fread($f, $filesize);
	fclose($f);
	$data = unpack('C*', $data);
	// the data array starts at index 1 for whatever reason
	if ($data[4] != 2) {
		echo 'invalid map version: '.$data[3];
		continue;
	}
	// assuming the map file is correctly structured
	$numremoves = $data[5] | ($data[6] << 8) | ($data[7] << 16) | ($data[8] << 24);
	$numobjects = $data[9] | ($data[10] << 8) | ($data[11] << 16) | ($data[12] << 24);
	$numzones = $data[13] | ($data[14] << 8) | ($data[15] << 16) | ($data[16] << 24);
	$offset = 1 /*remember data starts at 1 because php idk*/ + 28 + 20 * $numremoves + 28 * $numobjects;
	while ($numzones--) {
		$zonedata = new stdClass();
		for ($i = 0; $i < 4; $i++) {
			$int = $data[$offset] | ($data[$offset+1] << 8) | ($data[$offset+2] << 16) | ($data[$offset+3] << 24);
			$zonedata->coords[$i] = unpack('f', pack('i', $int))[1];
			$offset += 4;
		}
		$zonedata->color = $data[$offset] | ($data[$offset+1] << 8) | ($data[$offset+2] << 16) | ($data[$offset+3] << 24);
		$zones[] = $zonedata;
		$offset += 4;

		if ($zonedata->coords[0] < $minx) $minx = $zonedata->coords[0];
		if ($zonedata->coords[1] < $miny) $miny = $zonedata->coords[1];
		if ($zonedata->coords[2] > $maxx) $maxx = $zonedata->coords[2];
		if ($zonedata->coords[3] > $maxy) $maxy = $zonedata->coords[3];
	}
}

if (count($zones) < 100) {
	// LSA and SFA have only a few zones to hide removed buildings from the minimap,
	// but those obviously aren't islands.
	// Assume it's not an island and skip if there are too little amount of zones
	goto nomapforthisap;
}

// this block is blindlessly copied from genaerodromechart
// it's supposed to make acceptable image dimensions from the min/max coordinates
function xcoord($x)
{
	global $scale, $offx;
	return (int) (($x + $offx) * $scale);
}
function ycoord($y)
{
	global $scale, $offy, $imgh;
	return (int) ($imgh - ($y + $offy) * $scale);
}
$maxx += 50;
$maxy += 50;
$minx -= 50;
$miny -= 50;
$imgw = 500;
$sizex = $maxx - $minx;
$sizey = $maxy - $miny;
$offx = $offy = 0;
if ($sizex < 700) {
	$offx = (700 - $sizex) / 2;
	$sizex = 700;
}
$scale = $imgw / $sizex;
$imgh = $sizey * $scale;
$offx -= $minx;
$offy -= $miny;
// end blindlessy copied block of code

$im = imagecreate($imgw, $imgh);
$bg = imagecolorallocate($im, 112, 136, 168);

$colors = [];

foreach($zones as $z) {
	if (!isset($colors[$z->color])) {
		$colors[$z->color] = imagecolorallocate($im, $z->color & 0xFF, ($z->color >> 8) & 0xFF, ($z->color >> 16) & 0xFF);
	}
	imagefilledrectangle($im, xcoord($z->coords[0]), ycoord($z->coords[1]), xcoord($z->coords[2]), ycoord($z->coords[3]), $colors[$z->color]);
}

ob_start();
imagepng($im);
imagedestroy($im);
$imagedata = ob_get_clean();
file_put_contents('../static/gen/islandmap_'.$aptcode.'.png', $imagedata);
$htmldata = '<h3>Island map</h3><p><img src="'.$STATICPATH.'/gen/islandmap_'.$aptcode.'.png" alt="'.$aptcode.' island map" title="'.$aptcode.' island map" /></p>';
writehtml:
file_put_contents('../articles/islandmaps/'.$aptcode.'.html', $htmldata);

if (isset($_GET['id'])) {
	echo $htmldata;
}

goto nextap;

nomapforthisap:
// write an empty file for airports that don't have an island map
$htmldata = '';
goto writehtml;
