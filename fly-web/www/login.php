<?php
require '../inc/bootstrap.php';
require '../inc/form.php';

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
	$stmt = $db->prepare('SELECT i,pw,falng,falnw,name,groups FROM usr WHERE name=? LIMIT 1');
	$stmt->bindValue(1, $_POST['usr']);
	if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r)) {
		$r = $r[0];

		// another block check, but on per-account base (max 4 in 30m)
		++$db_querycount;
		$stmt = $db->query('SELECT COUNT(u) AS c FROM fal WHERE stamp>UNIX_TIMESTAMP()-1800 AND u='.$r->i);
		if ($stmt->execute() && ($_r = $stmt->fetchAll()) && $_r[0]->c > 4) {
			$loginerr = 'Too many recent failed login attempts, please try again later.';
			goto skiplogin;
		}

		if (password_verify($_POST['pwd'], $r->pw)) {
			$stay = isset($_POST['stay']);
			$__sesid = generate_random_key();
			++$db_querycount;
			$stmt = $db->prepare('INSERT INTO webses(id,usr,stamp,lastupdate,stay,ip) VALUES(?,?,UNIX_TIMESTAMP(),UNIX_TIMESTAMP(),?,?)');
			$stmt->bindValue(1, $__sesid);
			$stmt->bindValue(2, $r->i);
			$stmt->bindValue(3, $stay ? 1 : 0);
			$stmt->bindValue(4, $__clientip);
			$stmt->execute();
			$expire = $stay ? (time() + 30 * 24 * 3600) : 0;
			setcookie($COOKIENAME, $__sesid, $expire, $COOKIEPATH, $COOKIEDOMAIN, $COOKIEHTTPS, true);

			if (isset($_GET['ret'])) {
				header('Location: '.$ABS_URL.'/'.header_value($_GET['ret']));
			} else {
				header('Location: '.$ABS_URL.'/');
			}
			die('redirecting');
		}

		$time = time();
		$db_querycount += 2;
		$db->query('UPDATE usr SET lastfal='.$time.' WHERE i='.$r->i);
		$stmt = $db->prepare('INSERT INTO fal(u,stamp,ip,isweb) VALUES(?,'.$time.',?,1)');
		$stmt->bindValue(1, $r->i);
		$stmt->bindValue(2, $__clientip);
		$stmt->execute();
		unset($r);
		unset($_POST['pwd']);
	}
	// No, I'm not going to counter timing attacks. The member list is public anyways.
	$attemptedlogin = 1;
} skiplogin:

$__template = '_login';
require '../inc/output.php';
