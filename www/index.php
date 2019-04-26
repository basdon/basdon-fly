<?php

include('../inc/bootstrap.php');

if ($__REPARSE__) {
	include('../inc/spate.php');
	spate_default_generate('index');
}

include('../gen/index.php');
