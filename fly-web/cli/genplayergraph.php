<?php
// treat all notices/warnings/errors as errors
function exception_error_handler($severity, $message, $file, $line)
{
	throw new ErrorException($message, 0, $severity, $file, $line);
}
set_error_handler("exception_error_handler");

require '../inc/conf.php';
require '../inc/db.php';

$imgw = 980;
$imgh = 140;

$im = imagecreate($imgw, $imgh);

$bg = imagecolorallocate($im, 255, 255, 255);
$color_fill = imagecolorallocate($im, 0xeb, 0xeb, 0x68);
$color_outline = imagecolorallocate($im, 0xA8, 0xA8, 0x4B);
$color_peak = imagecolorallocate($im, 0x69, 0x7F, 0xEA);
$color_text = imagecolorallocate($im, 0, 0, 0);
$color_hour_guide = imagecolorallocate($im, 0x80, 0x80, 0x80);

$max_num_players = 0;
$val = 0;
$rightvalue = 0;
$time_end = time() - 60;
$time_start = $time_end - 3600 * 24;
// when changing the total time, adjust the vertical hour indicator bars thingies
$values = [];
try {
	// result of query is list of timestamps with either -1 or 1
	// 1 at the end of the session, -1 at the start of the session
	// since the graph is made from right to left, values are summed to get the playercount at any time
	// (last 60 seconds are not shown on the graph, because session end times update every 30s)
	// It looks like results are squashed (ie when there are 4 players online, it only gives one row
	// instead of 4 rows with the e,1 values, why?), also selecting i seems to counter this.
	foreach ($db->query("(SELECT i,s AS stamp,-1 AS t FROM ses WHERE e>$time_start)
			UNION (SELECT i,e AS stamp,1 AS t FROM ses WHERE e>$time_start) ORDER BY stamp DESC") as $r)
	{
		$val += $r->t;
		$max_num_players = max($max_num_players, $val);
		$r->value = $val;
		if ($r->stamp < $time_end) {
			$values[] = $r;
		} else {
			$rightvalue = $val;
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
$maxy = $imgh - 20;
$miny = 3 + imagefontheight(2) + 3;
$max_value = max($max_num_players, 1);
/*basically put a value for every x of the graph*/
$o = new stdclass();
$o->stamp = $time_end;
$o->value = $rightvalue;
array_unshift($values, $o);
$value = $rightvalue;
$idx = 0;
$vals = [];
for ($x = $imgw - 1; $x >= 0; $x--) {
	$t = $time_start + ($time_end - $time_start) * ($x + 1) / $imgw;
	$newvalue = 0;
	$thisvalue = $value;
	while ($idx < count($values) - 1 && $values[$idx + 1]->stamp >= $t) {
		$idx++;
		$value = $values[$idx]->value;
		$newvalue = max($newvalue, $value);
		$thisvalue = $newvalue;
	}
	if ($thisvalue == $max_value) {
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
	$vals[] = $imgh - $miny - ($maxy - $miny) * ($thisvalue / $max_value);
}

/*smoothen out the values, give them a slope*/
$processed = array_fill(0, count($vals), 0);
for ($j = 0; $j < 5; $j++) {
	for ($i = 0; $i < count($vals) - 1; $i++) {
		$n = $i + 1;
		$cur = $vals[$i];
		$nex = $vals[$n];
		if (abs($nex - $cur) > 10) {
			if (!$processed[$n]) {
				$processed[$n] = 1;
				$vals[$n] = $cur + ($nex - $cur) * 3 / 4;
			}
			if (!$processed[$i]) {
				$processed[$i] = 1;
				$vals[$i] = $cur + ($nex - $cur) * 1 / 4;
			}
		}
	}
}

/*I guess*/
foreach ($vals as &$v) {
	$v = (int) $v;
}

/*calc points to draw*/
$x = $imgw - 1;
$lastv = $vals[0];
$points = [];
for ($i = 2; $i < count($vals) - 2; $i++) {
	$x = $imgw - 1 - $i;
	$v = $vals[$i];
	if ($v == $lastv) {
		$points[] = [$x, $v];
	} else if ($v < $lastv) {
		for ($z = $lastv - 1; $z >= $v; $z--) {
			$points[] = [$x, $z];
		}
	} else {
		for ($z = $lastv + 1; $z <= $v; $z++) {
			$points[] = [$x, $z];
		}
	}
	$lastv = $v;
}

/*draw it!*/
foreach ($points as $p) {
	imagefilledellipse($im, $p[0], $p[1], 5, 5, $color_outline);
}
foreach ($points as $p) {
	imagefilledellipse($im, $p[0], $p[1], 3, 3, $color_fill);
}

/*hour 'axis' guidelines*/
$hour_guide_time = $time_end;
$hour_guide_time_offset = 3600 - ($time_end % 3600); /*align them on the hour*/
$x = $imgw - 1 + $hour_guide_time_offset * $imgw / 24 / 3600;
$xincrement = $imgw / 24; /*one per hour*/
$texty = $imgh - imagefontheight(2);
$lt = $imgh - imagefontheight(2);
for ($i = 0; $i < 25 /*25 because we do one extra at the right*/; $i++) {
	imageline($im, $x, $imgh, $x, $lt, $color_hour_guide);
	$halfwidth = imagefontwidth(2) * 1.5;
	$textx = $x - $halfwidth - $xincrement / 2;
	if ($textx  > 0 && $textx < $imgw - $halfwidth * 2) {
		imagestring($im, 2, $textx, $texty, date('H', $hour_guide_time) . 'h', $color_hour_guide);
	}
	$hour_guide_time -= 3600;
	$x -= $xincrement;
}

/*show peak and text, if there were any players*/
if (count($peakranges)) {
	$longest = $peakranges[0];
	foreach ($peakranges as $p) {
		$len = $p->max - $p->min;
		if ($len > $longest->max - $longest->min) {
			$longest = $p;
		}
	}
	$x = $longest->min + ($longest->max - $longest->min) / 2;
	$y = $imgh - $maxy;
	imagefilledellipse($im, $x, $y, 8, 8, $color_peak);
	$txt = "peak: " . $max_num_players;
	$txtwidth = imagefontwidth(2) * strlen($txt);
	$x -= $txtwidth / 2;
	$x = max($x, 2);
	$x = min($x, $imgw - $txtwidth - 2);
	imagestring($im, 2, $x, 2, $txt, $color_text);
}

//$text = date('j M H:i O', time());
//$textx = ($imgw - imagefontwidth(2) * strlen($text)) / 2;
//$texty = $imgh - imagefontheight(2);
//imagestring($im, 2, $textx, $texty, $text, $color_text);
if ($max_num_players == 0) {
	$text = 'no players last 24h :(';
	$textx = ($imgw - imagefontwidth(2) * strlen($text)) / 2;
	$texty = $imgh - imagefontheight(2) - 30;
	imagestring($im, 2, $textx, $texty, $text, $color_text);
}

if (isset($_SERVER['REMOTE_ADDR']) && !isset($_GET['d'])) {
	header('Content-Type: image/png');
}

ob_start();
imagepng($im);
file_put_contents('../www/s/gen/playergraph.png', ob_get_contents());
if (isset($_SERVER['REMOTE_ADDR'])) {
	ob_end_flush();
} else {
	ob_end_clean();
}

imagedestroy($im);

