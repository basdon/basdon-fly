<!DOCTYPE html>
<html lang="en">
<head>
	<title>credits :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<main>
		<h2 id="main">Credits</h2>
		<ul>
			<li>
				<p><strong>Programming</strong>: robin_be</p>
			</li>
			<li>
				<p><strong>Mapping</strong>: {@unsafe $SETTING__CREDITS_MAPP}</p>
				<p><small>See the airport articles (<a href="article.php?category=Airports">Category: Airports</a>) to see who mapped what airport. If not mentioned, it's probably robin_be.</small></p>
			</li>
			<li>
				<p><strong>Brand graphics</strong>: Jenis</p>
			</li>
			<li>
				<p><strong>Beta testers</strong>: {@unsafe $SETTING__CREDITS_BETA}</p>
			</li>
			<li>
				<p><strong>Special thanks</strong>: {@unsafe $SETTING__CREDITS_SPEC}</p>
			</li>
			<li>
				<p><strong>Inspiration and ideas</strong>: {@unsafe $SETTING__CREDITS_INSP}</p>
			</li>
		</ul>
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
