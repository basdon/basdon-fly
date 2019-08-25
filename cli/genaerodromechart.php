<?php

require '../inc/conf.php';
require '../inc/db.php';

function xcoord($x)
{
	global $scale, $offx;
	return ($x + $offx) * $scale;
}

function ycoord($y)
{
	global $scale, $offy, $imgh;
	return $imgh - ($y + $offy) * $scale;
}

$maxx = -100000;
$maxy = -100000;
$minx = 100000;
$miny = 100000;

$apid = $_GET['id'];
$runway_ends = [];
foreach ($db->query('SELECT x,y,z,w,i FROM rnw WHERE a='.$apid.' ORDER BY i') as $r) {
	if (!isset($runway_ends[$r->i])) {
		$runway_ends[$r->i] = [$r];
	} else {
		$runway_ends[$r->i][] = $r;
	}
	$maxx = max($maxx, $r->x);
	$maxy = max($maxy, $r->y);
	$minx = min($minx, $r->x);
	$miny = min($miny, $r->y);
}

$missionpoints = [];
foreach ($db->query('SELECT x,y,z,t FROM msp WHERE a='.$apid) as $r) {
	$missionpoints[] = $r;
	$maxx = max($maxx, $r->x);
	$maxy = max($maxy, $r->y);
	$minx = min($minx, $r->x);
	$miny = min($miny, $r->y);
}

$servicepoints = [];
foreach ($db->query('SELECT x,y,z FROM svp WHERE apt='.$apid) as $r) {
	$servicepoints[] = $r;
	$maxx = max($maxx, $r->x);
	$maxy = max($maxy, $r->y);
	$minx = min($minx, $r->x);
	$miny = min($miny, $r->y);
}

$vehicles = [];
foreach ($db->query('SELECT m,x,y,z FROM veh WHERE e=1 AND ap='.$apid.' ORDER BY i ASC') as $r) {
	$vehicles[] = $r;
	$maxx = max($maxx, $r->x);
	$maxy = max($maxy, $r->y);
	$minx = min($minx, $r->x);
	$miny = min($miny, $r->y);
}

$maxx += 50;
$maxy += 50;
$minx -= 50;
$miny -= 50;

$imgw = 500;
$sizex = $maxx - $minx;
$sizey = $maxy - $miny;
$scale = ($imgw - 100) / $sizex;
$imgh = $sizey * $scale;
$offx = -$minx;
$offy = -$miny;
$offx += 100 / $scale;

$PI2 = pi() / 2;
$PI4 = pi() / 4;
$sqrt2 = sqrt(2);

$im = imagecreate($imgw, $imgh);
$color_white = $bg = imagecolorallocate($im, 255, 255, 255);
$color_black = imagecolorallocate($im, 0, 0, 0);
$color_rnw = imagecolorallocate($im, 11, 136, 192);
$color_service_inner = imagecolorallocate($im, 225, 225, 225);
$color_veh = imagecolorallocate($im, 91, 104, 119);
$color_veh_outline = imagecolorallocate($im, 64, 64, 64);
$color_msp = imagecolorallocate($im, 170, 0, 0);
$color_msp_outline = $color_black;
$font = 2;

foreach ($runway_ends as $r) {
	if (count($r) == 2) {
		$ox = (int) xcoord($r[0]->x);
		$oy = (int) ycoord($r[0]->y);
		$x = (int) xcoord($r[1]->x);
		$y = (int) ycoord($r[1]->y);
		$angle = atan2($y - $oy, $x - $ox);
		$anglePI4 = $angle + $PI4;
		$angle_PI4 = $angle + $PI4;
		$w = $sqrt2 * $r[0]->w / 2 * $scale;
		$pts = [$x + $w * cos($angle + $PI4),
			$y + $w * sin($angle + $PI4),
			$x + $w * cos($angle - $PI4),
			$y + $w * sin($angle - $PI4),
			$ox - $w * cos($angle + $PI4),
			$oy - $w * sin($angle + $PI4),
			$ox - $w * cos($angle - $PI4),
			$oy - $w * sin($angle - $PI4)];
		imagefilledpolygon($im, $pts, 4, $color_rnw);
	} else {

	}
}

$mspsize = 7;
$vehsize = 3;

function rect($x, $y, $size, $col_outline, $col)
{
	global $im;
	$os = $size;
	$ss = $os - 1;
	imagefilledrectangle($im, $x - $os, $y - $os, $x + $os, $y + $os, $col_outline);
	imagefilledrectangle($im, $x - $ss, $y - $ss, $x + $ss, $y + $ss, $col);
}

$missiontypes = ['cargo' => 0, 'passengers' => 0, 'heli' => 0, 'military' => 0, 'military heli' => 0, 'other' => 0];
foreach ($missionpoints as $p) {
	$x = xcoord($p->x);
	$y = ycoord($p->y);
	rect($x, $y, $mspsize, $color_msp_outline, $color_msp);
	if ($p->t & (1 | 2 | 4)) {
		$missiontypes['passengers']++;
	} else if ($p->t & (8 | 16 | 32)) {
		$missiontypes['cargo']++;
	} else if ($p->t & (64 | 128)) {
		$missiontypes['heli']++;
	} else if ($p->t & (256)) {
		$missiontypes['military']++;
	} else if ($p->t & (512)) {
		$missiontypes['military heli']++;
	} else {
		$missiontypes['other']++;
	}
}

$vehnames = [
	592 => 'Andromada',
	577 => 'AT-400',
	511 => 'Beagle',
	548 => 'Cargobob',
	512 => 'Crop duster',
	593 => 'Dodo',
	425 => 'Hunter',
	520 => 'Hydra',
	417 => 'Leviathan',
	487 => 'Maverick',
	553 => 'Nevada',
	497 => 'Police maverick',
	563 => 'Raindance',
	476 => 'Rustler',
	488 => 'San News heli',
	447 => 'Seasparrow',
	519 => 'Shamal',
	460 => 'Skimmer',
	469 => 'Sparrow',
	513 => 'Stuntplane'
];
$vehs = [];
foreach ($vehnames as $k => $v) {
	$vehs[$k] = 0;
}
foreach ($vehicles as $v) {
	$x = xcoord($v->x);
	$y = ycoord($v->y);
	$vehs[$v->m]++;
	rect($x, $y, $vehsize, $color_veh_outline, $color_veh);
}

$y = 2;
$fh = imagefontheight($font);
if (array_sum($missiontypes) != 0) {
	imagestring($im, $font, 2, $y, "Mission points:", $color_black);
	$y += $fh;
	foreach ($missiontypes as $type => $amount) {
		if ($amount > 0) {
			imagestring($im, $font, 2, $y, " $type ($amount)", $color_black);
			$y += $fh;
		}
	}
}
if (array_sum($vehs) != 0) {
	imagestring($im, $font, 2, $y, "Vehicles:", $color_black);
	$y += $fh;
	foreach ($vehs as $id => $amount) {
		if ($amount > 0) {
			imagestring($im, $font, 2, $y, " {$vehnames[$id]} ($amount)", $color_black);
			$y += $fh;
		}
	}
}

if (!isset($_GET['d'])) header('Content-Type: image/png');

ob_start();
imagepng($im);
file_put_contents('map.png', ob_get_contents());
ob_end_flush();

imagedestroy($im);

