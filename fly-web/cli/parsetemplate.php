<?php
require('../inc/spate.php');
if (count($argv) != 2) {
	die('need file');
}
$parts = explode('/', $argv[1]);
$template_name = substr($parts[count($parts) - 1], 0, -4);
spate_default_generate($template_name);
