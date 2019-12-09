<?php
include('../inc/bootstrap.php');
include('../templates/trac_constants.php');

if (!isset($loggeduser)) {
	$__script = '_unauthorized';
	$ret = 'tracnew.php';
} else {

	if (isset($_POST['summary'], $_POST['description'], $_POST['_form'], $_POST['HAARP']) && $HAARP == $_POST['HAARP']) {
		$summary = $_POST['summary'];
		if (empty($summary)) {
			$__rawmsgs[] = 'Cannot submit without a summary.';
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

		$status = 0;
		$severity = 0;
		$visibility = $GROUPS_ALL;
		if (group_is_admin($usergroups) && isset($_POST['severity'], $_POST['visibility'])) {
			$visibility = max($GROUP_OWNER, intval($_POST['visibility'])); // never allow less than owner privs
			if (array_key_exists(intval($_POST['severity']), $trac_severities)) {
				$severity = intval($_POST['severity']);
			}
			if (group_is_owner($usergroups)) {
				$status = 2; // if owner creates, it's instantly acknowledged
			}
		}

		++$db_querycount;
		$stmt = $db->prepare('INSERT INTO tract(op,stamp,updated,status,severity,visibility,summary,description) VALUES(?,?,?,?,?,?,?,?)');
		$stmt->bindValue(1, $loggeduser->i);
		$stmt->bindValue(2, $t);
		$stmt->bindValue(3, $t);
		$stmt->bindValue(4, $status);
		$stmt->bindValue(5, $severity);
		$stmt->bindValue(6, $visibility);
		$stmt->bindValue(7, $summary);
		$stmt->bindValue(8, $_POST['description']);
		if ($stmt->execute()) {
			header('Location: tracview.php?id=' . $db->lastInsertId());
			die('redirecting');
		}
		$__rawmsgs[] = 'Failed to create a ticket. <a href="contact.php">Contact us.</a>';
	} reject:

	include('../inc/form.php');
	$__script = '_tracnew';
}

include('../inc/output.php');
