<!DOCTYPE html>
<html lang="en">
<head>
	<title>locations :: basdon.net aviation server</title>
	<link rel="stylesheet" href="{@unsafe $STATICPATH}/article.{@unsafe $CSS_SUFFIX}" type="text/css" />
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		<h2 id="main">Locations</h2>
		{@if isset($location_not_found_message)}
			<p class="msg error">{$location_not_found_message}</p>
		{@endif}
		{@eval $group = $airports_mainland; $groupname = 'Mainland San Andreas'}
		{@render location_group.tpl}
		{@eval $group = $airports_ne; $groupname = 'North-east San Andreas'}
		{@render location_group.tpl}
		{@eval $group = $airports_nw; $groupname = 'North-west San Andreas'}
		{@render location_group.tpl}
		{@eval $group = $airports_se; $groupname = 'South-east San Andreas'}
		{@render location_group.tpl}
		{@eval $group = $airports_sw; $groupname = 'South-west San Andreas'}
		{@render location_group.tpl}
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
