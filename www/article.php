<?php
include('../inc/bootstrap.php');

function fetch_category_box($categoryname, $categoryid, &$cat, &$categories)
{
	global $db, $db_querycount;

	$cat = new stdClass();
	$cat->id = -1;
	$cat->color = 'dddddd';
	$cat->name = 'Uncategorized';
	$cat->articles = [];
	$cat->parent = null;
	$cat->subs = [];
	$categories = [$cat];

	if ($categoryname != null) {
		$stmt = $db->prepare('SELECT id,parent,name,color FROM artcat WHERE name=? LIMIT 1');
		$stmt->bindValue(1, $categoryname);
	} else if ($categoryid != null) {
		$stmt = $db->prepare('SELECT id,parent,name,color FROM artcat WHERE id=? LIMIT 1');
		$stmt->bindValue(1, $categoryid);
	} else {
		return;
	}

	++$db_querycount;
	if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r)) {
		$r = $r[0];
		$cat->id = $r->id;
		$cat->name = $r->name;
		$cat->color = $r->color;
		$category_ids = [$r->id];

		$catmapping = [];
		$catmapping[$r->id] = $cat;

		$parentcat = $cat;

		// traverse parent categories
		$parentid = $r->parent;
		if ($parentid != null) {
			$stmt = $db->prepare('SELECT parent,name,color FROM artcat WHERE id=? LIMIT 1');
nextparentcat4catpage:
			$stmt->bindValue(1, $parentid);
			++$db_querycount;
			if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r)) {
				$r = $r[0];
				$p = new stdClass();
				$p->color = $r->color;
				$p->name = $r->name;
				$p->parent = null;
				array_unshift($categories, $p);
				$p->subs = [$parentcat];
				$parentcat->parent = $p;
				$parentcat = $p;
				if (($parentid = $r->parent) != null) {
					goto nextparentcat4catpage;
				}
			}
		}

		// fetch all child categories (one level)
		++$db_querycount;
		foreach ($db->query('SELECT id,name,color FROM artcat WHERE parent='.$cat->id.' ORDER BY name ASC') as $r) {
			$sub = new stdClass();
			$sub->id = $r->id;
			$sub->name = $r->name;
			$sub->color = $r->color;
			$sub->articles = [];
			$cat->subs[] = $sub;
			$category_ids[] = $r->id;
			$catmapping[$r->id] = $sub;
		}

		// fetch all articles (either in category or in subcategories)
		++$db_querycount;
		foreach ($db->query('SELECT name,title,cat FROM art WHERE cat IN ('.implode(',',$category_ids).') ORDER BY name ASC') as $r) {
			$catmapping[$r->cat]->articles[] = $r;
		}
		unset($catmapping);
		unset($category_ids);
	} else {
		++$db_querycount;
		foreach ($db->query('SELECT name,title FROM art WHERE ISNULL(cat) ORDER BY name ASC') as $a) {
			$cat->articles[] = $a;
		}
	}
}

if (isset($_GET['category'])) {
	fetch_category_box($_GET['category'], null, $cat, $categories);
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

	fetch_category_box(null, $r->cat, $cat, $categories);
	// to not show category box on uncategorized pages
	if ($cat->id == -1) {
		unset($cat);
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
