<?php
include('../inc/funcs.php');

http_response_code(301);
if (isset($_GET['code'])) {
	header('Location: ' . $BASEPATH . '/article.php?title=' . header_value($_GET['code']));
} else {
	header('Location: ' . $BASEPATH . '/article.php?category=Airports');
}
