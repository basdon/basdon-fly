<?php
// treat all notices/warnings/errors as errors
function exception_error_handler($severity, $message, $file, $line)
{
	throw new ErrorException($message, 0, $severity, $file, $line);
}
set_error_handler("exception_error_handler");

require('../inc/spate.php');
if (count($argv) != 2) {
	die('need file');
}
$parts = explode('/', $argv[1]);
$template_name = substr($parts[count($parts) - 1], 0, -4);
spate_default_generate($template_name);
