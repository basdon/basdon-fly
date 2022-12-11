<?php
require '../inc/bootstrap.php';
require '../inc/trac_constants.php';

if (!isset($loggeduser)) {
	$__script = '_unauthorized';
	$ret = 'tracnew.php';
} else {

	if (isset($_POST['summary'], $_POST['description'], $_POST['_form'], $_POST['visibility'], $_POST['HAARP']) && $HAARP == $_POST['HAARP']) {
		$summary = $_POST['summary'];
		if (empty($summary)) {
			$__rawmsgs[] = 'Cannot submit without a summary.';
			goto reject;
		}
		if (strlen($summary) > 80) {
			$__rawmsgs[] = 'Summary too long.';
			goto reject;
		}
		++$db_querycount;
		$stmt = $db->prepare('SELECT id FROM tract WHERE op=? AND summary=? LIMIT 1');
		$stmt->bindValue(1, $loggeduser->i);
		$stmt->bindValue(2, $summary);
		if ($stmt->execute() && ($stmt = $stmt->fetchAll()) && count($stmt)) {
			$__rawmsgs[] = 'Duplicate prevention: <a href="tracview.php?id='.$stmt[0]->id.'">you already made a ticket with this title</a>.';
			goto reject;
		}

		$t = time();

		$state = 0;
		$severity = 0;
		$visibility = max($GROUP_OWNER, intval($_POST['visibility'])); // never allow less than owner privs

		if (group_is_admin($usergroups) && isset($_POST['severity'])) {
			if (array_key_exists($_POST['severity'], $trac_severities)) {
				$severity = intval($_POST['severity']);
			}
			if (group_is_owner($usergroups) && isset($_POST['state']) && array_key_exists($_POST['state'], $trac_states)) {
				$state = intval($_POST['state']);
			}
		}

		++$db_querycount;
		$stmt = $db->prepare('INSERT INTO tract(op,stamp,updated,state,severity,visibility,summary,description) VALUES(?,?,?,?,?,?,?,?)');
		$stmt->bindValue(1, $loggeduser->i);
		$stmt->bindValue(2, $t);
		$stmt->bindValue(3, $t);
		$stmt->bindValue(4, $state);
		$stmt->bindValue(5, $severity);
		$stmt->bindValue(6, $visibility);
		$stmt->bindValue(7, $summary);
		$stmt->bindValue(8, $_POST['description']);
		if ($stmt->execute()) {
			$id = $db->lastInsertId();

			include '../inc/echo.php';
			$gamemsg = 'TRAC: '.$loggeduser->name.' created ticket #'.$id;
			if ($visibility == $GROUPS_ALL) {
				$gamemsg .= ': ' . mb_convert_encoding($summary, 'ASCII');
			}
			$ircmsg = $gamemsg.' -> '.$ABS_URL.'/tracview.php?id='.$id;
			echo_send_message_irc_game(1, $ircmsg, $gamemsg);

			header('Location: tracview.php?id=' . $id . '#u' .$t);
			die('redirecting');
		}
		$__rawmsgs[] = 'Failed to create a ticket. <a href="contact.php">Contact us.</a>';
	} reject:

	include '../inc/form.php';
	$__script = '_tracnew';
}

require '../inc/output.php';
