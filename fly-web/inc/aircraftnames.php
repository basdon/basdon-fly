<?php 
function aircraft_name($model)
{
	static $names = array(
		'417'=>'Leviathan',
		'425'=>'Hunter',
		'460'=>'Skimmer',
		'476'=>'Rustler',
		'469'=>'Sparrow',
		'487'=>'Maverick',
		'488'=>'News Chopper',
		'497'=>'Police Maverick',
		'511'=>'Beagle',
		'519'=>'Shamal',
		'520'=>'Hydra',
		'548'=>'Cargobob',
		'553'=>'Nevada',
		'563'=>'Raindance',
		'577'=>'AT-400',
		'592'=>'Andromada',
		'593'=>'Dodo',
	);
	if (array_key_exists($model, $names)) {
		return $names[$model];
	}
	return 'Unknown';
}
