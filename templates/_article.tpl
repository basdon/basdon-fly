<!DOCTYPE html>
<html lang="en">
<head>
	<title>{$article_title} :: basdon.net aviation server</title>
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
				{@foreach $article_categories as $c}
					&#32;&gt;&#32;
					<a href="article.php?category={@unsafe $c->name}">{@unsafe $c->name}</a>
				{@endforeach}
				&#32;&gt;&#32;
				<a href="article.php?title={$article_name}">{$article_name}</a>
				{@if $article_redirected_from != null}
					<small> (Redirected from {@unsafe $article_redirected_from})</small>
				{@endif}
			</p>
			{@if $article_pageviews == null}
				<h2 id="main">Article not found!</h2>
				<p>The article with name '{$article_name}' was not found.</p>
			{@else}
				<h2 id="main">{@unsafe $article_title}</h2>
				{@eval $file = '../articles/gen/' . $article_name . '.html'}
				{@if !is_file($file)}
					<p style="color:red">Failed to get article contents.</p>
				{@else}
					{@eval include($file)}
				{@endif}
				{@if count($article_categories)}
					<div class="artcat">
						{@foreach $article_categories as $c}
							<p style="background:#{@unsafe $c->color}">
								Category: <a href="article.php?category={@unsafe $c->name}">{@unsafe $c->name}</a>
							</p>
							<p>
								{@eval $bull = ''}
								{@foreach $c->articles as $a}
									{@unsafe $bull}
									<a href="article.php?title={@unsafe $a->name}">{@unsafe $a->title}</a>
									{@eval $bull = ' &bull; '}
								{@endforeach}
							</p>
						{@endforeach}
					</div>
				{@endif}
			{@endif}
			<p style="text-align:right">
				<small>
					{@if $article_pageviews != null}This article has been accessed {@unsafe $article_pageviews} time(s).<br/>{@endif}
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
