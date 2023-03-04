<?php
// treat all notices/warnings/errors as errors
function exception_error_handler($severity, $message, $file, $line)
{
	throw new ErrorException($message, 0, $severity, $file, $line);
}
set_error_handler("exception_error_handler");

require '../inc/conf.php';
require '../inc/db.php';

$boxes = [
	['List_Of_Beacons', 'cat_nav_beacons.png', 'Beacons', 'Every beacon for all your navigation needs.'],
	['ADF', 'cat_nav_adf.png', 'ADF', 'Find your way towards a beacon.'],
	['VOR', 'cat_nav_vor.png', 'VOR', 'Line up perfectly with a runway.'],
	['ILS', 'cat_nav_ils.png', 'ILS', 'Optimize your descend into landing through any weather'],
];

ob_start();
foreach ($boxes as $box) {
	$title = $db->query("SELECT title FROM art WHERE name='{$box[0]}'")->fetchAll()[0]->title;
	echo("<table class=box>");
	echo("<tr>");
	echo("<td><a href=\"article.php?title={$box[0]}\"><img src=/s/articles/{$box[1]} alt=\"{$box[2]}\" title=\"{$box[2]}\"></a></td>");
	echo("<td><p><a href=\"article.php?title={$box[0]}\">{$title}</a><br>{$box[3]}</p></td>");
	echo("</tr>");
	echo("</table>");
}
file_put_contents('../gen/navinfotable.html', ob_get_contents());
ob_end_clean();
