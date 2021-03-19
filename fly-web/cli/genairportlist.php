<?php

require '../inc/conf.php';
require '../inc/db.php';

ob_start();
foreach ($db->query('SELECT apt.c,art.title,art.name FROM apt JOIN artalt ON artalt.alt = apt.c JOIN art ON art.id = artalt.art') as $a) {
	echo("<div>");
	$code = strtolower($a->c);
	echo("<a href=\"article.php?title={$a->name}\"><img src=\"{$STATICPATH}/articles/{$code}.jpg\" alt=\"{$a->c}\" title=\"{$a->c}\"/></a>");
	echo("<br/>");
	echo("<a href=\"article.php?title={$a->name}\">{$a->title}</a>");
	echo("</div>");
}
file_put_contents('../gen/airportlist.html', ob_get_contents());
ob_end_clean();
