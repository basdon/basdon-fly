<?php
include('../inc/bootstrap.php');

// weather
++$db_querycount;
$weather_all = [];
$weather_percategory = [0, 0, 0, 0, 0, 0];
$weather_names = [
	'LA Extra Sunny',
	'LA Sunny',
	'LA Extra Sunny Smog',
	'LA Sunny Smog',
	'LA Cloudy',
	'SF Sunny',
	'SF Extra Sunny',
	'SF Cloudy',
	'SF Rainy',
	'SF Foggy',
	'LV Sunny',
	'LV Extra Sunny',
	'LV Cloudy',
	'CS Extra Sunny',
	'CS Sunny',
	'CS Cloudy',
	'CS Rainy',
	'DE Extra Sunny',
	'DE Sunny',
	'DE Sandstorm',
	'Underwater'
];
$weather_categorymapping = [0, 1, 0, 1, 2, 1, 0, 2, 3, 4, 1, 0, 2, 0, 1, 2, 3, 0, 1, 5, 4];
$weather_categorynames = ['Clear', 'Light clouds', 'Overcast', 'Thunderstorms', 'Thick fog', 'Sandstorms'];
$weather_totalcount = 0;
$wth = $db->query('SELECT w, COUNT(w) c FROM wth GROUP BY w ORDER BY c DESC');
if ($wth !== false && ($weathercats = $wth->fetchAll())) {
	foreach ($weathercats as $w) {
		$weather_totalcount += $w->c;
		$weather_all[$w->w] = $w->c;
		$weather_percategory[$weather_categorymapping[$w->w]] += $w->c;
	}
}
arsort($weather_percategory);
unset($weather_categorymapping);

$__script = '_stats';
include('../inc/output.php');
