<!DOCTYPE html>
<html lang="en">
<head>
	<title>change log :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		{@render trac_inline_style.css}
		table.trac td:first-child {
			white-space: nowrap;
		}
		h3 {
			margin-bottom: .25em;
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<main>
		{@if isset($selectedrelease)}
			<p><a href="trac.php">Tracker</a> &gt; <a href="tracversion.php">Change log</a> &gt; Release {@unsafe trac_releasetime($selectedrelease)}</p>
			<h2 id=main>Tracker: Change log for release {@unsafe trac_releasetime($selectedrelease)}</h2>
			{@if array_key_exists($selectedrelease, $mapping)}
				{@eval $ticketlist_tickets = $mapping[$selectedrelease]}
				{@render tracversionticketlist.tpl}
				<p style=font-size:small>
					GitHub release:
					&#32;
					<a href="https://github.com/basdon/{@unsafe $release_repo}/releases/tag/{@unsafe trac_releasetime($selectedrelease)}">
						{@unsafe $release_repo}@{@unsafe trac_releasetime($selectedrelease)}
					</a>
					&#32;
					(may or may not exist)
			{@else}
				<p class="msg error">This version does not exist!</p>
			{@endif}
		{@else}
			<p><a href="trac.php">Tracker</a> &gt; Change log</p>
			<h2 id=main>Tracker: Change log</h2>

			{@if group_is_owner($usergroups)}
				<p><a href="tracversion.php?releaseconfirm">[Owner: release]</a></p>
				{@if isset($_GET['releaseconfirm'])}
					<p><a href="tracversion.php?dorelease&rel={@unsafe time()}">[Owner: yes, really do release]</a></p>
				{@endif}
			{@endif}

			<h3>Tickets ready for next release</h3>
			{@eval $ticketlist_tickets = $readytickets}
			{@render tracversionticketlist.tpl}
			{@unsafe $pagination = simple_pagination('tracversion.php?page=', $page, $releasecount, 10)}

			{@foreach $releaseids as $release}
				<h3><a href="tracversion.php?rel={@unsafe $release}">{@unsafe trac_releasetime($release)}</a></h3>
				{@eval $ticketlist_tickets = $mapping[$release]}
				{@render tracversionticketlist.tpl}
			{@endforeach}

			{@unsafe $pagination}
		{@endif}
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
