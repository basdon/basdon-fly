<?php
require '../inc/bootstrap.php';

// weather
++$db_querycount;
$weather_percentage_per_weather = [];
$weather_percentage_per_category = [0, 0, 0, 0, 0, 0];
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
$weather_iconnames = ['sun', 'cloudy', 'cloudy', 'lightning', 'clouds', 'clouds'];
$weather_totalcount = 0;
$wth = $db->query('SELECT w, COUNT(w) c FROM wth GROUP BY w ORDER BY c DESC');
if ($wth !== false && ($weathercats = $wth->fetchAll())) {
	foreach ($weathercats as $w) {
		$weather_totalcount += $w->c;
		$weather_percentage_per_weather[$w->w] = $w->c;
		$weather_percentage_per_category[$weather_categorymapping[$w->w]] += $w->c;
	}
}
arsort($weather_percentage_per_category);
$last_weathers = $db->query('SELECT w, t FROM wth ORDER BY t DESC LIMIT 10');

$__script = '_stats';
require '../inc/output.php';
