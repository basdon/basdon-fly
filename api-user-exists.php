<?php

require 'db.php';

if (!isset($_POST['u'])) {
	echo 'e';
	exit;
}

$id = find_user_id_by_name($_POST['u']);

if ($id == -2) {
	echo 'e';
	exit;
}

if ($id == -1) {
	echo 'f';
	exit;
}

echo 't';
echo chr(0x80 | ($id & 0xFF));
echo chr(0x80 | (($id >> 7) & 0xFF));
echo chr(0x80 | (($id >> 14) & 0xFF));
echo chr(0x80 | (($id >> 21) & 0xFF));

