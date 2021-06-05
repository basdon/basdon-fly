<?php
// parses minimap map files and runways in db to create one file with all polygon coordinates
// output is an array with whole numbers:
// - first number is amount of runways, and for each runway:
//   - number for heading/angle (deg), if negative it means it's untowered
//   - two numbers for xy (one end of the runway, extends in the direction of the heading/angle)
//   - number for length
//   - number for width
// - then an indefinite amount of rectangle clusters with:
//   - number for amount of rectangles
//   - two numbers for xy top-left position of the cluster
//   - for each rectangle:
//     - four numbers: minx, miny, maxx, maxy (relative to top-left position of cluster)
$mapdata = [];

require '../inc/conf.php';
require '../inc/db.php';

// runways
$runways_by_airport = [];
$mapdata[0] = 0;
$runwaydata = [];
foreach($db->query('SELECT r.a,r.x,r.y,r.i,r.h,r.w,a.flags FROM rnw r JOIN apt a ON r.a=a.i WHERE r.type=1') as $r) {
	if (!isset($runways_by_airport[$r->a])) {
		$runways_by_airport[$r->a] = [];
	}
	if (isset($runways_by_airport[$r->a][$r->i])) {
		$rw = $runways_by_airport[$r->a][$r->i];
		$dx = $rw->x - $r->x;
		$dy = $rw->y - $r->y;
		$mapdata[] = ($rw->flags & 1) ? round($rw->h) : -round($rw->h);
		$mapdata[] = round($rw->x);
		$mapdata[] = round($rw->y);
		$mapdata[] = round(sqrt($dx * $dx + $dy * $dy));
		$mapdata[] = round($rw->w);
		$mapdata[0]++;
	} else {
		$runways_by_airport[$r->a][$r->i] = $r;
	}
}

// mainland rectangle cluster
$mapdata[] = 1;
$mapdata[] = -3000;
$mapdata[] = -3000;
$mapdata[] = 0;
$mapdata[] = 0;
$mapdata[] = 6000;
$mapdata[] = 6000;

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
		$numzones = unpack('V', substr($data, 16, 4))[1];
		if (!$numzones) {
			fwrite(STDERR, "minimap map file has no zones: {$filename}\n");
			exit(1);
		}
		$numremoves = unpack('V', substr($data, 4, 4))[1];
		$offset = 32 + 20 * $numremoves;
		$cminx = 9e9;
		$cminy = 9e9;
		$zones = [];
		while ($numzones--) {
			$c = unpack('f4', substr($data, $offset, 16)); $offset += 20;
			$z = $zones[] = new stdClass();
			$z->minx = $c[1];
			$z->miny = -$c[2];
			$z->maxx = $c[3];
			$z->maxy = -$c[4];
			$cminx = min($cminx, $z->minx);
			$cminy = min($cminy, $z->miny);
		}
		$mapdata[] = count($zones);
		$mapdata[] = round($cminx);
		$mapdata[] = round($cminy);
		foreach ($zones as $zd) {
			$mapdata[] = round($zd->minx - $cminx);
			$mapdata[] = round($zd->miny - $cminy);
			$mapdata[] = round($zd->maxx - $cminx);
			$mapdata[] = round($zd->maxy - $cminy);
		}
	}
}

// write
file_put_contents('../gen/islandmapdata.txt', '['.implode(',', $mapdata).'];');
