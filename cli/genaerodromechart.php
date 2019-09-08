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

$apt= $db->query('SELECT b,x,y,z FROM apt WHERE i='.$apid)->fetchAll()[0];

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
$scale = $imgw / $sizex;
$imgh = $sizey * $scale;
$offx = -$minx;
$offy = -$miny;

$PI2 = pi() / 2;
$PI4 = pi() / 4;
$sqrt2 = sqrt(2);

$im = imagecreate($imgw, $imgh);
$color_white = $bg = imagecolorallocate($im, 255, 255, 255);
$color_black = imagecolorallocate($im, 0, 0, 0);
$color_rnw_towered = imagecolorallocate($im, 12, 136, 192);
$color_svp_inner = imagecolorallocate($im, 225, 225, 225);
$color_veh = imagecolorallocate($im, 91, 104, 119);
$color_veh_outline = imagecolorallocate($im, 64, 64, 64);
$color_msp = imagecolorallocate($im, 170, 0, 0);
$color_msp_outline = $color_black;
$color_ndb_a = imagecolorallocate($im, 157, 108, 159);
$color_ndb_b = imagecolorallocate($im, 194, 164, 194);
$font = 2;

$runway_fillings = [];
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
		imagefilledpolygon($im, $pts, 4, $color_rnw_towered);
		$w /= 3;
		$pts = [$x + $w * cos($angle + $PI4),
			$y + $w * sin($angle + $PI4),
			$x + $w * cos($angle - $PI4),
			$y + $w * sin($angle - $PI4),
			$ox - $w * cos($angle + $PI4),
			$oy - $w * sin($angle + $PI4),
			$ox - $w * cos($angle - $PI4),
			$oy - $w * sin($angle - $PI4)];
		$runway_fillings[] = $pts;
	} else {
		// helipad
		$r = $r[0];
		$x = xcoord($r->x);
		$y = ycoord($r->y);
		$w2 = $r->w / 2;
		imagefilledarc($im, xcoord($r->x), ycoord($r->y), $r->w, $r->w, 0, 360, $color_rnw_towered, IMG_ARC_PIE);
		$w4 = $r->w / 4;
		imagefilledrectangle($im, $x - 3, $y - $w4, $x - 8, $y + $w4, $color_white);
		imagefilledrectangle($im, $x + 3, $y - $w4, $x + 8, $y + $w4, $color_white);
		imagefilledrectangle($im, $x - 3, $y - 3, $x + 3, $y + 3, $color_white);
	}
}

foreach ($runway_fillings as $rf)
{
	imagefilledpolygon($im, $rf, 4, $bg);
}

// ndb
$x = xcoord($apt->x);
$y = ycoord($apt->y);
// +
imageline($im, $x -1, $y, $x + 1, $y, $color_ndb_a);
imageline($im, $x, $y - 1, $x, $y + 1, $color_ndb_a);
// o
imageellipse($im, $x, $y, 6, 6, $color_ndb_a);
// dots
$NDBSIZE = 24;
for ($i = 6; $i < $NDBSIZE; $i += 2) {
	$amt = pi() * $i;
	$dang = 2 * pi() / $amt;
	for ($ang = 0; $amt >= 0; $amt--, $ang += $dang) {
		imagesetpixel($im, $x + cos($ang) * $i, $y + sin($ang) * $i, $color_ndb_b);
	}
}
$off = $NDBSIZE * sqrt(2) / 2;
$w = strlen($apt->b) * imagefontwidth($font);
$h = imagefontheight($font);
$x -= $off + $w;
$y -= $off + $h - 2;
// text white background
imagefilledrectangle($im, $x - 1, $y - 1, $x + $w - 1, $y + $h - 3, $bg);
// text border
imagerectangle($im, $x - 2, $y - 1, $x + $w, $y + $h - 3, $color_ndb_a);
// text
imagestring($im, $font, $x, $y - 2, $apt->b, $color_ndb_a);

$mspsize = 5;
$vehsize = 3;

function rect($x, $y, $size, $col_outline, $col)
{
	global $im;
	$os = $size;
	$ss = $os - 1;
	imagefilledrectangle($im, $x - $os, $y - $os, $x + $os, $y + $os, $col_outline);
	imagefilledrectangle($im, $x - $ss, $y - $ss, $x + $ss, $y + $ss, $col);
}

foreach ($missionpoints as $p) {
	$x = xcoord($p->x);
	$y = ycoord($p->y);
	rect($x, $y, $mspsize, $color_msp_outline, $color_msp);
}

foreach ($vehicles as $v) {
	$x = xcoord($v->x);
	$y = ycoord($v->y);
	rect($x, $y, $vehsize, $color_veh_outline, $color_veh);
}

// service points, sweet icon
foreach ($servicepoints as $svp) {
	$x = xcoord($svp->x) - 6;
	$y = ycoord($svp->y) - 8;
	imagefilledrectangle($im, $x + 2, $y + 2, $x + 9, $y + 13, $color_svp_inner);
	imagefilledrectangle($im, $x + 2, $y, $x + 9, $y + 1, $color_black);
	imagefilledrectangle($im, $x, $y + 2, $x + 1, $y + 7, $color_black);
	imagefilledrectangle($im, $x + 10, $y + 2, $x + 11, $y + 6, $color_black);
	imagefilledrectangle($im, $x + 2, $y + 14, $x + 9, $y + 15, $color_black);
	imagefilledrectangle($im, $x, $y + 9, $x + 1, $y + 13, $color_black);
	imagefilledrectangle($im, $x + 10, $y + 8, $x + 11, $y + 13, $color_black);
	imagesetpixel($im, $x + 1, $y + 1, $color_black);
	imagesetpixel($im, $x + 2, $y + 2, $color_black);
	imagesetpixel($im, $x + 10, $y + 1, $color_black);
	imagesetpixel($im, $x + 9, $y + 2, $color_black);
	imagesetpixel($im, $x + 1, $y + 14, $color_black);
	imagesetpixel($im, $x + 10, $y + 14, $color_black);
	imagesetpixel($im, $x + 2, $y + 13, $color_black);
	imagesetpixel($im, $x + 9, $y + 13, $color_black);
	imagefilledrectangle($im, $x + 4, $y + 4, $x + 7, $y + 5, $color_black);
	imagefilledrectangle($im, $x + 4, $y + 10, $x + 7, $y + 11, $color_black);
	imagefilledrectangle($im, $x + 8, $y + 6, $x + 10, $y + 7, $color_black);
	imagefilledrectangle($im, $x + 1, $y + 8, $x + 3, $y + 9, $color_black);
	imagesetpixel($im, $x + 2, $y + 7, $color_black);
	imagesetpixel($im, $x + 9, $y + 8, $color_black);
	imagesetpixel($im, $x + 6, $y + 6, $color_black);
	imagesetpixel($im, $x + 7, $y + 6, $color_black);
	imagesetpixel($im, $x + 4, $y + 9, $color_black);
	imagesetpixel($im, $x + 5, $y + 9, $color_black);
}

if (!isset($_GET['d'])) header('Content-Type: image/png');

ob_start();
imagepng($im);
file_put_contents('map.png', ob_get_contents());
ob_end_flush();

imagedestroy($im);

