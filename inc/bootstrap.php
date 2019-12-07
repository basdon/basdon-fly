<?php
$__timer = microtime(true);

$__msgs = [];
$__rawmsgs = [];

require('../inc/conf.php');
include('../inc/db.php');

$GROUP_BANNED = 1;
$GROUP_GUEST = 2;
$GROUP_MEMBER = 4;
$GROUP_ADMIN = 268435456;
$GROUP_OWNER = 1073741824;

$__clientip = $_SERVER['REMOTE_ADDR'];

if (isset($_COOKIE[$COOKIENAME]) && strlen($__sesid = $_COOKIE[$COOKIENAME]) == 32) {
	++$db_querycount;
	$s = $db->prepare('SELECT stay,u.i,u.name,u.groups,lastfal,falnw,falng FROM webses w JOIN usr u ON w.usr=u.i WHERE id=?');
	$s->bindValue(1, $__sesid);
	if ($s->execute() && ($r = $s->fetchAll()) && count($r)) {
		$loggeduser = $r[0];
		$loggeduser->logoutkey = md5($SECRET1 . $__sesid);
		$loggeduser->hasfailedlogins = $loggeduser->falnw < $loggeduser->lastfal;
		$expire = $loggeduser->stay ? (time() + 30 * 24 * 3600) : 0;
		setcookie($COOKIENAME, $__sesid, $expire, $COOKIEPATH, $COOKIEDOMAIN, $COOKIEHTTPS, true);
		++$db_querycount;
		$db->query('UPDATE webses SET lastupdate=UNIX_TIMESTAMP() WHERE id=\''.$__sesid.'\'');
	} else {
		setcookie($COOKIENAME, '', 0, $COOKIEPATH, $COOKIEDOMAIN, $COOKIEHTTPS, true);
	}
}

function generate_random_key()
{
	$key = '';
	for ($i = 0; $i < 32; $i++) {
		do {
			$n = rand(48, 122);
		} while((57 < $n && $n < 65) || (90 < $n && $n < 97));
		$key .= chr($n);
	}
	return $key;
}

function simple_pagination($urlWithParam, $currentPageFromOne, $totalItems, $itemsPerPage)
{
	// totalItems - 1 since if it's 50 and itemsPerPage would be 50, totalPages would be 2 (wrong)
	$totalPages = floor(max(0, $totalItems - 1) / $itemsPerPage) + 1;
	$str = '<p class="pagination">Page:';
	for ($i = 0; $i < $totalPages; ) {
		$i++;
		if ($i == $currentPageFromOne) {
			$str .= ' <b>' . $i . '</b>';
		} else {
			$str .= " <a href=\"{$urlWithParam}{$i}\">{$i}</a>";
		}
	}
	$str .= '</p>';
	return $str;
}

function format_duration_short($time)
{
	if ($time < 2) {
		return '1 second';
	}
	if ($time < 60) {
		return $time . ' seconds';
	}
	if ($time < 120) {
		return '1 minute';
	}
	if ($time < 3600) {
		return round($time / 60, 0) . ' minutes';
	}
	if ($time < 7200) {
		return '1 hour';
	}
	return round($time / 3600, 0) . ' hours';
}

function format_time_since($time)
{
	$tdiff = time() - $time;
	if ($tdiff < 86400 && date('j') == date('j', $time)) {
		return 'today';
	}
	if ($tdiff < 172800) {
		return 'yesterday';
	}
	if ($tdiff < 5184000) {
		return floor($tdiff / 86400) . ' days ago';
	}
	$tyear = (int) date('Y', $time);
	$nowyear = (int) date('Y');
	$tmonth = (int) date('n', $time);
	$nowmonth = (int) date('n');
	$dyear = $nowyear - $tyear;
	$dmonth = $nowmonth - $tmonth;
	if ($dmonth < 0) {
		$dmonth += 12;
		$dyear--;
	}
	$str = '';
	$glue = ', ';
	if ($dyear < 1) {
		$glue = '';
	} else if ($dyear < 2) {
		$str = '1 year';
	} else {
		$str = $dyear . ' years';
	}
	if ($dmonth < 1) {
	} else if ($dmonth < 2) {
		$str .= $glue . '1 month';
	} else {
		$str .= $glue . $dmonth . ' months';
	}
	return $str . ' ago';
}

function get_page()
{
	if (isset($_GET['page'])) {
		return max(intval($_GET['page']), 1);
	}
	return 1;
}

function linkuser($user)
{
	// link guests by their id, other users by their name
	if ($user->name[0] == '@') {
		return '<a href="user.php?id='.$user->i.'">'.$user->name.'</a>';
	} else {
		return '<a href="user.php?name='.urlencode($user->name).'">'.htmlspecialchars($user->name, ENT_QUOTES|ENT_HTML5|ENT_SUBSTITUTE).'</a>';
	}
}

if (isset($__script)) {
	include('../inc/output.php');
}
