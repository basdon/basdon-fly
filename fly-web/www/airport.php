<?php
include('../inc/funcs.php');

http_response_code(301);
if (isset($_GET['code'])) {
	header('Location: ' . $ABS_URL . '/article.php?title=' . header_value($_GET['code']));
} else {
	header('Location: ' . $ABS_URL . '/article.php?category=Airports');
}
