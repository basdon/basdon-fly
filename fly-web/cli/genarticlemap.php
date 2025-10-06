<?php
// treat all notices/warnings/errors as errors
function exception_error_handler($severity, $message, $file, $line)
{
	throw new ErrorException($message, 0, $severity, $file, $line);
}
set_error_handler("exception_error_handler");

require '../inc/conf.php';
require '../inc/db.php';

$articlemap = [];
foreach ($db->query('SELECT name,title FROM art') as $a) {
	$articlemap[$a->name] = $a->title;
}
$articlecatmap = [];
foreach ($db->query('SELECT name FROM artcat') as $a) {
	$articlecatmap[$a->name] = $a->name;
}

$output = '<?php $articlemap=' . var_export($articlemap, true) . '; $articlecatmap=' . var_export($articlecatmap, true) . ';';

file_put_contents('../gen/articlemap.php', $output);
