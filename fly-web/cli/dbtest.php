<?php
$stdin = fopen('php://stdin', 'r');

try {
	$aaa = "hi\n";
} catch (Exception $e) {
}
echo $aaa;

require '../inc/conf.php';

echo "---------------------------------------\n";
echo "normal query:\n";
$db = new PDO("mysql:host={$db_host};dbname={$db_name}", $db_uname, $db_passw);
$cmdlogsize = $db->query('SELECT count(*) as c FROM cmdlog')->fetchAll()[0];
echo "result:\n";
var_dump($cmdlogsize);
echo "pdo errorcode {$db->errorCode()}\n"; // 00000

echo "---------------------------------------\n";
echo "fail prepare (exception):\n";
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
echo "ensure db is off, press enter"; fgets($stdin);
try {
	$stmt = $db->prepare('SELECT count(*) as c FROM cmdlog WHERE 1=?');
	echo "result:\n";
	var_dump($stmt); // PDOStatement instance
} catch (Exception $e) {
	echo "exception:\n";
	var_dump($e);
}
echo "pdo errorcode {$db->errorCode()}\n"; // 00000
echo "\n";

echo "---------------------------------------\n";
echo "fail prepare (silent):\n";
echo "ensure db is on, press enter"; fgets($stdin);
require '../inc/db.php';
$db = new PDO("mysql:host={$db_host};dbname={$db_name}", $db_uname, $db_passw);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
echo "ensure db is off, press enter"; fgets($stdin);
try {
	$stmt = $db->prepare('SELECT count(*) as c FROM cmdlog WHERE 1=?');
	echo "result:\n";
	var_dump($stmt); // PDOStatement instance
} catch (Exception $e) {
	echo "exception:\n";
	var_dump($e);
}
echo "pdo errorcode {$db->errorCode()}\n"; // 00000
echo "\n";

echo "---------------------------------------\n";
echo "fail bindValue (exception):\n";
echo "ensure db is on, press enter"; fgets($stdin);
require '../inc/db.php';
$db = new PDO("mysql:host={$db_host};dbname={$db_name}", $db_uname, $db_passw);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$stmt = $db->prepare('SELECT count(*) as c FROM cmdlog WHERE 1=?');
echo "pdo prepare errorcode {$db->errorCode()}\n"; // 00000
echo "ensure db is off, press enter"; fgets($stdin);
try {
	$res = $stmt->bindValue(1, 1);
	echo "result:\n";
	var_dump($res); // bool(true)
} catch (Exception $e) {
	echo "exception:\n";
	var_dump($e);
}
echo "pdo errorcode {$db->errorCode()}\n"; // 00000
echo "stmt errorcode {$stmt->errorCode()}\n"; // (empty)
echo "\n";

echo "---------------------------------------\n";
echo "fail bindValue (silent):\n";
echo "ensure db is on, press enter"; fgets($stdin);
require '../inc/db.php';
$db = new PDO("mysql:host={$db_host};dbname={$db_name}", $db_uname, $db_passw);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$stmt = $db->prepare('SELECT count(*) as c FROM cmdlog WHERE 1=?');
echo "pdo prepare errorcode {$db->errorCode()}\n"; // 00000
echo "ensure db is off, press enter"; fgets($stdin);
try {
	$res = $stmt->bindValue(1, 1);
	echo "result:\n";
	var_dump($res); // bool(true)
} catch (Exception $e) {
	echo "exception:\n";
	var_dump($e);
}
echo "pdo errorcode {$db->errorCode()}\n"; // 00000
echo "stmt errorcode {$stmt->errorCode()}\n"; // (empty)
echo "\n";

echo "---------------------------------------\n";
echo "fail execute (exception):\n";
echo "ensure db is on, press enter"; fgets($stdin);
require '../inc/db.php';
$db = new PDO("mysql:host={$db_host};dbname={$db_name}", $db_uname, $db_passw);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$stmt = $db->prepare('SELECT count(*) as c FROM cmdlog WHERE 1=?');
echo "pdo prepare errorcode {$db->errorCode()}\n"; // 00000
$stmt->bindValue(1, 1);
echo "stmt bindValue errorcode {$stmt->errorCode()}\n"; // (empty)
echo "ensure db is off, press enter"; fgets($stdin);
try {
	$res = $stmt->execute();
	echo "result:\n";
	var_dump($res);
} catch (Exception $e) {
	echo "exception:\n";
	var_dump($e); // exception
}
echo "pdo errorcode {$db->errorCode()}\n"; // 00000
echo "stmt errorcode {$stmt->errorCode()}\n"; // 70100

echo "---------------------------------------\n";
echo "fail execute (silent):\n";
echo "ensure db is on, press enter"; fgets($stdin);
require '../inc/db.php';
$db = new PDO("mysql:host={$db_host};dbname={$db_name}", $db_uname, $db_passw);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$stmt = $db->prepare('SELECT count(*) as c FROM cmdlog WHERE 1=?');
echo "pdo prepare errorcode {$db->errorCode()}\n"; // 00000
$stmt->bindValue(1, 1);
echo "stmt bindValue errorcode {$stmt->errorCode()}\n"; // (empty)
echo "ensure db is off, press enter"; fgets($stdin);
try {
	$res = $stmt->execute();
	echo "result:\n";
	var_dump($res); // bool(false)
} catch (Exception $e) {
	echo "exception:\n";
	var_dump($e);
}
echo "pdo errorcode {$db->errorCode()}\n"; // 00000
echo "stmt errorcode {$stmt->errorCode()}\n"; // 70100

