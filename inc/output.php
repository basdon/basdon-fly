<?php
if ($__REPARSE__) {
	include('../inc/spate.php');
	file_put_contents("../gen/{$__script}.php", spate_generate('../templates/', "{$__script}.tpl"));
}
include('../gen/' . $__script . '.php');
