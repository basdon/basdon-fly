<?php 
function fmt_mission_type($missiontype)
{
	switch ($missiontype) {
	case 1: return 'Passengers (small quantity)';
	case 2: return 'Passengers (medium quantity)';
	case 4: return 'Passengers (large quantity)';
	case 8: return 'Cargo (small quantity)';
	case 16: return 'Cargo (medium quantity)';
	case 32: return 'Cargo (large quantity)';
	case 64: return 'Helicopter (passengers)';
	case 128: return 'Helicopter (cargo)';
	case 256: return 'Helicopter (military)';
	case 512: return 'Military';
	case 1024: return 'AWACS';
	case 2048: return 'Stunt';
	case 4096: return 'Cropduster';
	case 8192: return 'Passengers (small quantity, water)';
	default: return 'Unknown';
	}
}
$passenger_mission_types = (1 | 2 | 4 | 8192);
?>
