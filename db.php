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

function output_nonnull_5byte_num($num)
{
	echo chr(0x80 | ($num & 0xFF));
	echo chr(0x80 | (($num >> 7) & 0xFF));
	echo chr(0x80 | (($num >> 14) & 0xFF));
	echo chr(0x80 | (($num >> 21) & 0xFF));
	echo chr(0xF0 | (($num >> 28) & 0x0F));
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
	$stmt = $db->prepare('SELECT i FROM usr WHERE n=? LIMIT 1');
	if ($stmt === false) goto err;
	$stmt->bindValue(1, $name);
	if (!$stmt->execute()) goto err_stmt;
	$r = $stmt->fetchAll();
	if ($r === false) goto err_stmt;
	if (count($r) == 0) {
		return -1;
	}
	return $r[0]->i;
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
	$stmt = $db->query('SELECT p,s FROM usr WHERE i='.intval($id, 10).' LIMIT 1');
	if ($stmt === false) goto err;
	if (!$stmt->execute()) goto err_stmt;
	$r = $stmt->fetchAll();
	if ($r === false) goto err_stmt;
	if (count($r) == 0) {
		return -1;
	}
	if (!password_verify($pw, $r[0]->p)) {
		return -1;
	}
	return $r[0]->s;
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
	$pw = password_hash($pw, PASSWORD_BCRYPT, array('cost' => 10));
	return create_user_raw($name, $pw, $group);
}

// -1 on failure, id on success
function create_guest($name)
{
	return create_user_raw($name, '', 0);
}

// -1 on failure, id on success
function create_user_raw($name, $pw, $group)
{
	global $lastdberr, $db;
	if ($db == null) goto err;
	$stmt = $db->prepare('INSERT INTO usr(n,p,r,l,g) VALUES(?,?,UNIX_TIMESTAMP(),UNIX_TIMESTAMP(),?)');
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

// -1 on failure, sessionid on success
function create_game_session($uid, $ip)
{
	global $lastdberr, $db;
	if ($db == null) goto err;
	$db->exec('UPDATE usr SET l=UNIX_TIMESTAMP() WHERE i='.intval($uid, 10).' LIMIT 1');
	$stmt = $db->prepare('INSERT INTO ses(u,s,e,j) VALUES(?,UNIX_TIMESTAMP(),UNIX_TIMESTAMP(),?)');
	if ($stmt === false) goto err;
	$stmt->bindValue(1, $uid, PDO::PARAM_INT);
	$stmt->bindValue(2, $ip, PDO::PARAM_STR);
	if (!$stmt->execute()) {
		$lastdberr = $stmt->errorCode();
		return -1;
	}
	return $db->lastInsertId();
err:
	$lastdberr = $db->errorCode();
	return -1;
}

