<?php
require '../inc/bootstrap.php';
include '../inc/aircraftnames.php';
include '../inc/flightstatuses.php';
include '../inc/queryflightlist.php';

$flight_list_opts = flight_list_query_get_opts_from_query_parameters();
$flight_list_opts->fetch_pagination_data = true;
$flight_list = flight_list_query($flight_list_opts);

$__script = '_flights';
require '../inc/output.php';
