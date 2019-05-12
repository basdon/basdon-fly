<?php
include('../inc/bootstrap.php');

$article_name = 'Main_Page';
if (isset($_GET['title'])) {
	$article_name = $_GET['title'];
}
$article_title = 'Article not found';
$article_pageviews = null;

++$db_querycount;
$stmt = $db->prepare('SELECT id,title,pageviews FROM art WHERE name=? LIMIT 1');
$stmt->bindValue(1, $article_name);
if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r)) {
	$r = $r[0];

	++$db_querycount;
	$db->exec('UPDATE art SET pageviews=pageviews+1 WHERE id=' . $r->id);

	$article_title = $r->title;
	$article_pageviews = $r->pageviews + 1;
}

$__script = '_article';
include('../inc/output.php');
