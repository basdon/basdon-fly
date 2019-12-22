<!DOCTYPE html>
<html lang="en">
<head>
	<title>change log :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		{@render trac_inline_style.css}
		.singlebody ul {
			margin: 0;
		}
		.singlebody h3 {
			margin-bottom: .5em;
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		<p><a href="trac.php">Tracker</a> &gt; Change log</p>
		<h2 id="main">Change log</h2>

		<h3>Tickets ready for next version</h3>
		<ul>
			{@eval $list = $readytickets}
			{@render tracversionticketlist.tpl}
		</ul>

		{@foreach $releaseids as $release}
			<h3>{@unsafe trac_releasetime($release)}</h3>
			{@eval $list = $mapping[$release]}
			{@render tracversionticketlist.tpl}
		{@endforeach}

		{@unsafe $pagination = simple_pagination('tracversion.php?page=', $page, $releasecount, 10)}
		{@unsafe $pagination}
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
