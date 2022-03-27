<!DOCTYPE html>
<html lang="en">
<head>
	<title>brand :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div><div>
		<h2 id="main">Brand</h2>
		<p>Basdon's brand graphics were made by player Jenis. The official colors are white <code>#ffffff</code> and blue <code>#003df1</code>.</p>
		<h3>Full transparent logo</h3>
		<p><img src="{@unsafe $STATICPATH}/brand/logo-full.png" title="BAS logo with written out 'basdon.net avaiation server' underneath" style="background:#000;max-width:200px"></p>
		<h3>URL transparent logo</h3>
		<p><img src="{@unsafe $STATICPATH}/brand/logo-url.png" title="BAS logo with 'basdon.net' underneath" style="background:#000;max-width:200px"></p>
		<h3>Compact transparent logo</h3>
		<p><img src="{@unsafe $STATICPATH}/brand/logo-compact.png" title="BAS logo with 'basdon.net' inside" style="background:#000;max-width:200px"></p>
		<h3>Square opaque logo</h3>
		<p><img src="{@unsafe $STATICPATH}/brand/square.png" title="BAS logo with 'basdon.net' underneath, on blue background" style="max-width:200px"></p>
		<h3>Favicon</h3>
		<p><img src="{@unsafe $STATICPATH}/favicon.ico" title="'B' styled as the BAS logo on blue background"></p>
	</div></div>
	{@render defaultfooter.tpl}
</body>
</html>
