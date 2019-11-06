<!DOCTYPE html>
<html lang="en">
<head>
	<title>Category:{$cat->name} :: basdon.net aviation server</title>
	<link rel="stylesheet" href="{@unsafe $STATICPATH}/article.{@unsafe $CSS_SUFFIX}" type="text/css" />
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div>
		<div class="mainright article">
			<p>
				<a href="article.php">Articles</a>
				{@foreach $categories as $c}
					&#32;&gt;&#32;
					<a href="article.php?category={@urlencode $c->name}">{@unsafe $c->name}</a>
				{@endforeach}
			</p>
			<h2 id="main">Category: {@unsafe $cat->name}</h2>
			{@render article_category_box.tpl}
			{@render article_bottom_right_links.tpl}
		</div>
		{@render aside.tpl}
		<div class="clear"></div>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
