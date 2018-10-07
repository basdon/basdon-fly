<?php

require 'db.php';

if (!isset($_POST['n'], $_POST['p'], $_POST['j'])) {
	echo 'e' . 'missing_data';
	exit;
}

$id = create_user($_POST['n'], $_POST['p'], 4);

if ($id == -1) {
	echo 'e' . $lastdberr;
	exit;
}

$sessionid = create_game_session($id, $_POST['j']);

echo 's';
output_nonnull_5byte_num($id);
output_nonnull_5byte_num($sessionid);

