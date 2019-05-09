<?php
include('../inc/bootstrap.php');

$id = -1;
if (isset($_GET['id'])) {
	$id = (int) $_GET['id'];
	$id++;
	$id--;
}
$returnpage = -1;
if (isset($_GET['rp'])) {
	$returnpage = (int) $_GET['rp'];
	$returnpage++;
	$returnpage--;
}

$__script = 'flight';
include('../inc/output.php');
