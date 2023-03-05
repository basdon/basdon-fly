<?php
require '../inc/bootstrap.php';

/// TODO: delete this line when all queries are changed to work with ERRMODE_SILENT
if (!is_null($db)) $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

// weather
$weather_categorymapping = [0, 1, 0, 1, 2, 1, 0, 2, 3, 4, 1, 0, 2, 0, 1, 2, 3, 0, 1, 5, 4];
$weather_categorynames = ['Clear', 'Light clouds', 'Overcast', 'Thunderstorms', 'Thick fog', 'Sandstorms'];
$weather_iconnames = ['sun', 'cloudy', 'cloudy', 'lightning', 'clouds', 'clouds'];
function stats_query_weather_percentages()
{
	global $weather_iconnames, $weather_categorynames, $weather_categorymapping;

	$weather_count_per_weather = [];
	$weather_count_per_category = [0, 0, 0, 0, 0, 0];
	$weather_names = [
		'LA Extra Sunny', 'LA Sunny', 'LA Extra Sunny Smog', 'LA Sunny Smog', 'LA Cloudy', 'SF Sunny', 'SF Extra Sunny',
		'SF Cloudy', 'SF Rainy', 'SF Foggy', 'LV Sunny', 'LV Extra Sunny', 'LV Cloudy', 'CS Extra Sunny', 'CS Sunny',
		'CS Cloudy', 'CS Rainy', 'DE Extra Sunny', 'DE Sunny', 'DE Sandstorm', 'Underwater'
	];
	$weather_totalcount = 0;
	foreach (dbquery('SELECT w, COUNT(w) c FROM wth GROUP BY w ORDER BY c DESC')->fetch_generator() as $w) {
		$weather_totalcount += $w->c;
		$weather_count_per_weather[$w->w] = $w->c;
		$weather_count_per_category[$weather_categorymapping[$w->w]] += $w->c;
	}
	arsort($weather_count_per_category);
	foreach ($weather_count_per_category as $category => &$v) {
		$value = $v;
		$v = new stdClass();
		$v->percentage = $value > 0 ? round(100 * $value / $weather_totalcount, 2) : 0;
		$v->name = $weather_categorynames[$category];
		$v->icon = $weather_iconnames[$category];
		$v->entries = [];
		foreach ($weather_count_per_weather as $weather => $count) {
			if ($weather_categorymapping[$weather] == $category) {
				$e = new stdClass();
				$e->name = $weather_names[$weather];
				$e->percentage = $count > 0 ? round(100 * $count / $weather_totalcount, 2) : 0;
				$v->entries[] = $e;
			}
		}
		yield $v;
	}
}
function stats_query_last_weathers()
{
	global $weather_iconnames, $weather_categorynames, $weather_categorymapping;

	$now = time();
	$data = new stdClass();
	foreach (dbquery('SELECT w, t FROM wth ORDER BY t DESC LIMIT 10')->fetch_generator() as $w) {
		$category = $weather_categorymapping[$w->w];
		$data->name = $weather_categorynames[$category];
		$data->icon = $weather_iconnames[$category];
		$data->when = $now - $w->t;
		yield $data;
	}
}

$__template = '_stats';
require '../inc/output.php';
