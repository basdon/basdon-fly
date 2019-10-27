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
		$articles = $db->prepare('SELECT name,title FROM art WHERE cat=? ORDER BY name ASC');
		$articles->bindValue(1, $categoryid);
		$articles->execute();
	} else {
		++$db_querycount;
		$articles = $db->query('SELECT name,title FROM art WHERE ISNULL(cat) ORDER BY name ASC');
	}

	$__script = '_article_category';
	include('../inc/output.php');
	die();
}

$article_name = 'Main_Page';
if (isset($_GET['title'])) {
	$article_name = $_GET['title'];
}
$article_id = -1;
$article_title = 'Article not found';
$article_pageviews = null;
$article_categories = [];
$article_redirected_from = null;

++$db_querycount;
$stmt = $db->prepare('SELECT id,name,title,pageviews,cat FROM art WHERE name=? LIMIT 1');
$stmt->bindValue(1, $article_name);
if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r)) {
	$r = $r[0];

	++$db_querycount;
	$db->exec('UPDATE art SET pageviews=pageviews+1 WHERE id=' . $r->id);

	$article_id = $r->id;
	$article_name = $r->name;
	$article_title = $r->title;
	$article_pageviews = $r->pageviews + 1;

	if (isset($_GET['from'])) {
		$stmt = $db->prepare('SELECT art,alt FROM artalt WHERE alt=?');
		$stmt->bindValue(1, $_GET['from']);
		if ($stmt->execute() && ($r2 = $stmt->fetchAll()) && count($r2) && $r2[0]->art == $r->id) {
			$article_redirected_from = $r2[0]->alt;
		}
	}

	$cat = $r->cat;
	if ($cat != null) {
		$stmt = $db->prepare('SELECT name,color,parent FROM artcat WHERE id=?');
nextparentcat:
		$stmt->bindValue(1, $cat);
		++$db_querycount;
		if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r)) {
			array_push($article_categories, $r[0]);
			++$db_querycount;
			$arts = $db->query('SELECT id,name,title FROM art WHERE cat=' . $cat);
			if ($arts != null) {
				$r[0]->articles = $arts->fetchAll();
			}
			if (($cat = $r[0]->parent) != null) {
				goto nextparentcat;
			}
		}
	}
} else {
	$stmt = $db->prepare('SELECT art.name FROM art JOIN artalt ON art.id = artalt.art WHERE artalt.alt=?');
	$stmt->bindValue(1, $article_name);
	if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r)) {
		http_response_code(301);
		header('Location: '.$BASEPATH.'/article.php?title='.$r[0]->name.'&from='.$article_name);
		die();
	}
}

$__script = '_article';
include('../inc/output.php');
