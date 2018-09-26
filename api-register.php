<?php

require 'db.php';

if (!isset($_POST['u'], $_POST['p'])) {
	echo 'e' . 'missing_data';
	exit;
}

$id = create_user($_POST['u'], $_POST['p'], 4);

if ($id == -1) {
	echo 'e' . $lastdberr;
	exit;
}

echo 's';
echo chr(0x80 | ($id & 0xFF));
echo chr(0x80 | (($id >> 7) & 0xFF));
echo chr(0x80 | (($id >> 14) & 0xFF));
echo chr(0x80 | (($id >> 21) & 0xFF));

