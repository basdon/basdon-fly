<?php 
$flightstatuses = array(
	'1'=>'In progress',
	'2'=>'Abandoned',
	'4'=>'Crashed',
	'8'=>'Finished',
	'16'=>'Declined',
	'32'=>'Died',
	'64'=>'Server error'
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
?>
