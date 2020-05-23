<?php
include('../inc/conf.php');

http_response_code(301);
if (isset($_GET['code']) && !is_array($_GET['code'])) {
	header('Location: ' . $BASEPATH . '/locations.php?code=' . $_GET['code']);
} else {
	header('Location: ' . $BASEPATH . '/locations.php');
}
