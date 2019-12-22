<?php
include('../inc/bootstrap.php');
include('../inc/form.php');
include('../inc/trac_constants.php');

function trac_releasetime($t)
{
	return date('YmdHis', $t);
}

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

		include('../inc/echo.php');
		$gamemsg = 'TRAC: '.$loggeduser->name.' commented on ticket #'.$id;
		$ircmsg = $gamemsg.' -> '.$BASEPATH.'/tracview.php?id='.$id;
		echo_send_message_irc_game(1, $ircmsg, $gamemsg);
	}
} else if (group_is_admin($usergroups) && isset($_POST['_form'], $_POST['summary'], $_POST['severity'], $_POST['visibility'], $_POST['HAARP']) && $HAARP == $_POST['HAARP']) {
	++$db_querycount;
	$old = $db->query('SELECT summary,severity,visibility,status FROM tract WHERE id='.$id);
	if ($old && ($old = $old->fetchAll()) && count($old)) {
		$old = $old[0];
		$fields = [];
		$values = [];
		$changes = [];

		if ($_POST['summary'] != $old->summary) {
			$fields[] = 'summary=?';
			$values[] = $_POST['summary'];
			if (empty($_POST['summary'])) {
				$__rawmsgs[] = 'Cannot set summary to empty';
				goto reject;
			}
			$changes[] = 'summary: "'.$old->summary.'" => "'.$_POST['summary'].'"';
		}

		// THESE MUST BE IN SYNC WITH tracversion.php WHEN MAKING A RELEASE!

		function check_transition($prop, $map)
		{
			global $changes, $fields, $values, $old;
			if ($_POST[$prop] != $old->$prop && array_key_exists($_POST[$prop], $map)) {
				$fields[] = $prop.'=?';
				$values[] = $_POST[$prop];
				$changes[] = $prop.': <span class="'.$prop.$old->$prop.'">'.$map[$old->$prop].'</span> => <span class="'.$prop.$_POST[$prop].'">'.$map[$_POST[$prop]].'</span>';
			}
		}

		check_transition('severity', $trac_severities);
		check_transition('visibility', $trac_visibilities);
		if (group_is_owner($usergroups)) {
			check_transition('status', $trac_statuses);
		}

		if (count($fields)) {
			++$db_querycount;
			$stmt = $db->prepare('UPDATE tract SET updated=UNIX_TIMESTAMP(),'.implode($fields, ',').' WHERE id='.$id);
			for ($i = 0; $i < count($values); $i++) {
				$stmt->bindValue($i + 1, $values[$i]);
			}
			$stmt->execute();
			++$db_querycount;
			$stmt = $db->prepare('INSERT INTO tracc(parent,usr,ip,stamp,type,comment) VALUES('.$id.','.$loggeduser->i.',?,UNIX_TIMESTAMP(),1,?)');
			$stmt->bindValue(1, $__clientip);
			$stmt->bindValue(2, implode($changes, '<br/>'));
			$stmt->execute();

			include('../inc/echo.php');
			$gamemsg = 'TRAC: '.$loggeduser->name.' updated ticket #'.$id;
			$ircmsg = $gamemsg.' -> '.$BASEPATH.'/tracview.php?id='.$id;
			echo_send_message_irc_game(1, $ircmsg, $gamemsg);
		}
	} else {
		$__rawmsgs[] = 'Unknown ticket!';
	}
} reject:

$trac_summary = '[unknown ticket]';
$trac_released = null;
++$db_querycount;
$trac = $db->query('SELECT _u.name,_u.i,stamp,updated,released,status,severity,visibility,summary,description FROM tract JOIN usr _u ON _u.i=tract.op WHERE visibility&'.$usergroups.' AND id='.$id);
if ($trac && ($trac = $trac->fetchAll()) && count($trac)) {
	$trac = $trac[0];

	$trac_summary = $trac->summary;
	$trac_released = $trac->released;
	$trac_released_display = trac_releasetime($trac_released);

	$trac_visibility = $trac->visibility;
	if (array_key_exists($trac->visibility, $trac_visibilities)) {
		$trac_visibility = $trac_visibilities[$trac->visibility];
	}

	$comments = $db->query('SELECT _u.name,_u.i,id,usr,ip,stamp,type,comment FROM tracc JOIN usr _u ON tracc.usr=_u.i WHERE parent='.$id);
} else {
	unset($trac);
}

$__script = '_tracview';
include('../inc/output.php');
