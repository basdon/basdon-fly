<?php
if ($__REPARSE_TEMPLATES_EVERY_PAGE_LOAD__) {
	require_once('../inc/spate.php');
	spate_default_generate($__template);
}
include('../templates/gen/' . $__template . '.php');
// don't die, this may be included in the middle of an article etc
