<?php
include('../inc/bootstrap.php');

$page = get_page();
$db_querycount += 2;
$tracthreads = $db->query('SELECT id,updated,status,severity,summary FROM tract WHERE visibility&'.$usergroups.' OR op='.$userid.' LIMIT 50 OFFSET '.(($page - 1) * 50));
$totalrows = $db->query('SELECT COUNT(id) c FROM tract');
if ($totalrows && ($totalrows = $totalrows->fetchAll())) {
	$totalrows = $totalrows[0]->c;
} else {
	$totalrows = 0;
}

$__script = '_trac';
include('../inc/output.php');
