<?php
// treat all notices/warnings/errors as errors
function exception_error_handler($severity, $message, $file, $line)
{
	throw new ErrorException($message, 0, $severity, $file, $line);
}
set_error_handler("exception_error_handler");

require '../inc/conf.php';
require '../inc/db.php';

$apts = $db->query('SELECT c FROM apt ORDER BY c ASC')->fetchAll();

ob_start();
echo '<?php $all_airport_codes=[';
$comma = false;
foreach ($apts as $apt) {
	if ($comma) {
		echo ',';
	} else {
		$comma = true;
	}
	echo "'{$apt->c}'";
}
echo '];';
file_put_contents('../inc/gen/all_airport_codes.php', ob_get_contents());
ob_end_clean();
