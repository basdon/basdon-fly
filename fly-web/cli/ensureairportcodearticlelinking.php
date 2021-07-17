<?php
// treat all notices/warnings/errors as errors
function exception_error_handler($severity, $message, $file, $line)
{
	throw new ErrorException($message, 0, $severity, $file, $line);
}
set_error_handler("exception_error_handler");

require '../inc/conf.php';
require '../inc/db.php';

// just error when a mapping is not found,
// because the article name can be whatever so it can't be automated

$status = 0;
foreach ($db->query('SELECT apt.c FROM apt LEFT OUTER JOIN artalt ON apt.c=artalt.alt WHERE artalt.alt IS NULL') as $a) {
	echo 'missing article alt mapping for airport ' . $a->c . "\n";
	$status = 1;
}
exit($status);
