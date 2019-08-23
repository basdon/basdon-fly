<!DOCTYPE html>
<html lang="en">
<head>
	<title>Category:{$category} :: basdon.net aviation server</title>
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
					<a href="article.php?category={@unsafe $c}">{@unsafe $c}</a>
				{@endforeach}
			</p>
			<h2 id="main">Category:{@unsafe $category}</h2>
			<div class="artcat">
				<p style="background:#{@unsafe $category_color}">Category: {@unsafe $category}</p>
				<p>
					{@eval $bull = ''}
					{@foreach $articles as $a}
						{@unsafe $bull}<a href="article.php?title={@unsafe $a->name}">{$a->title}</a>
						{@eval $bull = ' &bull; '}
					{@endforeach}
				</p>
			</div>
			<p style="text-align:right">
				<small>
					<a href="article.php">Main page</a> <a href="articleindex.php">Article index</a>
				</small>
			</p>
		</div>
		{@render aside.tpl}
		<div class="clear"></div>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
