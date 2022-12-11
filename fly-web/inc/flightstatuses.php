<?php 
$flightstatuses = array(
	'1'=>'In progress',
	'2'=>'Abandoned',
	'4'=>'Crashed',
	'8'=>'Landed',
	'16'=>'Aborted',
	'32'=>'Died',
	'64'=>'Server error',
	'128'=>'Connection lost',
	'256'=>'Player quit',
	'512'=>'Player kicked',
	'1024'=>'Vehicle destroyed',
	'2048'=>'Paused',
	'4096'=>'Expired',
);
function fmt_flight_status($state, $tload)
{
	global $flightstatuses;
	if ($state == 1 && $tload == 0) {
		return 'Started';
	}
	if (array_key_exists($state, $flightstatuses)) {
		return $flightstatuses[$state];
	}
	return 'Unknown?';
}
