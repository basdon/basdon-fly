<?php
include('../inc/bootstrap.php');

$article_categories = [];

++$db_querycount;
$cats = $db->query('SELECT id,parent,name,color FROM artcat');
if ($cats !== false) {
	foreach ($cats as $c) {
		$article_categories[$c->id] = $c;
	}
}

++$db_querycount;
$articles = $db->query('SELECT id,cat,name,title FROM art ORDER BY cat ASC,title ASC');

$__script = '_articleindex';
include('../inc/output.php');
