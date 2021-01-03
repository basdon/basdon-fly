<?php
include('../inc/bootstrap.php');

$id = -1;
$name = '';
if (isset($_GET['id'])) {
	$id = intval($_GET['id']);
	$r = $db->query('SELECT name FROM usr WHERE i=' . $id . ' LIMIT 1');
	if ($r !== false && ($r = $r->fetchAll()) !== false && count($r)) {
		$name = $r[0]->name;
	} else {
		$id = -1;
	}
	$paginationbaseurl = 'user.php?id='.$id;
} elseif (isset($_GET['name'])) {
	$stmt = $db->prepare('SELECT i,name FROM usr WHERE name=? LIMIT 1');
	$stmt->bindValue(1, $_GET['name']);
	if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r)) {
		$name = $r[0]->name;
		// don't show guest accounts when searching by name
		if ($name[0] == '@') {
			$name = '';
			goto skip;
		}
		$id = $r[0]->i;
		$paginationbaseurl = 'user.php?name='.$name;
	}
} skip:
++$db_querycount;

$__script = '_user';
include('../inc/output.php');
