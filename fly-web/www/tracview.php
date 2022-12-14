<?php
if (!isset($_GET['id'])) {
	header('Location: /trac.php');
	die();
}

require '../inc/bootstrap.php';
require '../inc/form.php';
require '../inc/trac_constants.php';

function trac_releasetime($t)
{
	return date('YmdHis', $t);
}

$id = intval($_GET['id']);

if (group_is_user_notbanned($usergroups) && isset($_POST['_form'], $_POST['comment'], $_POST['HAARP']) && $HAARP == $_POST['HAARP']) {
	$t = time();
	++$db_querycount;
	$res = $db->query('UPDATE tract SET updated='.$t.' WHERE id='.$id);
	if ($res !== false && $res->rowCount()) {
		$stmt = $db->prepare('INSERT INTO tracc(parent,usr,ip,stamp,type,comment) VALUES('.$id.','.$loggeduser->i.',?,'.$t.',0,?)');
		$stmt->bindValue(1, $__clientip);
		$stmt->bindValue(2, $_POST['comment']);
		$stmt->execute();
		unset($_POST['_form']);

		// redirecting is kinda unnecessary, but marks the link as visited...
		// (it will also prevent resubmitting)
		header('Location: tracview.php?id='.$id.'#u'.$t);

		include '../inc/echo.php';
		$gamemsg = 'TRAC: '.$loggeduser->name.' commented on ticket #'.$id;
		$ircmsg = $gamemsg.' -> '.$ABS_URL.'/tracview.php?id='.$id;
		echo_send_message_irc_game(1, $ircmsg, $gamemsg);

		die('redirecting');
	}
} else if (group_is_admin($usergroups) && isset($_POST['_form'], $_POST['summary'], $_POST['severity'], $_POST['visibility'], $_POST['HAARP']) && $HAARP == $_POST['HAARP']) {
	++$db_querycount;
	$old = $db->query('SELECT summary,severity,visibility,state FROM tract WHERE id='.$id);
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
			$old_summary = htmlspecialchars($old->summary, ENT_QUOTES|ENT_HTML5|ENT_SUBSTITUTE);
			$new_summary = htmlspecialchars($_POST['summary'], ENT_QUOTES|ENT_HTML5|ENT_SUBSTITUTE);
			$changes[] = 'summary: "'.$old_summary.'" => "'.$new_summary.'"';
		}

		// THESE MUST BE IN SYNC WITH tracversion.php WHEN MAKING A RELEASE!

		function check_transition($name, $prop, $map, &$outvar)
		{
			global $changes, $fields, $values, $old;
			if ($_POST[$prop] != $old->$prop && array_key_exists($_POST[$prop], $map)) {
				$fields[] = $prop.'=?';
				$values[] = $_POST[$prop];
				$changes[] = $name.': <span class="'.$prop.$old->$prop.'">'.$map[$old->$prop].'</span> => <span class="'.$prop.$_POST[$prop].'">'.$map[$_POST[$prop]].'</span>';
				$outvar = $_POST[$prop];
			}
		}

		$visibility = $old->visibility;
		$state = $old->state;
		$impact = $old->severity;

		check_transition('impact', 'severity', $trac_severities, $impact);
		check_transition('visibility', 'visibility', $trac_visibilities, $visibility);
		if (group_is_owner($usergroups)) {
			check_transition('status', 'state', $trac_states, $state);
		}

		if (count($fields)) {
			$t = time();
			++$db_querycount;
			$stmt = $db->prepare('UPDATE tract SET updated='.$t.','.implode($fields, ',').' WHERE id='.$id);
			for ($i = 0; $i < count($values); $i++) {
				$stmt->bindValue($i + 1, $values[$i]);
			}
			$stmt->execute();
			++$db_querycount;
			$stmt = $db->prepare('INSERT INTO tracc(parent,usr,ip,stamp,type,comment) VALUES('.$id.','.$loggeduser->i.',?,'.$t.',1,?)');
			$stmt->bindValue(1, $__clientip);
			$stmt->bindValue(2, implode($changes, '<br/>'));
			$stmt->execute();

			// redirecting is kinda unnecessary, but marks the link as visited...
			header('Location: tracview.php?id='.$id.'#u'.$t);

			include '../inc/echo.php';
			$gamemsg = 'TRAC: '.$loggeduser->name.' updated ticket #'.$id;
			if ($visibility == $GROUPS_ALL) {
				$gamemsg .= ': ' . mb_convert_encoding($_POST['summary'], 'ASCII');
			}
			$ircmsg = $gamemsg . ' -> '.$ABS_URL.'/tracview.php?id='.$id;
			echo_send_message_irc_game(1, $ircmsg, $gamemsg);

			die('redirected');
		}
	} else {
		$__rawmsgs[] = 'Unknown ticket!';
	}
} reject:

$trac_summary = '[unknown ticket]';
$trac_released = null;
++$db_querycount;
$trac = $db->query('SELECT _u.name,_u.i,stamp,updated,released,state,severity,visibility,summary,description FROM tract JOIN usr _u ON _u.i=tract.op WHERE (visibility&'.$usergroups.' OR op='.$userid.') AND id='.$id);
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
require '../inc/output.php';
