<?php

include('inc/bootstrap.php');

if ($__REPARSE__) {
	include('inc/spate.php');
	spate_default_generate('flight');
}

include('gen/flight.php');
