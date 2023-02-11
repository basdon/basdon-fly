<!DOCTYPE html>
<html lang="en">
<head>
	<title>changelog :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<main>
		<h2 id="main">Changelog</h2>
		<p>This list only contains written up change notes with the most notable changes. A list of every single change can be found by looking at the <a href="https://github.com/basdon/basdon-fly/commits/master">commits of the basdon/basdon-fly github repository</a>{@globe}.</p>
		<ul>
			{@foreach fetch_changes() as $change}
				<li><strong>{@unsafe $change->stamp}</strong>: {$change->entry}</li>
			{@endforeach}
		</ul>
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
