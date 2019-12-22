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

		{@if group_is_owner($usergroups)}
			<p><a href="tracversion.php?releaseconfirm">[Owner: release]</a></p>
			{@if isset($_GET['releaseconfirm'])}
				<p><a href="tracversion.php?dorelease&rel={@unsafe time()}">[Owner: yes, really do release]</a></p>
			{@endif}
		{@endif}

		{@if !isset($selectedrelease)}
			<h3>Tickets ready for next release</h3>
			{@eval $list = $readytickets}
			{@render tracversionticketlist.tpl}
			{@unsafe $pagination = simple_pagination('tracversion.php?page=', $page, $releasecount, 10)}
		{@endif}

		{@foreach $releaseids as $release}
			<h3>{@unsafe trac_releasetime($release)}</h3>
			{@eval $list = $mapping[$release]}
			{@render tracversionticketlist.tpl}
		{@endforeach}

		{@if isset($pagination)}
			{@unsafe $pagination}
		{@endif}
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
