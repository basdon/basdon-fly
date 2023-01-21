<?php 
static $aircraft_names = array(
	'592'=>'Andromada',
	'577'=>'AT-400',
	'511'=>'Beagle',
	'548'=>'Cargobob',
	'593'=>'Dodo',
	'425'=>'Hunter',
	'520'=>'Hydra',
	'417'=>'Leviathan',
	'487'=>'Maverick',
	'553'=>'Nevada',
	'488'=>'News Chopper',
	'497'=>'Police Maverick',
	'563'=>'Raindance',
	'476'=>'Rustler',
	'519'=>'Shamal',
	'460'=>'Skimmer',
	'469'=>'Sparrow',
);
function aircraft_name($model)
{
	global $aircraft_names;

	if (array_key_exists($model, $aircraft_names)) {
		return $aircraft_names[$model];
	}
	return 'Unknown';
}
