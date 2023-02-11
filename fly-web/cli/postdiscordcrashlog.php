<?php
require '../inc/conf.php';

if (!isset($DISCORD_CRASHLOG_WEBHOOK)) {
	exit(0);
}

$content = new stdClass();
$content->content = "Server crashed. ";
if ($argc > 1) {
	$content->content .= "Coredump available: <{$ABS_URL}/s/coredumps/{$argv[1]}> ";
}
$content->content .= "Server will restart.";

$resource = fopen($DISCORD_CRASHLOG_WEBHOOK, 'r', false, stream_context_create([
	'http' => [
		'method' => 'POST',
		'header' => 'Content-Type: application/json',
		'content' => json_encode($content),
	]
]));
if ($resource !== false) {
	fclose($resource);
}
// TODO ideally log somewhere, although an E_WARNING should be emitted, when 'fopen' fails
