<?php

require 'db.php';

if (!isset($_POST['i'], $_POST['p'])) {
	echo 'e' . 'missing_data';
	exit;
}

$id = intval($_POST['i'], 10);

$score = check_user_credentials($id, $_POST['p'], null);

if ($score == -2) {
	echo 'e' . $lastdberr;
	exit;
}

if ($score == -1) {
	echo 'f';
	exit;
}

echo 't';

