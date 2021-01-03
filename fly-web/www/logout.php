<?php
include('../inc/bootstrap.php');

if (isset($loggeduser)) {
	setcookie($COOKIENAME, '', 0, $COOKIEPATH, $COOKIEDOMAIN, $COOKIEHTTPS, true);
	++$db_querycount;
	$db->query('UPDATE webses SET lastupdate=UNIX_TIMESTAMP() WHERE id=\''.$__sesid.'\'');
	header('Location: ' . $BASEPATH . '/');
	die('redirecting');
}

$__script = '_logout';
include('../inc/output.php');
