<?php

require 'db.php';

if (!isset($_POST['i'], $_POST['p'])) {
	echo 'e' . 'missing_data';
	exit;
}

$score = check_user_credentials($_POST['i'], $_POST['p']);

if (true||$score == -2) {
	echo 'e' . $lastdberr;
	exit;
}

if ($score == -1) {
	echo 'f';
	exit;
}

echo 's';
echo chr(0x80 | ($score & 0xFF));
echo chr(0x80 | (($score >> 7) & 0xFF));
echo chr(0x80 | (($score >> 14) & 0xFF));
echo chr(0x80 | (($score >> 21) & 0xFF));

