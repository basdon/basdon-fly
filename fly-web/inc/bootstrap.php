<?php
$__timer = microtime(true);

$__msgs = [];
$__rawmsgs = [];

if (!file_exists('../inc/conf.php')) {
	die('missing "inc/conf.php" file, read "inc/conf.sample.php" instructions');
}
require('../inc/conf.php');
include('../inc/db.php');
include('../inc/funcs.php');
if ($__REPARSE__) {
	include('../cli/gensettings.php');
} else if (!file_exists('../inc/__settings.php')) {
	die('missing "inc/__settings.php" file, run "make web" first');
}
include('../inc/__settings.php');

$META_TAGS = [
	'og:image' => '/s/favicon-96x96.png',
	'og:site_name' => 'basdon.net aviation server',
];

$GROUP_BANNED = 1;
$GROUP_GUEST = 2;
$GROUP_MEMBER = 4;
$GROUP_ADMIN = 268435456;
$GROUP_OWNER = 1073741824;

$GROUPS_ALL = 2147483647;
$GROUPS_ADMIN = $GROUPS_ALL & ~($GROUP_ADMIN - 1);

// TODO: move this to conf.php?
$datetimeformat = 'j M Y H:i';

function group_is_user_notbanned($group)
{
	global $GROUP_MEMBER, $GROUP_BANNED;
	return $group >= $GROUP_MEMBER && !($group & $GROUP_BANNED);
}

function group_is_banned($group)
{
	global $GROUP_BANNED;
	return $group & $GROUP_BANNED;
}

function group_is_admin($group)
{
	global $GROUP_ADMIN;
	return $group >= $GROUP_ADMIN;
}

function group_is_owner($group)
{
	global $GROUP_OWNER;
	return $group >= $GROUP_OWNER;
}

$__clientip = $_SERVER['REMOTE_ADDR'];

$usergroups = $GROUP_GUEST;
$userid = -1;
if (isset($_COOKIE[$COOKIENAME]) && strlen($_COOKIE[$COOKIENAME]) == 32) {
	++$db_querycount;
	$s = $db->prepare('SELECT stay,u.i,u.name,u.groups,lastfal,falnw,falng FROM webses w JOIN usr u ON w.usr=u.i WHERE id=?');
	$s->bindValue(1, $_COOKIE[$COOKIENAME]);
	if ($s->execute() && ($r = $s->fetchAll()) && count($r)) {
		$__sesid = $_COOKIE[$COOKIENAME];
		$loggeduser = $r[0];
		$loggeduser->logoutkey = md5($SECRET1 . $__sesid);
		$loggeduser->hasfailedlogins = $loggeduser->falnw < $loggeduser->lastfal;
		$expire = $loggeduser->stay ? (time() + 30 * 24 * 3600) : 0;
		setcookie($COOKIENAME, $__sesid, $expire, $COOKIEPATH, $COOKIEDOMAIN, $COOKIEHTTPS, true);
		$db_querycount += 2;
		$db->query('UPDATE webses SET lastupdate=UNIX_TIMESTAMP() WHERE id=\''.$__sesid.'\'');
		$db->query('UPDATE usr SET lastseenweb=UNIX_TIMESTAMP() WHERE i='.$loggeduser->i);
		$userid = $loggeduser->i;
		$usergroups = $loggeduser->groups;
		$HAARP = $loggeduser->logoutkey; // maybe this should he something different?
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

function format_datetime($time)
{
	global $datetimeformat;
	return date($datetimeformat, $time) . ' (GMT' . date('O') . ')';
}

function format_duration_short($time)
{
	if ($time == 1) {
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

function format_duration_short_days($time)
{
	if ($time < 86400) {
		return format_duration_short($time);
	}
	$d = floor($time / 86400);
	$s = "$d day";
	if ($d != 1) {
		$s .= 's';
	}
	return $s . ' ' . format_duration_short($time - $d * 86400);
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
	if ($user == null) {
		return 'None';
	} else if ($user->name[0] == '@') {
		return '<a href="user.php?id='.$user->i.'">'.$user->name.'</a>';
	} else {
		return '<a href="user.php?name='.urlencode($user->name).'">'.htmlspecialchars($user->name, ENT_QUOTES|ENT_HTML5|ENT_SUBSTITUTE).'</a>';
	}
}

if (isset($__script)) {
	include('../inc/output.php');
}
