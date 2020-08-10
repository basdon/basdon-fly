<?php

require '../inc/conf.php';
require '../inc/db.php';
require '../templates/vehiclenames.php';

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

$aptcodes = [];
if (isset($_GET['id'])) {
	$aptcodes[] = $db->query('SELECT c FROM apt WHERE i='.$_GET['id'])->fetchAll()[0]->c;
} else {
	foreach ($db->query('SELECT c FROM apt') as $r) {
		$aptcodes[] = $r->c;
	}
}

nextap:

$apcode = array_pop($aptcodes);
if ($apcode == null) {
	exit();
}

$maxx = -100000;
$maxy = -100000;
$minx = 100000;
$miny = 100000;

$apt = $db->query('SELECT i,c,n,x,y,z,flags FROM apt WHERE c=\''.$apcode.'\'')->fetchAll()[0];
$apid = $apt->i;

$runway_ends = [];
foreach ($db->query('SELECT x,y,z,w,i,h,s,n FROM rnw WHERE a='.$apid.' ORDER BY i') as $r) {
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
foreach ($db->query('SELECT name,x,y,z,t FROM msp WHERE a='.$apid) as $r) {
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

$spawns = [];
foreach ($db->query('SELECT class,sx,sy FROM spw WHERE ap='.$apid) as $s) {
	$spawns[] = $s;
	$maxx = max($maxx, $s->sx);
	$maxy = max($maxy, $s->sy);
	$minx = min($minx, $s->sx);
	$miny = min($miny, $s->sy);
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

$PI2 = pi() / 2;
$PI4 = pi() / 4;
$sqrt2 = sqrt(2);

$im = imagecreate($imgw, $imgh);
imagealphablending($im, false);
imagesavealpha($im, true);
$bg = imagecolorallocate($im, 244, 244, 244);
imagecolortransparent($im, $bg);
$color_black = imagecolorallocate($im, 0, 0, 0);
if ($apt->flags & 1 /*APT_FLAG_TOWERED*/) {
	$color_rnw = imagecolorallocate($im, 12, 136, 192);
} else {
	$color_rnw = imagecolorallocate($im, 156, 107, 159);
}
$color_svp_inner = imagecolorallocate($im, 225, 225, 225);
$color_veh = imagecolorallocate($im, 91, 104, 119);
$color_veh_outline = imagecolorallocate($im, 64, 64, 64);
$color_msp = imagecolorallocate($im, 170, 0, 0);
$color_msp_outline = $color_black;
$color_ndb_a = imagecolorallocate($im, 157, 108, 159);
$color_ndb_b = imagecolorallocate($im, 194, 164, 194);
$color_ils = imagecolorallocate($im, 0, 255, 0);
$color_spawn = imagecolorallocate($im, 0, 168, 30);
$color_spawn_outline = imagecolorallocate($im, 0, 128, 30);
$font = 2;

// ap name
bordered_text($imgw / 2, imagefontheight($font) / 2, $apt->n, $color_black);

$runway_fillings = [];
$runway_texts = [];
foreach ($runway_ends as $r) {
	if (count($r) == 2) {

		// ils feathers
		for ($i = 0; $i < 2; $i++) {
			if ($r[$i]->n & 4) {
				$h = deg2rad($r[$i]->h) + $PI2;
				$x = (int) xcoord($r[$i]->x) - cos($h) * 30;
				$y = (int) ycoord($r[$i]->y) - sin($h) * 30;
				$ILS_DIST = 1500;
				$d1 = $scale * $ILS_DIST;
				$d2 = $scale * ($ILS_DIST - 160);
				$w = $scale * 90;
				$pts = [$x, $y,
					$x + $d1 * cos($h) + $w * cos($h + $PI2), $y + $d1 * sin($h) + $w * sin($h + $PI2),
					$x + $d2 * cos($h), $y + $d2 * sin($h),
					$x + $d1 * cos($h) + $w * cos($h - $PI2), $y + $d1 * sin($h) + $w * sin($h - $PI2)];
				imagepolygon($im, $pts, 4, $color_ils);
			}
		}

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
		$txt = new stdClass();
		$txt->angle = $angle + pi();
		$txt->x = $ox;
		$txt->y = $oy;
		$txt->h = $r[0]->h;
		$txt->s = $r[0]->s;
		$txt->n = $r[0]->n;
		$runway_texts[] = $txt;
		$txt = new stdClass();
		$txt->angle = $angle;
		$txt->x = $x;
		$txt->y = $y;
		$txt->h = $r[1]->h;
		$txt->s = $r[1]->s;
		$txt->n = $r[1]->n;
		$runway_texts[] = $txt;
	} else {
		// helipad
		$r = $r[0];
		$x = xcoord($r->x);
		$y = ycoord($r->y);
		$w2 = $r->w / 2;
		imagefilledarc($im, xcoord($r->x), ycoord($r->y), $r->w, $r->w, 0, 360, $color_rnw, IMG_ARC_PIE);
		$w4 = $r->w / 4;
		imagefilledrectangle($im, $x - 3, $y - $w4, $x - 8, $y + $w4, $bg);
		imagefilledrectangle($im, $x + 3, $y - $w4, $x + 8, $y + $w4, $bg);
		imagefilledrectangle($im, $x - 3, $y - 3, $x + 3, $y + 3, $bg);
	}
}

foreach ($runway_fillings as $rf)
{
	imagefilledpolygon($im, $rf, 4, $bg);
}

function bordered_text($x, $y, $txt, $col)
{
	global $im, $font, $bg;
	$w = strlen($txt) * imagefontwidth($font);
	$h = imagefontheight($font);
	$x -= $w / 2;
	$y -= $h / 2 - 2;
	// text white background
	imagefilledrectangle($im, $x - 1, $y - 1, $x + $w - 1, $y + $h - 3, $bg);
	// text border
	imagerectangle($im, $x - 2, $y - 1, $x + $w, $y + $h - 3, $col);
	// text
	imagestring($im, $font, $x, $y - 2, $txt, $col);
}

foreach ($runway_texts as $t)
{
	$x = $t->x + 30 * cos($t->angle);
	$y = $t->y + 30 * sin($t->angle);
	$txt = ' ' . sprintf('%02d', round($t->h / 10)) . $t->s;
	if ($t->n & 2) {
		$txt[0] = '^';
	}
	if ($t->n & 4) {
		$txt[0] = '*';
	}
	bordered_text($x, $y, $txt, $color_rnw);
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
$off = $NDBSIZE * sqrt(2) / 2 + 7;
bordered_text($x - $off, $y - $off, $apt->c, $color_ndb_a);

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

if (isset($_GET['web'])) {
	if (!isset($_GET['d'])) header('Content-Type: image/png');
}

ob_start();
imagepng($im);
imagedestroy($im);
$imagedata = ob_get_clean();
file_put_contents('../static/gen/aerodromev2_'.$apt->c.'.png', $imagedata);

ob_start();
?>
<link rel="stylesheet" href="<?=$STATICPATH?>/aerodromechart.css" type="text/css" />
<input id="aer-show" type="checkbox" checked="" /><label for="aer-show">Show chart (wide content)</label><br/>
<div class="aechart" style="min-width:<?=$imgw?>px;min-height:<?=$imgh?>px">
	<img src="<?=$STATICPATH?>/gen/aerodromev2_<?=$apt->c?>.png" alt="<?=$apcode?> aerodrome chart"/>
	<?php if(count($vehicles) || count($missionpoints) || count($spawns)) {?>
		<div class="toggles">
			<input type="checkbox" checked="checked" id="aer-0" class="hide"/>
			<p><label for="aer-0" class="toggle">Select elements to show</label></p>
			<div>
				<?php if(count($vehicles)) { ?>
					<input type="checkbox" checked="checked" id="aer-veh" class="hide"/>
					<p><label for="aer-veh" class="toggle">Vehicles</label></p>
					<div>
						<?php $vcounts = []; ?>
						<?php foreach ($vehicles as $v) { ?>
							<?php
								$text = vehicle_name($v->m);
								if (isset($vcounts[$v->m])) {
									$text .= ' (' . ++$vcounts[$v->m] . ')';
								} else {
									$vcounts[$v->m] = 1;
								}
							?>
							<p><label><input type="checkbox" checked="checked"/><span class="veh" style="left:<?=round($imgw*($v->x-$minx)/($maxx-$minx)-4,1)?>px;top:<?=round($imgh-$imgh*($v->y-$miny)/($maxy-$miny)-4,1)?>px" title="<?=$text?>"></span><?=$text?></label></p>
						<?php } ?>
					</div>
				<?php } ?>
				<?php if(count($missionpoints)) { ?>
					<input type="checkbox" checked="checked" id="aer-msp" class="hide"/>
					<p><label for="aer-msp" class="toggle">Mission points</label></p>
					<div>
						<?php foreach ($missionpoints as $p) { ?>
							<?php
								$text = $p->name;
								if ($p->t & (1 | 2 | 4)) {
									$text .= ' (passengers)';
								} else if ($p->t & (8 | 16 | 32)) {
									$text .= ' (cargo)';
								} else if ($p->t & (64 | 128)) {
									$text .= ' (heli)';
								} else if ($p->t & (256)) {
									$text .= ' (military heli)';
								} else if ($p->t & (512)) {
									$text .= ' (military)';
								}
							?>
							<p><label><input type="checkbox" checked="checked"/><span class="msp" style="left:<?=round($imgw*($p->x-$minx)/($maxx-$minx)-7,1)?>px;top:<?=round($imgh-$imgh*($p->y-$miny)/($maxy-$miny)-7,1)?>px" title="<?=$text?>"></span><?=$text?></label></p>
						<?php } ?>
					</div>
				<?php } ?>
				<?php if(count($spawns)) { ?>
					<input type="checkbox" checked="checked" id="aer-spw" class="hide"/>
					<p><label for="aer-spw" class="toggle">Spawn points</label></p>
					<div>
						<?php foreach ($spawns as $s) { ?>
							<?php
								$text = '';
								foreach ([1=>'pilot',2=>'rescue',4=>'army',8=>'aid worker',16=>'trucker'] as $val => $name) {
									if ($s->class & $val) {
										if (strlen($text)) $text .= '/';
										$text .= $name;
									}
								}
							?>
							<p><label><input type="checkbox" checked="checked"/><span class="spw" style="left:<?=round($imgw*($s->sx-$minx)/($maxx-$minx)-5,1)?>px;top:<?=round($imgh-$imgh*($s->sy-$miny)/($maxy-$miny)-6,1)?>px" title="<?=$text?>"></span><?=$text?></label></p>
						<?php } ?>
					</div>
				<?php } ?>
			</div>
		</div>
	<?php } ?>
</div>
<?php
$htmldata = ob_get_clean();
$htmldata = str_replace("\t", "", $htmldata);
$htmldata = str_replace("\n", "", $htmldata);
file_put_contents('../articles/aerodromecharts/'.$apt->c.'.html', $htmldata);

if (isset($_GET['id'])) {
	echo $htmldata;
}

goto nextap;
