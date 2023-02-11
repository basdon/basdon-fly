<!DOCTYPE html>
<html lang="en">
<head>
	<title>Article Index :: basdon.net aviation server</title>
	<link rel="stylesheet" href="/s/article.css?v2" type="text/css" />
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<main class=article>
		<h2 id="main">Article index</h2>
		{@foreach $cats as $cat}
			{@render article_category_box.tpl}
		{@endforeach}
		{@render article_bottom_right_links.tpl}
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
