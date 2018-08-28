<?php

require 'db.creds.php';

$db = null;
$db_querycount = 0;
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
	global $db;
	return $db->errorCode() != '000000';
}

// -1 on failure, id on success
function find_user_id_by_name($name)
{
	global $db;
	if ($db == null) goto err;
	$stmt = $db->prepare('SELECT id FROM usr WHERE name=? LIMIT 1');
	if ($stmt === false) goto err;
	$stmt->bindValue(1, $name);
	$stmt->execute();
	$r = $stmt->fetch();
	if ($r === false || !count($r)) goto err;
	return $r[0]->id;
err:
	return -1;
}

function check_user_credentials($user, $pw)
{
}

function create_user($user, $pw)
{
}

