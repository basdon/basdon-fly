<?php

require 'tempdb.php';

$imgw = 220;
$imgh = 80;

$d = isset($_GET['d']);

$im = imagecreate($imgw, $imgh);

$bg = imagecolorallocate($im, 0xd8, 0xd8, 0xd8);
$color_fill = imagecolorallocate($im, 0xeb, 0xeb, 0x68);
$color_outline = imagecolorallocate($im, 0xA8, 0xA8, 0x4B);
$color_peak = imagecolorallocate($im, 0x69, 0x7F, 0xEA);
$color_text = imagecolorallocate($im, 0, 0, 0);

$max = 0;
$val = 0;
$te = time() - 60;
$ts = $te - 3600 * 48;
$values = [];
try {
	foreach ($db->query("(SELECT s AS stamp,-1 AS t FROM ses WHERE e>$ts)
			UNION (SELECT e AS stamp,1 AS t FROM ses WHERE e>$ts) ORDER BY stamp DESC") as $r)
	{
		$val += $r->t;
		$max = max($max, $val);
		$r->value = $val;
		if ($r->stamp < $te) {
			$values[] = $r;
		}
	}
} catch (PDOException $e) {
	die($e->getMessage());
}

$o = new stdclass();
$o->stamp = 0;
$o->value = 0;
$values[] = $o;

$peakranges = [];
$inpeak = false;
$maxy = $imgh - 12;
if ($max > 0) {
	if ($values[0]->stamp < $te) {
		$o = new stdclass();
		$o->stamp = $te;
		$o->value = 0;
		array_unshift($values, $o);
	}
	$value = $values[0]->value;
	$idx = 0;
	$lasty = -1;
	for ($x = $imgw - 1; $x >= 0; $x--) {
		$t = $ts + ($te - $ts) * ($x + 1) / $imgw;
		$newvalue = 0;
		$thisvalue = $value;
		while ($idx < count($values) - 1 && $values[$idx + 1]->stamp >= $t) {
			$idx++;
			$value = $values[$idx]->value;
			$newvalue = max($newvalue, $value);
			$thisvalue = $newvalue;
		}
		if ($thisvalue == $max) {
			if ($inpeak) {
				$peakranges[0]->min = $x;
			} else {
				$peak = new stdclass();
				$peak->min = $x;
				$peak->max = $x;
				array_unshift($peakranges, $peak);
				$inpeak = true;
			}
		} else {
			$inpeak = false;
		}
		$ytop = $imgh - $maxy * $thisvalue;
		imageline($im, $x, $imgh, $x, $ytop / $max, $color_fill);
		imagesetpixel($im, $x, $ytop, $color_outline);
		if ($lasty != -1 && $lasty != $ytop) {
			imageline($im, $x + 1, $lasty, $x + 1, $ytop, $color_outline);
		}
		$lasty = $ytop;
	}
} else {
	$txt = 'no players last 48h :(';
	imagestring($im, 2, ($imgw - imagefontwidth(2) * strlen($txt)) / 2, 2, $txt, $color_text);
}

if (count($peakranges)) {
	$longest = $peakranges[0];
	foreach ($peakranges as $p) {
		$len = $p->max - $p->min;
		if ($len > $longest->max - $longest->min) {
			$longest = $p;
		}
	}
	$x = $longest->min + ($longest->max - $longest->min) / 2;
	$y = $imgh - $maxy * $max;
	imagefilledrectangle($im, $x - 2, $y - 2, $x + 2, $y + 2, $color_peak);
	$txt = "peak: " . $max;
	if ($x > $imgw / 2) {
		$x -= imagefontwidth(2) * strlen($txt);
	}
	imagestring($im, 2, $x, -2, $txt, $color_text);
}

$lastupdate = date('j M H:i O', time());
imagestring($im, 2, ($imgw - imagefontwidth(2) * strlen($lastupdate)) / 2, $imgh - imagefontheight(2), $lastupdate, $color_text);

if (!$d) header('Content-Type: image/png');

ob_start();
imagepng($im);
file_put_contents('../static/gen/playergraph.png', ob_get_contents());
ob_end_flush();

imagedestroy($im);

