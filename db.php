<?php

require 'db.creds.php';

$db = null;
$db_querycount = 0;
$lastdberr = '000000';
try {
	$db = new PDO("mysql:host={$db_host};dbname={$db_name}", $db_uname, $db_passw);
	$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_OBJ);
	$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
	$db->exec('SET NAMES utf8');
	++$db_querycount;
} catch (PDOException $e) {
	// TODO: do something about it
}

// true if error occured
function db_err()
{
	global $lastdberr, $db;
	if ($lastdberr != '000000') {
		return true;
	}
	return ($lastdberr = $db->errorCode()) != '000000';
}

// -2 on failure, -1 if not exist, id on success
function find_user_id_by_name($name)
{
	global $db;
	if ($db == null) goto err;
	$stmt = $db->prepare('SELECT id FROM usr WHERE name=? LIMIT 1');
	if ($stmt === false) goto err;
	$stmt->bindValue(1, $name);
	if (!$stmt->execute()) goto err_stmt;
	$r = $stmt->fetchAll();
	if ($r === false) goto err_stmt;
	if (count($r) == 0) {
		return -1;
	}
	return $r[0]->id;
err:
	$lastdberr = $db->errorCode();
	return -2;
err_stmt:
	$lastdberr = $stmt->errorCode();
	return -2;
}

// -2 on failure, -1 if no match, score on success
function check_user_credentials($id, $pw)
{
	global $db;
	if ($db == null) goto err;
	$stmt = $db->query('SELECT pw,score FROM usr WHERE id='.intval($id, 10).' LIMIT 1');
	if ($stmt === false) goto err;
	if (!$stmt->execute()) goto err_stmt;
	$r = $stmt->fetchAll();
	if ($r === false) goto err_stmt;
	if (count($r) == 0) {
		return -1;
	}
	if (!password_verify($pw, $r[0]->pw)) {
		return -1;
	}
	return $r[0]->score;
err:
	$lastdberr = $db->errorCode();
	return -2;
err_stmt:
	$lastdberr = $stmt->errorCode();
	return -2;
}

// -1 on failure, id on success
function create_user($name, $pw, $group)
{
	global $lastdberr, $db;
	if ($db == null) goto err;
	$pw = password_hash($pw, PASSWORD_BCRYPT, array('cost' => 10));
	$stmt = $db->prepare('INSERT INTO usr(name,pw,regdate,lastseen,grp) VALUES(?,?,UNIX_TIMESTAMP(),UNIX_TIMESTAMP(),?)');
	if ($stmt === false) goto err;
	$stmt->bindValue(1, $name, PDO::PARAM_STR);
	$stmt->bindValue(2, $pw, PDO::PARAM_STR);
	$stmt->bindValue(3, $group, PDO::PARAM_INT);
	if (!$stmt->execute()) {
		$lastdberr = $stmt->errorCode();
		return -1;
	}
	return $db->lastInsertId();
err:
	$lastdberr = $db->errorCode();
	return -1;
}

