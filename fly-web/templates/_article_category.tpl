<!DOCTYPE html>
<html lang="en">
<head>
	<title>Category:{$cat->name} :: basdon.net aviation server</title>
	<link rel="stylesheet" href="/s/article.css?v2" type="text/css" />
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<main class=article>
		<p>
			<a href="article.php">Articles</a>
			{@foreach $categories as $c}
				&#32;&gt;&#32;
				<a href="article.php?category={@urlencode $c->name}">Category: {@unsafe $c->name}</a>
			{@endforeach}
		</p>
		<h2 id="main">Category: {@unsafe $cat->name}</h2>
		{@render article_category_box.tpl}
		{@render article_bottom_right_links.tpl}
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
