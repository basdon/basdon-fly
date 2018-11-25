<?php

require 'tempdb.php';

$ILS_DIST = 1500;
$imgwh = 900;
$dim = 40000;
if (isset($_GET['s'])) {
	$dim = 6000;
}
if (isset($_GET['dim'])) {
	$dim = $_GET['dim'];
}
$rwwidth = max(1, (int) (5 - ($dim - 6000) / 32000 * 5));
$centerx = 0;
$centery = 0;
$scale = $imgwh / $dim;
$offx = (($dim / 2) + $centerx) * $scale;
$offy = (($dim / 2) + $centery) * $scale;

$d = isset($_GET['d']);

function xcoord($x)
{
	global $scale, $offx;
	return $x * $scale + $offx;
}

function ycoord($y)
{
	global $scale, $offy;
	return -$y * $scale + $offy;
}

$im = imagecreate($imgwh, $imgwh);
$bg = imagecolorallocate($im, 255, 255, 255);
$color_ap = imagecolorallocate($im, 11, 136, 192);
$color_ndb_a = imagecolorallocate($im, 157, 108, 159);
$color_ndb_b = imagecolorallocate($im, 194, 164, 194);
$color_ils = imagecolorallocate($im, 0, 255, 0);
$codefont = 2;

try {
	$PI2 = pi() / 2;
	$PI4 = pi() / 4;
	$sqrt2 = sqrt(2);
	$lastapcode = '';
	$ox = 0;
	$oy = 0;
	$draw = false;
	$aptexts = [];
	$vorcaps = [];
	foreach ($db->query('SELECT r.x rx,r.y ry,r.n,r.h,a.c,a.b,a.x ax,a.y ay FROM rnw r JOIN apt a ON r.a=a.i ORDER BY r.a,r.i') as $r) {
		$code = $r->c;
		if (array_key_exists($code, $aptexts)) {
			$aptexts[$code] = [max($aptexts[$code][0], xcoord($r->rx)), ycoord($r->ay)];
		} else {
			$aptexts[$code] = [xcoord($r->rx), ycoord($r->ay)];

			$vorcaps[$code] = [$r->ax, $r->ay, []];
			$ax = xcoord($r->ax);
			$ay = ycoord($r->ay);

			imageline($im, $ax -1, $ay, $ax + 1, $ay, $color_ndb_a);
			imageline($im, $ax, $ay - 1, $ax, $ay + 1, $color_ndb_a);
			imageellipse($im, $ax, $ay, 6, 6, $color_ndb_a);
			for ($i = 6; $i < 20; $i += 2) {
				$amt = pi() * $i;
				$dang = 2 * pi() / $amt;
				for ($ang = 0; $amt >= 0; $amt--, $ang += $dang) {
					imagesetpixel($im, $ax + cos($ang) * $i, $ay + sin($ang) * $i, $color_ndb_b);
				}
			}
			$off = 26 * sqrt(2) / 3;
			$x = $ax - $off - strlen($r->b) * imagefontwidth($codefont);
			$y = $ay - $off - imagefontheight($codefont);
			imagestring($im, $codefont, $x, $y, $r->b, $color_ndb_a);

			/*
			imagefilledellipse($im, $ax, $ay, 20, 20, $color_ap);
			imageline($im, $ax, $ay - 13, $ax, $ay + 13, $color_ap);
			imageline($im, $ax - 13, $ay, $ax + 13, $ay, $color_ap);
			*/
		}
		$lastapcode = $code;

		if ($r->n & 4) {
			$h = deg2rad($r->h) + $PI2;
			$x = (int) xcoord($r->rx);
			$y = (int) ycoord($r->ry);
			$dist1 = $scale * $ILS_DIST;
			$dist2 = $scale * ($ILS_DIST - 160);
			$wide = $scale * 90;
			$pts = [$x, $y,
			        $x + $dist1 * cos($h) + $wide * cos($h + $PI2), $y + $dist1 * sin($h) + $wide * sin($h + $PI2),
			        $x + $dist2 * cos($h), $y + $dist2 * sin($h),
			        $x + $dist1 * cos($h) + $wide * cos($h - $PI2), $y + $dist1 * sin($h) + $wide * sin($h - $PI2)];
			imagepolygon($im, $pts, 4, $color_ils);
		}

		if ($draw) {
			$x = (int) xcoord($r->rx);
			$y = (int) ycoord($r->ry);
			$angle = atan2($y - $oy, $x - $ox);
			$anglePI4 = $angle + $PI4;
			$angle_PI4 = $angle + $PI4;
			$w = $sqrt2 * $rwwidth;
			$pts = [$x + $w * cos($angle + $PI4),
			        $y + $w * sin($angle + $PI4),
			        $x + $w * cos($angle - $PI4),
			        $y + $w * sin($angle - $PI4),
			        $ox - $w * cos($angle + $PI4),
			        $oy - $w * sin($angle + $PI4),
			        $ox - $w * cos($angle - $PI4),
			        $oy - $w * sin($angle - $PI4)];
			imagefilledpolygon($im, $pts, 4, $color_ap);
			$w = min($rwwidth - 1, 1) * $sqrt2;
			$pts = [$x + $w * cos($angle + $PI4),
				$y + $w * sin($angle + $PI4),
				$x + $w * cos($angle - $PI4),
				$y + $w * sin($angle - $PI4),
				$ox - $w * cos($angle + $PI4),
				$oy - $w * sin($angle + $PI4),
				$ox - $w * cos($angle - $PI4),
				$oy - $w * sin($angle - $PI4)];
			imagefilledpolygon($im, $pts, 4, $bg);
			$draw = false;
		} else {
			$ox = (int) xcoord($r->rx);
			$oy = (int) ycoord($r->ry);
			$draw = true;
		}
	}
	foreach ($aptexts as $code => $t) {
		imagestring($im, $codefont, $t[0] + 10, $t[1] - imagefontheight($codefont), $code, $color_ap);
	}
} catch (PDOException $e) {
	die($e->getMessage());
}

if (!$d) header('Content-Type: image/png');

ob_start();
imagepng($im);
file_put_contents('map.png', ob_get_contents());
ob_end_flush();

imagedestroy($im);

