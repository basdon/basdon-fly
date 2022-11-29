<?php
// treat all notices/warnings/errors as errors
function exception_error_handler($severity, $message, $file, $line)
{
	throw new ErrorException($message, 0, $severity, $file, $line);
}
set_error_handler("exception_error_handler");

$complete = 'var fm_islands=';
$complete .= file_get_contents('../gen/islandmapdata.txt');
$complete .= file_get_contents('../www/s/flightmap.js');
$complete = str_replace("\t", "", $complete);
file_put_contents('../www/s/gen/fm_complete.js', $complete);
