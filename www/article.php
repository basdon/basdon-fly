<?php
include('../inc/bootstrap.php');

if (isset($_GET['category'])) {
	++$db_querycount;
	$stmt = $db->prepare('SELECT id,parent,name,color FROM artcat WHERE name=? LIMIT 1');
	$stmt->bindValue(1, $_GET['category']);

	$category = 'Uncategorized';
	$categories = [$category];
	$category_color = 'dddddd';
	$articles = [];
	if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r)) {
		$r = $r[0];
		$category = $r->name;
		$categoryid = $r->id;
		$categories = [$category];
		$category_color = $r->color;

		$parent = $r->parent;
		if ($parent != null) {
			$stmt = $db->prepare('SELECT parent,name FROM artcat WHERE id=? LIMIT 1');
nextparentcat4catpage:
			$stmt->bindValue(1, $parent);
			++$db_querycount;
			if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r)) {
				$r = $r[0];
				array_push($categories, $r->name);
				if (($parent = $r->parent) != null) {
					goto nextparentcat4catpage;
				}
			}
		}

		++$db_querycount;
		$stmt = $db->prepare('SELECT name,title FROM art WHERE cat=?');
		$stmt->bindValue(1, $categoryid);
		if ($stmt->execute()) {
			$articles = $stmt;
		}
	}

	$__script = '_article_category';
	include('../inc/output.php');
	die();
}

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
			++$db_querycount;
			$arts = $db->query('SELECT name,title FROM art WHERE cat=' . $cat);
			if ($arts != null) {
				$r[0]->articles = $arts->fetchAll();
			}
			if (($cat = $r[0]->parent) != null) {
				goto nextparentcat;
			}
		}
	}
}

$__script = '_article';
include('../inc/output.php');
