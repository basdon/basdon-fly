<?php 
$missiontypemap = [
	$SETTING__MISSION_TYPE_PASSENGER_S => 'Passengers (small quantity)',
	$SETTING__MISSION_TYPE_PASSENGER_M => 'Passengers (medium quantity)',
	$SETTING__MISSION_TYPE_PASSENGER_L => 'Passengers (large quantity)',
	$SETTING__MISSION_TYPE_CARGO_S => 'Cargo (small quantity)',
	$SETTING__MISSION_TYPE_CARGO_M => 'Cargo (medium quantity)',
	$SETTING__MISSION_TYPE_CARGO_L => 'Cargo (large quantity)',
	$SETTING__MISSION_TYPE_HELI => 'Helicopter (passengers)',
	$SETTING__MISSION_TYPE_HELI_CARGO => 'Helicopter (cargo)',
	$SETTING__MISSION_TYPE_MIL_HELI => 'Helicopter (military)',
	$SETTING__MISSION_TYPE_MIL => 'Military',
	$SETTING__MISSION_TYPE_PASSENGER_WATER => 'Passengers (water)',
];
function fmt_mission_type($missiontype)
{
	global $missiontypemap;

	if (isset($missiontypemap[$missiontype])) {
		return $missiontypemap[$missiontype];
	}
	return 'Unknown';
}
?>
