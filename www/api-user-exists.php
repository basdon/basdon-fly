<?php

require 'db.php';

if (!isset($_POST['n']) && !isset($_POST['j'])) {
	echo 'e';
	exit;
}

if (should_ip_be_blocked_from_login_attempts($_POST['j'])) {
	echo 'l';
	exit;
}

$id = find_user_id_by_name($_POST['n']);

if ($id == -2) {
	echo 'e';
	exit;
}

if ($id == -1) {
	echo 'f';
	exit;
}

echo 't';
output_nonnull_5byte_num($id);

