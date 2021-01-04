<?php

require '../inc/conf.php';
require '../inc/db.php';

$articlemap = [];
foreach ($db->query('SELECT name,title FROM art') as $a) {
	$articlemap[$a->name] = $a->title;
}
$articlecatmap = [];
foreach ($db->query('SELECT name FROM artcat') as $a) {
	$articlecatmap[$a->name] = $a->name;
}

// don't add closing </a> tag, article gen takes care of that
$output = '<?php function map_article($name){$articlemap=' . var_export($articlemap, true) . ';';
$output .= <<<'EOD'
	$a='<a href="article.php?title='.$name;
	if(!array_key_exists($name,$articlemap)){
		$a.='" style="color:red';
	} else {
		$name=$articlemap[$name];
	}
	return $a.'">'.$name;
}
EOD;
$output .= 'function map_article_cat($name){$articlecatmap=' . var_export($articlecatmap, true) . ';';
$output .= <<<'EOD'
	$a='<a href="article.php?category='.$name;
	if(!array_key_exists($name,$articlecatmap)){
		$a.='" style="color:red';
	} else {
		$name=$articlecatmap[$name];
	}
	return $a.'">'.$name;
}
EOD;
file_put_contents('../gen/articlemap.php', $output);
