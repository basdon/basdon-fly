<?php
include('../inc/bootstrap.php');

$id = intval($_GET['id']);
$trac_summary = '[unknown ticket]';
++$db_querycount;
$trac = $db->query('SELECT _u.name,_u.i,stamp,updated,status,severity,visibility,summary,description FROM tract JOIN usr _u ON _u.i=tract.op WHERE visibility&'.$usergroups.' AND id='.$id);
if ($trac && ($trac = $trac->fetchAll()) && count($trac)) {
	$trac = $trac[0];

	$trac_summary = $trac->summary;

	$trac_visibility = $trac->visibility;
	if ($trac_visibility == $GROUPS_ALL) {
		$trac_visibility = 'public';
	}

	$trac_status = $trac->status;
	$trac_impact = $trac->severity;
} else {
	unset($trac);
}

$__script = '_tracview';
include('../inc/output.php');
