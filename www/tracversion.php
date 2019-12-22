<?php
include('../inc/bootstrap.php');
include('../inc/trac_constants.php');

function trac_releasetime($t)
{
	return date('YmdHis', $t);
}

$page = get_page();
$db_querycount += 2;
$releasecount = $db->query('SELECT COUNT(DISTINCT released) c FROM tract')->fetchAll()[0]->c; // NULL is not included in this count
$releases = $db->query('SELECT DISTINCT released FROM tract ORDER BY released DESC LIMIT 10 OFFSET '.(($page - 1) * 10))->fetchAll();

$releaseids = [];
$mapping = [];
foreach ($releases as $r) {
	if ($r->released != null) {
		$releaseids[] = $r->released;
		$mapping[$r->released] = [];
	}
}
// TODO: only use release from given rel GET param when present

if (count($releaseids)) {
	++$db_querycount;
	foreach ($db->query('SELECT id,released,severity,updated,summary FROM tract WHERE (visibility&'.$usergroups.' OR op='.$userid.') AND released IN ('.implode(',', $releaseids).') ORDER BY released DESC, updated DESC') as $r)
	{
		$mapping[$r->released][] = $r;
	}
}

$readytickets = $db->query('SELECT id,severity,updated,summary FROM tract WHERE status=3 AND (visibility&'.$usergroups.' OR op='.$userid.') ORDER BY severity DESC')->fetchAll();

$__script = '_tracversion';
include('../inc/output.php');
