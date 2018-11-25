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
if ($dim > 40000) {
	die();
}
$rwwidth = max(1, (int) (5 - ($dim - 6000) / 32000 * 5));
$centerx = 0;
$centery = 0;
$scale = $imgwh / $dim;
$offx = (($dim / 2) + $centerx) * $scale;
$offy = (($dim / 2) + $centery) * $scale;

$NDBSIZE = 22 - (12 / (40000 - 6000) * ($dim - 6000));
$SHOWRUNWAYIDS = $dim < 15000;
$DRAWTEXT = $dim < 25000;

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

if (!isset($_GET['watercolors'])) {
	$bg = imagecolorallocate($im, 255, 255, 255);
	$color_ap = imagecolorallocate($im, 11, 136, 192);
	$color_ndb_a = imagecolorallocate($im, 157, 108, 159);
	$color_ndb_b = imagecolorallocate($im, 194, 164, 194);
	$color_ils = imagecolorallocate($im, 0, 255, 0);
	$color_res = imagecolorallocate($im, 0, 255, 0);
	$color_special = imagecolorallocate($im, 87, 128, 192);
	$color_white = $bg;
	$color_ap_text = $color_ap;
} else {
	$bg = imagecolorallocate($im, 111, 136, 169);
	$color_ap = imagecolorallocate($im, 13, 89, 122);
	$color_ndb_a = imagecolorallocate($im, 125, 86, 127);
	$color_ndb_b = imagecolorallocate($im, 150, 104, 153);
	$color_ils = imagecolorallocate($im, 0, 255, 0);
	$color_white = imagecolorallocate($im, 255, 255, 255);
	$color_special = imagecolorallocate($im, 87, 128, 192);
	$color_black = imagecolorallocate($im, 0, 0, 0);
	$color_ap_text = $color_black;
}

$codefont = 2;

$res = [];
$res[] = [-91.586, 2123.01, 421.234, 1655.05];

foreach ($res as $r) {
	$x1 = (int) xcoord($r[0]);
	$y1 = (int) ycoord($r[1]);
	$x2 = (int) xcoord($r[2]);
	$y2 = (int) ycoord($r[3]);
	while (($x2 - $x1 + 1) % 3 > 0) $x2++;
	while (($y2 - $y1 + 1) % 3 > 0) $y2++;

	$len = 7;

	imagerectangle($im, $x1 - 1, $y1 - 1, $x2 + 1, $y2 + 1, $color_special);
	imagerectangle($im, $x1 - 2, $y1 - 2, $x2 + 2, $y2 + 2, $color_special);

	for ($i = $x1 + 1; $i < $x2; $i += 3) {
		$l = min($len, min($i - $x1, $x2 - $i));
		imageline($im, $i, $y1, $i, $y1 + $l, $color_special);
		imageline($im, $i, $y2 - $l, $i, $y2, $color_special);
	}
	for ($i = $y1 + 1; $i < $y2; $i += 3) {
		$l = min($len, min($i - $y1, $y2 - $i));
		imageline($im, $x1, $i, $x1 + $l, $i, $color_special);
		imageline($im, $x2 - $l, $i, $x2, $i, $color_special);
	}
}

$appos = [];
$vorcaps = [];
try {
	$PI2 = pi() / 2;
	$PI4 = pi() / 4;
	$sqrt2 = sqrt(2);
	$lastapcode = '';
	$ox = 0;
	$oy = 0;
	$draw = false;
	foreach ($db->query('SELECT r.x rx,r.y ry,r.n,r.h,r.s,a.c,a.b,a.x ax,a.y ay FROM rnw r JOIN apt a ON r.a=a.i ORDER BY r.a,r.i') as $r) {
		$code = $r->c;
		if (array_key_exists($code, $appos)) {
			$appos[$code][0] = max($appos[$code][0], xcoord($r->rx));
			$appos[$code][1] = min($appos[$code][1], ycoord($r->ry));
			$appos[$code][2] = max($appos[$code][2], ycoord($r->ry));
		} else {
			$appos[$code] = [xcoord($r->rx), ycoord($r->ry), ycoord($r->ry)];

			$vorcaps[$code] = [];
			$ax = xcoord($r->ax);
			$ay = ycoord($r->ay);

			imageline($im, $ax -1, $ay, $ax + 1, $ay, $color_ndb_a);
			imageline($im, $ax, $ay - 1, $ax, $ay + 1, $color_ndb_a);
			imageellipse($im, $ax, $ay, 6, 6, $color_ndb_a);
			for ($i = 6; $i < $NDBSIZE; $i += 2) {
				$amt = pi() * $i;
				$dang = 2 * pi() / $amt;
				for ($ang = 0; $amt >= 0; $amt--, $ang += $dang) {
					imagesetpixel($im, $ax + cos($ang) * $i, $ay + sin($ang) * $i, $color_ndb_b);
				}
			}

			if ($DRAWTEXT) {
				$off = $NDBSIZE * sqrt(2) / 2;
				$x = $ax - $off - strlen($r->b) * imagefontwidth($codefont);
				$y = $ay - $off - imagefontheight($codefont) + 4;
				imagestring($im, $codefont, $x, $y, $r->b, $color_ndb_a);
			}

			/*
			imagefilledellipse($im, $ax, $ay, 20, 20, $color_ap);
			imageline($im, $ax, $ay - 13, $ax, $ay + 13, $color_ap);
			imageline($im, $ax - 13, $ay, $ax + 13, $ay, $color_ap);
			*/
		}
		$lastapcode = $code;

		$txt = ' ' . sprintf('%02d', round($r->h / 10)) . $r->s;
		if ($r->n & 2) {
			$txt[0] = '^';
		}
		if ($r->n & 4) {
			$txt[0] = '*';
		}
		$vorcaps[$code][] = $txt;

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
} catch (PDOException $e) {
	die($e->getMessage());
}

if ($DRAWTEXT) {
	foreach ($appos as $code => $pos) {
		$fh = imagefontheight($codefont) - 4;
		$bh = $fh + 2;
		$bw = imagefontwidth($codefont) * 4 + 2;
		$bhh = $SHOWRUNWAYIDS ? (2 + count($vorcaps[$code]) * $fh) : 0;
		$x = $pos[0] + 12;
		$y = ($pos[2] + $pos[1] - $bh - $bhh) / 2;
		$bx = $x - 2;
		$by = $y + 1;

		imagefilledrectangle($im, $bx, $by, $bx + $bw, $by + $bh + $bhh, $color_white);
		imagerectangle($im, $bx, $by, $bx + $bw, $by + $bh, $color_ap_text);
		imagestring($im, $codefont, $x, $y, $code, $color_ap_text);

		if ($SHOWRUNWAYIDS) {
			imagerectangle($im, $bx, $by, $bx + $bw, $by + $bh + $bhh, $color_ap_text);

			foreach ($vorcaps[$code] as $key => $v) {
				imagestring($im, $codefont, $x, $y + 2 + ($key + 1) * $fh, $v, $color_ap_text);
			}
		}
	}
}

if (!$d) header('Content-Type: image/png');

ob_start();
imagepng($im);
file_put_contents('map.png', ob_get_contents());
ob_end_flush();

imagedestroy($im);

