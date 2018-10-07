<?php

require 'db.php';

if (!isset($_POST['n'], $_POST['j'])) {
	echo 'e' . 'missing_data';
	exit;
}

$id = create_guest($_POST['n']);

if ($id == -1) {
	echo 'e' . $lastdberr;
	exit;
}

$sessionid = create_game_session($id, $_POST['j']);

echo 's';
output_nonnull_5byte_num($id);
output_nonnull_5byte_num($sessionid);

