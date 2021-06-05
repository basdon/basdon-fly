<?php

require '../inc/conf.php';
require '../inc/db.php';

$aptids = [];
$aptcodes = [];
foreach ($db->query('SELECT DISTINCT i,c FROM apt') as $r) {
	$aptids[] = $r->i;
	$aptcodes[] = $r->c;
}

$flightmap_polydata = [];

// runways
$runways_by_airport = [];
$numrunways = 0;
$runwaydata = [];
foreach($db->query('SELECT r.a,r.x,r.y,r.i,r.h,r.w,a.flags FROM rnw r JOIN apt a ON r.a=a.i WHERE r.type=1') as $r) {
	if (!isset($runways_by_airport[$r->a])) {
		$runways_by_airport[$r->a] = [];
	}
	if (isset($runways_by_airport[$r->a][$r->i])) {
		$data = $runways_by_airport[$r->a][$r->i];
		$dx = $data[1] - $r->x;
		$dy = $data[2] - $r->y;
		$runwaydata[] = (int) $data[0];
		$runwaydata[] = (int) $data[1];
		$runwaydata[] = (int) $data[2];
		$runwaydata[] = (int) $data[3];
		$runwaydata[] = (int) $data[4];
		$runwaydata[] = round(sqrt($dx * $dx + $dy * $dy));
		$numrunways++;
	} else {
		$runways_by_airport[$r->a][$r->i] = [$r->flags, $r->x, $r->y, $r->h, $r->w, 0];
	}
}
$flightmap_polydata[] = $numrunways;
foreach ($runwaydata as $r) {
	$flightmap_polydata[] = $r;
}

// mainland
$flightmap_polydata[] = 1;
$flightmap_polydata[] = -3000;
$flightmap_polydata[] = -3000;
$flightmap_polydata[] = 0;
$flightmap_polydata[] = 0;
$flightmap_polydata[] = 6000;
$flightmap_polydata[] = 6000;

nextap:

$aptid = array_pop($aptids);
if ($aptid == null) {
	$island_data = 'var fm_islands=' . json_encode($flightmap_polydata) . ';';
	file_put_contents('../static/gen/islandmap_fm_islands.js', $island_data);
	exit();
}
$aptcode = array_pop($aptcodes);

$maxx = -100000;
$maxy = -100000;
$minx = 100000;
$miny = 100000;

$zones = [];

foreach($db->query('SELECT filename FROM map WHERE ap=\''.$aptid.'\'')->fetchAll() as $maprow) {
	$filename = "../../maps/{$maprow->filename}.map";
	$filesize = filesize($filename);
	if ($filesize < 32) {
		echo "invalid map file: {$filename}";
		continue;
	}
	$f = fopen($filename, 'rb');
	if (!$f) {
		echo "can't open: {$filename}";
		continue;
	}
	$data = fread($f, $filesize);
	fclose($f);
	$data = unpack('C*', $data);
	// the data array starts at index 1 for whatever reason
	if ($data[4] != 3) {
		echo 'invalid map version: '.$data[3];
		continue;
	}
	// assuming the map file is correctly structured
	$numremoves = $data[5] | ($data[6] << 8) | ($data[7] << 16) | ($data[8] << 24);
	$numobjects = $data[9] | ($data[10] << 8) | ($data[11] << 16) | ($data[12] << 24);
	$numzones = $data[17] | ($data[18] << 8) | ($data[19] << 16) | ($data[20] << 24);
	$offset = 1 /*remember data starts at 1 because php idk*/ + 32 + 20 * $numremoves;
	while ($numzones--) {
		$zd = new stdClass();
		for ($i = 0; $i < 4; $i++) {
			$int = $data[$offset] | ($data[$offset+1] << 8) | ($data[$offset+2] << 16) | ($data[$offset+3] << 24);
			$zd->coords[$i] = unpack('f', pack('i', $int))[1];
			$offset += 4;
		}
		$zd->color = $data[$offset] | ($data[$offset+1] << 8) | ($data[$offset+2] << 16) | ($data[$offset+3] << 24);
		$zones[] = $zd;
		$offset += 4;

		if ($zd->coords[0] < $minx) $minx = $zd->coords[0];
		if ($zd->coords[1] < $miny) $miny = $zd->coords[1];
		if ($zd->coords[2] > $maxx) $maxx = $zd->coords[2];
		if ($zd->coords[3] > $maxy) $maxy = $zd->coords[3];
	}
}

if (count($zones) < 100 && $minx > -4000 && $maxx < 4000 && $miny > -4000 && $maxy < 4000) {
	// LSA and SFA have only a few zones to hide removed buildings from the minimap,
	// but those obviously aren't islands.
	// Skip if there are too little amount of zones and it's within mainland boundaries.
	goto nomapforthisap;
}

$flightmap_polydata[] = count($zones);
$flightmap_polydata[] = round($minx);
$flightmap_polydata[] = -round($miny);
foreach ($zones as $zd) {
	$flightmap_polydata[] = round($zd->coords[0] - $minx);
	$flightmap_polydata[] = -round($zd->coords[1] - $miny);
	$flightmap_polydata[] = round($zd->coords[2] - $minx);
	$flightmap_polydata[] = -round($zd->coords[3] - $miny);
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
file_put_contents('../gen/islandmap_'.$aptcode.'.html', $htmldata);

if (isset($_GET['id'])) {
	echo $htmldata;
}

goto nextap;

nomapforthisap:
// write an empty file for airports that don't have an island map
$htmldata = '';
goto writehtml;
