<?php
include('../inc/conf.php');
include('../inc/funcs.php');
header('Content-Type: image/png');

// set up image
$imgw = 400;
$imgh = 200;
$im = imagecreatetruecolor($imgw, $imgh);
$bg = imagecolorallocate($im, 100, 134, 164);
$fill = imagecolorallocate($im, 204, 204, 204);
//$rw = [imagecolorallocate($im, 156, 107, 159), imagecolorallocate($im, 12, 136, 192)];
imagefilledrectangle($im, 0, 0, $imgw, $imgh, $bg);

// prechecks
if (isset($_GET['id'])) {
	$id = (int) $_GET['id'];
}
if (!isset($id) || $id < 0 || $id > 2000000) {
	imagestring($im, 5, 20, 20, 'no flight given', $fill);
	goto output;
}

// read flight file
$filename = '../static/fdr/'.$id.'.flight';
if (!file_exists($filename)) {
	imagestring($im, 5, 20, 20, 'no flight data', $fill);
	goto output;
}
$filesize = filesize($filename);
if ($filesize < 40) {
	imagestring($im, 5, 20, 20, 'no flight data', $fill);
	goto output;
}
$f = @fopen($filename, 'rb');
$data = fread($f, $filesize);
fclose($f);

// parse flight file
$v = unpack('V', $data)[1];
if (($v & 0xFFFFFF00) != 0x594C4600 || (($v & 0xFF) < 3 || ($v & 0xFF) > 5)) {
	imagestring($im, 5, 20, 20, 'flight file has incorrect version', $fill);
	goto output;
}
$v &= 0xFF;
if (unpack('V', substr($data, 4, 4))[1] != $id) {
	imagestring($im, 5, 20, 20, 'flight file has wrong flight id', $fill);
	goto output;
}
$chunksize = 24;
if ($v > 3) {
	$chunksize = 28;
}
$num_frames = ($filesize - 40);
if ($num_frames % $chunksize) {
	imagestring($im, 5, 20, 20, 'flight file has extra bytes: '.($num_frames % $chunksize), $fill);
	goto output;
}
$num_frames /= $chunksize;
if ($num_frames < 2) {
	imagestring($im, 5, 20, 20, 'flight file has lt 2 frames', $fill);
	goto output;
}
$minalt = 9e9; $maxalt = -9e9;
$minx = 9e9; $maxx = -9e9;
$miny = 9e9; $maxy = -9e9;
$offset = 40;
$posx = [];
$posy = [];
$altat = [];
for ($i = 0; $i < $num_frames; $i++) {
	if ($v == 4) {
		$alt = 0;
	} else {
		$alt = unpack('s', substr($data, $offset + 8, 2))[1];
	}
	list(, $x, $y) = unpack('f2', substr($data, $offset + 16, 8));
	$y = -$y;
	$minalt = min($minalt, $alt);
	$maxalt = max($maxalt, $alt);
	$minx = min($minx, $x);
	$maxx = max($maxx, $x);
	$miny = min($miny, $y);
	$maxy = max($maxy, $y);
	$altat[$i] = $alt;
	$posx[$i] = $x;
	$posy[$i] = $y;
	$offset += $chunksize;
}
$altrange = max(1, $maxalt - $minalt);

// col funcs
function huestuff($t)
{
	if ($t < 0) $t += 1;
	else if ($t > 1) $t -= 1;
	if ($t < 1/6) return 255 * 6 * $t;
	if ($t < 1/2) return 255;
	if ($t < 2/3) return 255 * (4 - 6 * $t);
	return 0;
}
function colforhue($hue)
{
	global $im;
	static $cols = [];
	$hueclamped = (int) ($hue * 253);
	if (isset($cols[$hueclamped])) {
		return $cols[$hueclamped];
	}
	$hue = $hueclamped / 253;
	$r = huestuff($hue + 1/3);
	$g = huestuff($hue);
	$b = huestuff($hue - 1/3);
	return $cols[$hueclamped] = [imagecolorallocate($im, $r, $g, $b), imagecolorallocatealpha($im, $r, $g, $b, 80)];
}

