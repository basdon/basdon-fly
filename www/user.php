<?php
include('../inc/bootstrap.php');

$id = -1;
$name = '';
$userqueryparam = '';
if (isset($_GET['id'])) {
	$id = (int) $_GET['id'];
	$id++;
	$id--;
	$r = $db->query('SELECT n FROM usr WHERE i=' . $id . ' LIMIT 1');
	if ($r !== false && ($r = $r->fetchAll()) !== false && count($r)) {
		$name = $r[0]->n;
	} else {
		$id = -1;
	}
	$userqueryparam = 'id=' . $id;
} elseif (isset($_GET['name'])) {
	$stmt = $db->prepare('SELECT i,g,n FROM usr WHERE n=? LIMIT 1');
	$stmt->bindValue(1, $_GET['name']);
	if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r) && $r[0]->g) {
		$id = $r[0]->i;
		$name = $r[0]->n;
		$userqueryparam = 'name=' . $name;
	}
}
++$db_querycount;

$__script = '_user';
include('../inc/output.php');
