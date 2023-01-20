<?php
require '../inc/bootstrap.php';
include '../inc/aircraftnames.php';
include '../inc/flightstatuses.php';
include '../inc/queryflightlist.php';

$flight_list_opts = flight_list_query_get_opts_from_query_parameters();
$flight_list_opts->fetch_pagination_data = true;
$flight_list = flight_list_query($flight_list_opts);

function get_flight_list_opt($opts, $name, $default, &$num)
{
	if (isset($opts->$name)) {
		$num++;
		return $opts->$name;
	}
	return $default;
}
$num_active_filters = 0;
$filter_status = get_flight_list_opt($flight_list_opts, 'filter_status', [], $num_active_filters);
$filter_pilot_id = get_flight_list_opt($flight_list_opts, 'filter_pilot_id', '', $num_active_filters);
$filter_pilot_name = get_flight_list_opt($flight_list_opts, 'filter_pilot_name', '', $num_active_filters);

$__script = '_flights';
require '../inc/output.php';
