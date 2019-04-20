<?php

include('inc/bootstrap.php');

if ($__REPARSE__) {
	include('inc/spate.php');
	file_put_contents('gen/index.php', spate_generate('./templates/', 'index.html'));
}

include('gen/index.php');
