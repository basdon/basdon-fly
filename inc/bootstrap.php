<?php
$__timer = microtime(true);

$__msgs = [];

require('../inc/conf.php');
include('../inc/db.php');

function simple_pagination($urlWithParam, $currentPageFromOne, $totalPages)
{
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

if (isset($__script)) {
	if ($__REPARSE__) {
		include('../inc/spate.php');
		spate_default_generate($__script);
	}
	include('../gen/' . $__script . '.php');
}
