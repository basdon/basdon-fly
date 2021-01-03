<?php
include('../inc/conf.php');

http_response_code(301);
if (isset($_GET['code']) && !is_array($_GET['code'])) {
	header('Location: ' . $BASEPATH . '/article.php?title=' . $_GET['code']);
} else {
	header('Location: ' . $BASEPATH . '/article.php?category=Airports');
}
