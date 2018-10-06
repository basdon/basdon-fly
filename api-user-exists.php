<?php

require 'db.php';

if (!isset($_POST['u'])) {
	echo 'e';
	exit;
}

if (should_ip_be_blocked_from_login_attempts($_POST['j'])) {
	echo 'l';
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
output_nonnull_5byte_num($id);

