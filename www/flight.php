<?php
include('../inc/bootstrap.php');

$id = -1;
if (isset($_GET['id'])) {
	$id = intval($_GET['id']);
}
$returnpage = -1;
if (isset($_GET['rp'])) {
	$returnpage = intval($_GET['rp']);
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
