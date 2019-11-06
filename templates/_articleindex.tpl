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
			{@foreach $cats as $cat}
				{@render article_category_box.tpl}
				<br/>
			{@endforeach}
			{@render article_bottom_right_links.tpl}
		</div>
		{@render aside.tpl}
		<div class="clear"></div>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
