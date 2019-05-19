<?php
include('../inc/bootstrap.php');

$article_name = 'Main_Page';
if (isset($_GET['title'])) {
	$article_name = $_GET['title'];
}
$article_title = 'Article not found';
$article_pageviews = null;
$article_categories = [];

++$db_querycount;
$stmt = $db->prepare('SELECT id,name,title,pageviews,cat FROM art WHERE name=? LIMIT 1');
$stmt->bindValue(1, $article_name);
if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r)) {
	$r = $r[0];

	++$db_querycount;
	$db->exec('UPDATE art SET pageviews=pageviews+1 WHERE id=' . $r->id);

	$article_name = $r->name;
	$article_title = $r->title;
	$article_pageviews = $r->pageviews + 1;

	$cat = $r->cat;
	if ($cat != null) {
		$stmt = $db->prepare('SELECT name,color,parent FROM artcat WHERE id=?');
nextparentcat:
		$stmt->bindValue(1, $cat);
		++$db_querycount;
		if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r)) {
			array_push($article_categories, $r[0]);
			if (($cat = $r[0]->parent) != null) {
				goto nextparentcat;
			}
		}
	}
}

$__script = '_article';
include('../inc/output.php');
