<?php
require('../inc/spate.php');
if (count($argv) != 2) {
	die('need file');
}
$gen = str_replace('.tpl', '.php', $argv[1]);
spate_default_generate(substr($argv[1], 0, -4));
