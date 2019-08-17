<?php

$db = null;
$db_querycount = 0;
try {
	$db = new PDO("mysql:host={$db_host};dbname={$db_name}", $db_uname, $db_passw);
	$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_OBJ);
	$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
	$db->exec('SET NAMES utf8mb4');
	++$db_querycount;
} catch (PDOException $e) {
	$__msgs[] = "Cannot connect to db.";
}
