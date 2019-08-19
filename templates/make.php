<?php
require('../inc/spate.php');
if (count($argv) != 2) {
	die('need file');
}
$gen = str_replace('.tpl', '.php', $argv[1]);
file_put_contents("../gen/{$gen}", spate_generate('../templates/', $argv[1]));