// calculate viewport
$map_vsize_x = $maxx - $minx;
$map_vsize_y = $maxy - $miny;
$imgxyratio = $imgw / $imgh;
$mapxyratio = $map_vsize_x / $map_vsize_y;
if ($mapxyratio > $imgw / $imgh) {
	$old_map_vsize_y = $map_vsize_y;
	$map_vsize_y = $map_vsize_x / $imgxyratio;
	$miny -= ($map_vsize_y - $old_map_vsize_y) / 2;
} else {
	$old_map_vsize_x = $map_vsize_x;
	$map_vsize_x = $map_vsize_y * $imgxyratio;
	$minx -= ($map_vsize_x - $old_map_vsize_x) / 2;
}
// padding
$pad = 10;
$dx = $map_vsize_x / $imgw * $pad;
$minx -= $dx;
$map_vsize_x += $dx * 2;
$dy = $map_vsize_y / $imgh * $pad;
$miny -= $dy;
$map_vsize_y += $dy * 2;
$scale_x = $imgw / $map_vsize_x;
$scale_y = $imgh / $map_vsize_y;

// draw mainland
$x1 = $scale_x * (-3000 - $minx);
$y1 = $scale_y * (-3000 - $miny);
$x2 = $scale_x * (3000 - $minx);
$y2 = $scale_y * (3000 - $miny);
imagefilledrectangle($im, $x1, $y1, $x2, $y2, $fill);

// read all minimap map files, draw ones that are in the viewport
foreach (scandir('../../maps', SCANDIR_SORT_NONE) as $file) {
	if (strpos($file, '_minimap.map') !== false) {
		$filename = "../../maps/{$file}";
		$filesize = filesize($filename);
		if ($filesize < 32) {
			fwrite(STDERR, "invalid map file: {$filename}\n");
			exit(1);
		}
		$f = fopen($filename, 'rb');
		if (!$f) {
			fwrite(STDERR, "can't open: {$filename}\n");
			exit(1);
		}
		$data = fread($f, $filesize);
		fclose($f);
		if (ord($data[3]) != 3) {
			fwrite(STDERR, 'invalid map version: '.ord($data[3])."\n");
			exit(1);
		}
		// assuming the map file is correctly structured
		$numremoves = unpack('V', substr($data, 4, 4))[1];
		$numzones = unpack('V', substr($data, 16, 4))[1];
		$offset = 32 + 20 * $numremoves;
		while ($numzones--) {
			list(, $zminx, $zminy, $zmaxx, $zmaxy) = unpack('f4', substr($data, $offset, 16));
			$zminy = -$zminy;
			$zmaxy = -$zmaxy;
			if ((($minx < $zminx && $zminx < $maxx) || ($minx < $zmaxx && $zmaxx < $maxx)) &&
				(($miny < $zminy && $zminy < $maxy) || ($miny < $zmaxy && $zmaxy < $maxy)))
			{
				$x1 = $scale_x * ($zminx - $minx);
				$y1 = $scale_y * ($zminy - $miny);
				$x2 = $scale_x * ($zmaxx - $minx);
				$y2 = $scale_y * ($zmaxy - $miny);
				imagefilledrectangle($im, $x1, $y1, $x2, $y2, $fill);
			}
			$offset += 20;
		}
	}
}

// draw flight path
$ax = $scale_x * ($posx[0] - $minx);
$ay = $scale_y * ($posy[0] - $miny);
$aa = (1 - ($altat[0] - $minalt) / $altrange) * 240 / 360;
for ($i = 1; $i < $num_frames; $i++) {
	$bx = $scale_x * ($posx[$i] - $minx);
	$by = $scale_y * ($posy[$i] - $miny);
	$ba = (1 - ($altat[$i] - $minalt) / $altrange) * 240 / 360;
	$steps = max(1, abs($bx - $ax), abs($by - $ay));
	for ($s = 0; $s < $steps; $s++) {
		$t = $s / $steps;
		$x = $ax + ($bx - $ax) * $t;
		$y = $ay + ($by - $ay) * $t;
		$c = colforhue($aa + ($ba - $aa) * $t);
		imagefilledellipse($im, $x, $y, 3, 3, $c[0]);
		imagefilledellipse($im, $x, $y, 5, 5, $c[1]);
	}
	$ax = $bx;
	$ay = $by;
	$aa = $ba;
}

// output
output:
imagetruecolortopalette($im, /*dither*/ false, /*num_colors*/ 20);
imagepng($im);
imagedestroy($im);
