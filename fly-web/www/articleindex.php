<?php
require '../inc/bootstrap.php';

$cats = [];
$subcats = []; // temp to have mapping to load articles into

$cat = new stdClass();
$cat->color = 'dddddd';
$cat->name = 'Uncategorized';
$cat->articles = [];
$cat->subs = [];
$cats[null] = $cat;

++$db_querycount;
foreach ($db->query('SELECT id,parent,name,color FROM artcat ORDER BY parent ASC,name ASC') as $cat) {
	$cat->articles = [];
	$cat->subs = [];
	if ($cat->parent == null) {
		$cats[$cat->id] = $cat;
	} else {
		$subcats[$cat->id] = $cat;
		$cats[$cat->parent]->subs[] = $cat;
	}
}

++$db_querycount;
foreach ($db->query('SELECT id,cat,name,title FROM art ORDER BY cat ASC,title ASC') as $art) {
	if (array_key_exists($art->cat, $cats)) {
		$cats[$art->cat]->articles[] = $art;
	} else if (array_key_exists($art->cat, $subcats)) {
		$subcats[$art->cat]->articles[] = $art;
	} else {
		$__msgs[] = 'Failed to load article '.$art->name.' into category '.$art->cat;
	}
}

unset($subcats);

$__script = '_articleindex';
require '../inc/output.php';
