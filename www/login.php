<?php
include('../inc/bootstrap.php');
include('../inc/form.php');

if (!isset($loggeduser) && isset($_POST['_form'], $_POST['usr'], $_POST['pwd'])) {
	if (!isset($_POST['yummie'])) {
		$loginerr = 'You can\'t login without accepting cookies.';
		goto skiplogin;
	}

	// check first if clientip has too many recent failed logins (max 12 in 30m)
	$stmt = $db->prepare('SELECT COUNT(u) AS c FROM fal WHERE stamp>UNIX_TIMESTAMP()-1800 AND ip=?');
	$stmt->bindValue(1, $__clientip);
	if ($stmt->execute() && ($r = $stmt->fetchAll()) && $r[0]->c > 12) {
		$loginerr = 'Too many recent failed login attempts, please try again later.';
		goto skiplogin;
	}

	++$db_querycount;
	$stmt = $db->prepare('SELECT i,pw FROM usr WHERE name=? LIMIT 1');
	$stmt->bindValue(1, $_POST['usr']);
	if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r)) {
		$r = $r[0];
		$id = $r->i;

		// another block check, but on per-account base (max 6 in 30m)
		$stmt = $db->query('SELECT COUNT(u) AS c FROM fal WHERE stamp>UNIX_TIMESTAMP()-1800 AND u='.$id);
		if ($stmt->execute() && ($_r = $stmt->fetchAll()) && $_r[0]->c > 6) {
			$loginerr = 'Too many recent failed login attempts, please try again later.';
			goto skiplogin;
		}

		if (password_verify($_POST['pwd'], $r->pw)) {
			$stay = isset($_POST['stay']);
			$__sesid = generate_random_key();
			$stmt = $db->prepare('INSERT INTO webses(id,usr,stamp,lastupdate,stay,ip) VALUES(?,?,UNIX_TIMESTAMP(),UNIX_TIMESTAMP(),?,?)');
			$stmt->bindValue(1, $__sesid);
			$stmt->bindValue(2, $id);
			$stmt->bindValue(3, $stay ? 1 : 0);
			$stmt->bindValue(4, $__clientip);
			$stmt->execute();
			$expire = $stay ? (time() + 30 * 24 * 3600) : 0;
			setcookie($COOKIENAME, $__sesid, $expire, $COOKIEPATH, $COOKIEDOMAIN, $COOKIEHTTPS, true);
			header('Location: ' . $BASEPATH . '/');
			die('redirecting');
		}

		++$db_querycount;
		$stmt = $db->prepare('INSERT INTO fal(u,stamp,ip,isweb) VALUES(?,UNIX_TIMESTAMP(),?,1)');
		$stmt->bindValue(1, $id);
		$stmt->bindValue(2, $__clientip);
		$stmt->execute();
		$loginerr = 'Password incorrect';
		unset($_POST['pwd']);
	} else {
		$loginerr = 'User not found';
	}
} skiplogin:

$__script = '_login';
include('../inc/output.php');
