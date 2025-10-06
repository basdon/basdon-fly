<!DOCTYPE html>
<html lang=en>
<head>
	<title>{$article_title} (deleted article) :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<main>
		<p>
			<a href=article.php>Articles</a>
			&#32;&gt;&#32;
			{$article_title} (deleted article)
		</p>
		<h2 id=main>Article deleted!</h2>
		<p>
			This article ({$article_title}) used to exist, but has been deleted.
		{@render article_bottom_right_links.tpl}
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
