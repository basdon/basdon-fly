<?php
// generate the island map images and html files for inclusion in articles

require '../inc/conf.php';
require '../inc/db.php';

$aptids = [];
$aptcodes = [];
foreach ($db->query('SELECT DISTINCT i,c FROM apt') as $r) {
	$aptids[] = $r->i;
	$aptcodes[] = $r->c;
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

foreach($db->query('SELECT filename FROM map WHERE ap='.$aptid)->fetchAll() as $maprow) {
	$filename = "../../maps/{$maprow->filename}.map";
	$filesize = filesize($filename);
	if ($filesize < 32) {
		echo "invalid map file: {$filename}";
		exit(1);
	}
	$f = fopen($filename, 'rb');
	if (!$f) {
		echo "can't open: {$filename}";
		exit(1);
	}
	$data = fread($f, $filesize);
	fclose($f);
	if (ord($data[3]) != 3) {
		echo "invalid map version for {$filename}: " . ord($data[3]) . "\n";
		exit(1);
	}
	$numremoves = unpack('V', substr($data, 4, 4))[1];
	$numzones = unpack('V', substr($data, 16, 4))[1];
	$offset = 32 + 20 * $numremoves;
	while ($numzones--) {
		$c = unpack('f4c/VV', substr($data, $offset, 20)); $offset += 20;
		$zd = $zones[] = new stdClass();
		$zd->minx = $c['c1'];
		$zd->miny = $c['c2'];
		$zd->maxx = $c['c3'];
		$zd->maxy = $c['c4'];
		$zd->color = $c['V'];
		$minx = min($minx, $zd->minx);
		$miny = min($miny, $zd->miny);
		$maxx = max($maxx, $zd->maxx);
		$maxy = max($maxy, $zd->maxy);
	}
}

if (count($zones) < 100 && $minx > -4000 && $maxx < 4000 && $miny > -4000 && $maxy < 4000) {
	// LSA and SFA have only a few zones to hide removed buildings from the minimap,
	// but those obviously aren't islands.
	// Skip if there are too little amount of zones and it's within mainland boundaries.
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
	imagefilledrectangle($im, xcoord($z->minx), ycoord($z->miny), xcoord($z->maxx), ycoord($z->maxy), $colors[$z->color]);
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
