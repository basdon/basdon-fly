<?php
include('../inc/bootstrap.php');
include('../inc/trac_constants.php');

function trac_version($t)
{
	return date('YmdHis', $t);
}

$page = get_page();
$db_querycount += 2;
$tracthreads = $db->query('SELECT id,updated,released,status,severity,summary FROM tract WHERE visibility&'.$usergroups.' OR op='.$userid.' ORDER BY status<4 DESC, id DESC LIMIT 100 OFFSET '.(($page - 1) * 100));
$totalrows = $db->query('SELECT (SELECT COUNT(id) FROM tract) c, (SELECT COUNT(id) FROM tract WHERE status<4) a');
if ($totalrows && ($totalrows = $totalrows->fetchAll())) {
	$unresolvedcount = $totalrows[0]->a;
	$totalrows = $totalrows[0]->c;
} else {
	$unresolvedcount = 0;
	$totalrows = 0;
}

$__script = '_trac';
include('../inc/output.php');
