<?php
if ($__REPARSE__) {
	require_once('../inc/spate.php');
	spate_default_generate($__script);
}
include('../gen/' . $__script . '.php');
// don't die, may be used in the missde of an article etc
