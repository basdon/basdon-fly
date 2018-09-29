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

$sessionid = create_game_session($id, $_POST['j']);

echo 's';
ouput_28bit_num($id);
ouput_28bit_num($sessionid);

