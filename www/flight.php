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

function aircraftowner($ownername)
{
	if ($ownername == null) {
		return 'public';
	} else {
		$usr = new stdClass();
		$usr->name = $ownername;
		$usr->i = -1; // guest shouldn't own vehicles, so this _should_ be fine
		return linkuser($usr);
	}
}

$__script = '_flight';
include('../inc/output.php');
