<?php
if ($__REPARSE__) {
	include('../inc/spate.php');
	spate_default_generate($__script);
}
include('../gen/' . $__script . '.php');
