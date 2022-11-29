<?php
include('../inc/bootstrap.php');

if (isset($loggeduser, $_GET['k']) && $_GET['k'] === $loggeduser->logoutkey) {
	setcookie($COOKIENAME, '', 0, $COOKIEPATH, $COOKIEDOMAIN, $COOKIEHTTPS, true);
	++$db_querycount;
	$db->query('UPDATE webses SET lastupdate=UNIX_TIMESTAMP() WHERE id=\''.$__sesid.'\'');
	header('Location: ' . $ABS_URL . '/');
	die('redirecting');
}

$__script = '_logout';
include('../inc/output.php');
