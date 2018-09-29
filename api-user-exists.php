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
output_nonnull_5byte_num($id);

