<?php

require 'db.php';

if (!isset($_POST['i'], $_POST['p'], $_POST['j'])) {
	echo 'e' . 'missing_data';
	exit;
}

$id = intval($_POST['i'], 16);

$score = check_user_credentials($id, $_POST['p']);

if ($score == -2) {
	echo 'e' . $lastdberr;
	exit;
}

if ($score == -1) {
	echo 'f';
	exit;
}

$sessionid = create_game_session($id, $_POST['j']);

if ($sessionid == -1) {
	echo 'e' . $lastdberr;
	exit;
}

echo 's';
output_28bit_num($score);
output_28bit_num($sessionid);

