<?php
require '../inc/bootstrap.php';

if (isset($loggeduser, $_GET['k']) && $_GET['k'] === $loggeduser->logoutkey) {
	// ideally the 3rd parameter should be ['expires' => 0, 'samesite' => 'Strict'],
	// but this is done on the webserver level for now
	setcookie($COOKIENAME, '', 0, $COOKIEPATH, $COOKIEDOMAIN, $COOKIEHTTPS, true);
	++$db_querycount;
	$db->query('UPDATE webses SET lastupdate=UNIX_TIMESTAMP() WHERE id=\''.$__sesid.'\'');
	header('Location: ' . $ABS_URL . '/');
	die('redirecting');
}

$__template = '_logout';
require '../inc/output.php';
