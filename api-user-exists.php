<?php

require 'db.php';

if (!isset($_POST['u'])) {
	echo 'e';
	exit;
}

$id = find_user_id_by_name($_POST['u']);

if (db_err()) {
	echo 'e';
	exit;
}

echo $id == -1 ? 'f' : 't';

