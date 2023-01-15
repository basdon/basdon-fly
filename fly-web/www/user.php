<?php
require '../inc/bootstrap.php';


if (isset($_GET['id'])) {
	$id = (int) $_GET['id'];
	++$db_querycount;
	$r = $db->query('SELECT name FROM usr WHERE i=' . $id . ' LIMIT 1');
	if ($r !== false && ($r = $r->fetchAll()) !== false && count($r)) {
		$name = $r[0]->name;
		if (substr("{$name}@", 0, 1) == '@') {
			$is_guest = true;
		}
		goto haveuser;
	}
} elseif (isset($_GET['name']) &&
	// don't allow searching guest accounts by name, because their names are not unique
	substr("{$_GET['name']}@", 0, 1) != '@')
{
	$stmt = $db->prepare('SELECT i,name FROM usr WHERE name=? LIMIT 1');
	++$db_querycount;
	$stmt->bindValue(1, $_GET['name']);
	if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r)) {
		$name = $r[0]->name;
		$id = $r[0]->i;
		goto haveuser;
	}
}
$__script = '_user^notfound';
require '../inc/output.php';
exit();

haveuser:

require '../inc/queryflightlist.php'; // required to render the user's flight list
require '../inc/aircraftnames.php'; // required to render the user's flight list
require '../inc/flightstatuses.php'; // required to render the user's flight list
require '../inc/missiontypes.php'; // required to render the user's flight list
$flight_list_opts = new stdClass();
if (isset($is_guest)) {
	$flight_list_opts->filter_pilot_id = $id;
} else {
	$flight_list_opts->filter_pilot_name = $name;
}
$flight_list_opts->fetch_pagination_data = false;
$flight_list_opts->limit = 15;
$flight_list = flight_list_query($flight_list_opts);
$flight_list->show_pilot = false;

$__script = '_user';
require '../inc/output.php';
