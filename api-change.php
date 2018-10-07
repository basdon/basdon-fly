<?php

require 'db.php';

if (!isset($_POST['i'], $_POST['p'])) {
	echo 'e' . 'missing_data';
	exit;
}

$id = intval($_POST['i'], 10);

if ($db == null) {
	echo 'enodb';
	exit;
}

$isguestregister = isset($_POST['g'], $_POST['n']);

$stmt = false;
if ($isguestregister) {
	$stmt = $db->prepare('UPDATE usr SET p=?,n=?,g=? WHERE i=?');
} else {
	$stmt = $db->prepare('UPDATE usr SET p=? WHERE i=?');
}
if ($stmt === false) goto err;
$stmt->bindValue(1, hash_pw($_POST['p']), PDO::PARAM_STR);
if ($isguestregister) {
	$stmt->bindValue(2, $_POST['n'], PDO::PARAM_STR);
	$stmt->bindValue(3, intval($_POST['g'], 10), PDO::PARAM_INT);
	$stmt->bindValue(4, $id, PDO::PARAM_INT);
} else {
	$stmt->bindValue(2, $id, PDO::PARAM_INT);
}
if (!$stmt->execute()) {
	$lastdberr = $stmt->errorCode();
	goto err;
}
echo 's';
exit;

err:
$lastdberr = $db->errorCode();
echo 'e' . $lastdberr;

