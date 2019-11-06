<!DOCTYPE html>
<html lang="en">
<head>
	<title>Article Index :: basdon.net aviation server</title>
	<link rel="stylesheet" href="{@unsafe $STATICPATH}/article.{@unsafe $CSS_SUFFIX}" type="text/css" />
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div>
		<div id="main" class="article">
			<h2>Article index</h2>
			{@if $articles === false}
				<p>Failed to get list of articles.</p>
			{@else}
				{@eval $lastcat = -1; $div_open = false; $bull = ''}
				{@foreach $articles as $a}
					{@if $a->cat != $lastcat}
						{@if $div_open}</p></div>{@endif}
						{@eval $lastcat = $a->cat; $div_open = true}
						{@if $lastcat != null && array_key_exists($lastcat, $article_categories)}
							{@eval $cat = $article_categories[$lastcat]}
						{@else}
							{@eval $cat = new stdclass();
							       $cat->name = 'Uncategorized';
							       $cat->parent = null;
							       $cat->color = 'dddddd'}
						{@endif}
						<div class="artcat">
							<p style="background:#{@unsafe $cat->color}">
								Category: <a href="article.php?category={@urlencode $cat->name}">{@unsafe $cat->name}</a>
							</p>
							<p>
						{@eval $bull = ''}
					{@endif}
					{@unsafe $bull}<a href="article.php?title={@unsafe $a->name}">{@unsafe $a->title}</a>
					{@eval $bull = ' &bull; '}
				{@endforeach}
				{@if $div_open}</p></div>{@endif}
			{@endif}
			{@render article_bottom_right_links.tpl}
		</div>
		{@render aside.tpl}
		<div class="clear"></div>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
