<!DOCTYPE html>
<html lang="en">
<head>
	<title>Article Index :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div>
		<div class="mainright article">
			<h2>Article index</h2>
			{@if $articles === false}
				<p>Failed to get list of articles.</p>
			{@else}
				{@eval $lastcat = -1; $ul_open = false}
				{@foreach $articles as $a}
					{@if $a->cat != $lastcat}
						{@if $ul_open}</ul>{@endif}
						{@eval $lastcat = $a->cat; $ul_open = true}
						{@if $lastcat != null && array_key_exists($lastcat, $article_categories)}
							{@eval $cat = $article_categories[$lastcat]}
						{@else}
							{@eval $cat = new stdclass();
							       $cat->name = 'Uncategorized';
							       $cat->parent = null;
							       $cat->color = 'dddddd'}
						{@endif}
						<h3 style="background:#{@unsafe $cat->color}">
							<a href="article.php?category={@unsafe $cat->name}">{@unsafe $cat->name}</a>
						</h3>
						<ul>
					{@endif}
					<li><a href="article.php?title={@unsafe $a->name}">{$a->title}</a></li>
				{@endforeach}
				{@if $ul_open}</ul>{@endif}
			{@endif}
		</div>
		{@render aside.tpl}
		<div class="clear"></div>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
