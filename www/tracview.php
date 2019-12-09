<?php
include('../inc/bootstrap.php');
include('../inc/form.php');
include('../templates/trac_constants.php');

$id = intval($_GET['id']);

if (group_is_user_notbanned($usergroups) && isset($_POST['_form'], $_POST['comment'], $_POST['HAARP']) && $HAARP == $_POST['HAARP']) {
	++$db_querycount;
	$res = $db->query('UPDATE tract SET updated=UNIX_TIMESTAMP() WHERE id='.$id);
	if ($res !== false && $res->rowCount()) {
		$stmt = $db->prepare('INSERT INTO tracc(parent,usr,ip,stamp,type,comment) VALUES('.$id.','.$loggeduser->i.',?,UNIX_TIMESTAMP(),0,?)');
		$stmt->bindValue(1, $__clientip);
		$stmt->bindValue(2, $_POST['comment']);
		$stmt->execute();
		unset($_POST['_form']);
	}
} else if (group_is_admin($usergroups) && isset($_POST['_form'], $_POST['summary'], $_POST['severity'], $_POST['visibility'], $_POST['HAARP']) && $HAARP == $_POST['HAARP']) {
	++$db_querycount;
	$old = $db->query('SELECT summary,severity,visibility,status FROM tract WHERE id='.$id);
	if ($old && ($old = $old->fetchAll()) && count($old)) {
		$old = $old[0];
		$fields = [];
		$values = [];
		if ($_POST['summary'] != $old->summary) {
			$fields[] = 'summary=?';
			$values[] = $_POST['summary'];
			if (empty($_POST['summary'])) {
				$__rawmsgs[] = 'Cannot set summary to empty';
				goto reject;
			}
		}
		if ($_POST['severity'] != $old->severity) {
			$fields[] = 'severity=?';
			$values[] = $_POST['severity'];
		}
		if ($_POST['visibility'] != $old->visibility) {
			$fields[] = 'visibility=?';
			$values[] = $_POST['visibility'];
		}
		if (group_is_owner($usergroups)) {
			if ($_POST['status'] != $old->status) {
				$fields[] = 'status=?';
				$values[] = $_POST['status'];
			}
		}
		if (count($fields)) {
			++$db_querycount;
			$stmt = $db->prepare('UPDATE tract SET updated=UNIX_TIMESTAMP(),'.implode($fields, ',').' WHERE id='.$id);
			for ($i = 0; $i < count($values); $i++) {
				$stmt->bindValue($i + 1, $values[$i]);
			}
			$stmt->execute();
		}
	} else {
		$__rawmsgs[] = 'Unknown ticket!';
	}
} reject:

$trac_summary = '[unknown ticket]';
++$db_querycount;
$trac = $db->query('SELECT _u.name,_u.i,stamp,updated,status,severity,visibility,summary,description FROM tract JOIN usr _u ON _u.i=tract.op WHERE visibility&'.$usergroups.' AND id='.$id);
if ($trac && ($trac = $trac->fetchAll()) && count($trac)) {
	$trac = $trac[0];

	$trac_summary = $trac->summary;

	$trac_visibility = $trac->visibility;
	if (array_key_exists($trac->visibility, $trac_visibilities)) {
		$trac_visibility = $trac_visibilities[$trac->visibility];
	}

	$trac_status = $trac->status;
	$trac_severity = $trac->severity;

	$comments = $db->query('SELECT _u.name,_u.i,id,usr,ip,stamp,type,comment FROM tracc JOIN usr _u ON tracc.usr=_u.i WHERE parent='.$id);
} else {
	unset($trac);
}

$__script = '_tracview';
include('../inc/output.php');
