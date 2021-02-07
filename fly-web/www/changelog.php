<?php
include('../inc/bootstrap.php');

function fetch_changes()
{
	global $db;
	return $db->query('SELECT stamp,entry FROM chg ORDER BY id DESC');
}

$__script = '_changelog';
include('../inc/output.php');
