<!DOCTYPE html>
<html lang="en">
<head>
	<title>changelog :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div><div>
		<h2 id="main">Changelog</h2>
		<ul>
			{@foreach fetch_changes() as $change}
				<li><strong>{@unsafe $change->stamp}</strong>: {$change->entry}</li>
			{@endforeach}
		</ul>
	</div></div>
	{@render defaultfooter.tpl}
</body>
</html>
