<?php
include('../inc/bootstrap.php');

$action = 'publicprofile';
$actions = ['publicprofile', 'fal'];
if (isset($_GET['action']) && array_key_exists($_GET['action'], $actions)) {
	$action = $actions;
}

if ($action == 'publicprofile') {
	$id = $loggeduser->i;
	$name = $loggeduser->name;
	$paginationbaseurl = 'account.php?action=publicprofile';
} else if ($action == 'fal') {
}

$__script = '_account';
include('../inc/output.php');
