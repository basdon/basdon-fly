<!DOCTYPE html>
<html lang=en>
<head>
	<title>{$article_title} :: basdon.net aviation server</title>
	<link rel=stylesheet href=/s/article.css?v3 type=text/css>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<main class=article>
		<p>
			<a href=article.php>Articles</a>
			{@foreach $categories as $c}
				&#32;&gt;&#32;
				<a href="article.php?category={@urlencode $c->name}">{@unsafe $c->name}</a>
			{@endforeach}
			&#32;&gt;&#32;
			<a href="article.php?title={$article_name}">{$article_title}</a>
			{@if $article_redirected_from != null}
				<small> (Redirected from {@unsafe $article_redirected_from})</small>
			{@endif}
		</p>
		{@if !isset($article_filename)}
			<h2 id=main>Article not found!</h2>
			<p>The article with name '{$article_name}' was not found.</p>
		{@else}
			<h2 id=main>{@unsafe $article_title}</h2>
			{@if !is_file($article_filename)}
				<p class="msg error">Failed to get article contents.</p>
			{@else}
				{@eval include $article_filename}
			{@endif}
			{@if isset($cat)}
				{@rem don't show category box on uncategorized pages @}
				{@render article_category_box.tpl}
			{@endif}
		{@endif}
		{@render article_bottom_right_links.tpl}
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