echo "---------------------------------------\n";
echo "fail fetchAll (exception):\n";
echo "ensure db is on, press enter"; fgets($stdin);
require '../inc/db.php';
$db = new PDO("mysql:host={$db_host};dbname={$db_name}", $db_uname, $db_passw);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$stmt = $db->prepare('SELECT id FROM cmdlog WHERE 1=?');
echo "pdo prepare errorcode {$db->errorCode()}\n"; // 00000
$stmt->bindValue(1, 1);
echo "stmt bindValue errorcode {$stmt->errorCode()}\n"; // (empty)
$res = $stmt->execute();
echo "stmt execute errorcode {$stmt->errorCode()}\n"; // 00000
echo "result:\n";
var_dump($res);
echo "ensure db is off, press enter"; fgets($stdin);
try {
	$res = $stmt->fetchAll();
	echo "result length: " . count($res) . "\n"; // 9963
} catch (Exception $e) {
	echo "exception:\n";
	var_dump($e);
}
echo "pdo errorcode {$db->errorCode()}\n"; // 00000
echo "stmt errorcode {$stmt->errorCode()}\n"; // 00000

echo "---------------------------------------\n";
echo "fail fetchAll (silent):\n";
echo "ensure db is on, press enter"; fgets($stdin);
require '../inc/db.php';
$db = new PDO("mysql:host={$db_host};dbname={$db_name}", $db_uname, $db_passw);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$stmt = $db->prepare('SELECT id FROM cmdlog WHERE 1=?');
echo "pdo prepare errorcode {$db->errorCode()}\n"; // 00000
$stmt->bindValue(1, 1);
echo "stmt bindValue errorcode {$stmt->errorCode()}\n"; // (empty)
$res = $stmt->execute();
echo "stmt execute errorcode {$stmt->errorCode()}\n"; // 00000
echo "result:\n";
var_dump($res);
echo "ensure db is off, press enter"; fgets($stdin);
try {
	$res = $stmt->fetchAll();
	echo "result length: " . count($res) . "\n"; // 9963
} catch (Exception $e) {
	echo "exception:\n";
	var_dump($e);
}
echo "pdo errorcode {$db->errorCode()}\n"; // 00000
echo "stmt errorcode {$stmt->errorCode()}\n"; // 00000

echo "---------------------------------------\n";
echo "fail fetch (exception):\n";
echo "ensure db is on, press enter"; fgets($stdin);
require '../inc/db.php';
$db = new PDO("mysql:host={$db_host};dbname={$db_name}", $db_uname, $db_passw);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$stmt = $db->prepare('SELECT id FROM cmdlog WHERE 1=?');
echo "pdo prepare errorcode {$db->errorCode()}\n"; // 00000
$stmt->bindValue(1, 1);
echo "stmt bindValue errorcode {$stmt->errorCode()}\n"; // (empty)
$res = $stmt->execute();
echo "stmt execute errorcode {$stmt->errorCode()}\n"; // 00000
echo "result:\n";
var_dump($res);
echo "ensure db is off, press enter"; fgets($stdin);
$res = [];
try {
	while (($r = $stmt->fetch()) !== false) {
		$res[] = $r;
	}
} catch (Exception $e) {
	echo "exception:\n";
	var_dump($e);
}
echo "result length: " . count($res) . "\n"; // 9963
echo "pdo errorcode {$db->errorCode()}\n";
echo "stmt errorcode {$stmt->errorCode()}\n";

echo "---------------------------------------\n";
echo "fail fetch (silent):\n";
echo "ensure db is on, press enter"; fgets($stdin);
require '../inc/db.php';
$db = new PDO("mysql:host={$db_host};dbname={$db_name}", $db_uname, $db_passw);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$stmt = $db->prepare('SELECT id FROM cmdlog WHERE 1=?');
echo "pdo prepare errorcode {$db->errorCode()}\n"; // 00000
$stmt->bindValue(1, 1);
echo "stmt bindValue errorcode {$stmt->errorCode()}\n"; // (empty)
$res = $stmt->execute();
echo "stmt execute errorcode {$stmt->errorCode()}\n"; // 00000
echo "result:\n";
var_dump($res);
echo "ensure db is off, press enter"; fgets($stdin);
$res = [];
try {
	while (($r = $stmt->fetch()) !== false) {
		$res[] = $r;
	}
} catch (Exception $e) {
	echo "exception:\n";
	var_dump($e);
}
echo "result length: " . count($res) . "\n"; // 9963
echo "pdo errorcode {$db->errorCode()}\n"; // 00000
echo "stmt errorcode {$stmt->errorCode()}\n"; // 00000

echo "---------------------------------------\n";
echo "fetch all\n";
echo "ensure db is on, press enter"; fgets($stdin);
require '../inc/db.php';
$db = new PDO("mysql:host={$db_host};dbname={$db_name}", $db_uname, $db_passw);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$stmt = $db->prepare('SELECT id FROM cmdlog WHERE 1=?');
echo "pdo prepare errorcode {$db->errorCode()}\n"; // 00000
$stmt->bindValue(1, 1);
echo "stmt bindValue errorcode {$stmt->errorCode()}\n"; // (empty)
$res = $stmt->execute();
echo "stmt execute errorcode {$stmt->errorCode()}\n"; // 00000
echo "result:\n";
var_dump($res);
$res = [];
try {
	while (($r = $stmt->fetch()) !== false) {
		$res[] = $r;
	}
} catch (Exception $e) {
	echo "exception:\n";
	var_dump($e);
}
echo "result length: " . count($res) . "\n"; // 9963
echo "pdo errorcode {$db->errorCode()}\n"; // 00000
echo "stmt errorcode {$stmt->errorCode()}\n"; // 00000
