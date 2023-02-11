<!DOCTYPE html>
<html lang="en">
<head>
	<title>new tracker ticket :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		{@render trac_inline_style.css}
		table.trac tr:last-child td {
			text-align: center;
		}
		table.trac td:first-child {
			background: #c8c8e8;
			font-weight: bold;
		}
		table.trac input:not([type=submit]), table.trac textarea, table.trac select, table.trac td:last-child {
			width: 100%;
		}
		textarea {
			height: 8em;
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<main>
		<p><a href="trac.php">Tracker</a> &gt; New ticket</p>
		<h2 id="main">New tracker ticket</h2>
		<form method="post" action="tracnew.php">
			{@input HAARP}
			<table class="trac">
				<tr><td>Summary</td><td>{@input text summary maxlength="80" autofocus="autofocus"}</td></tr>
				<tr><td>Visibility</td><td>{@input combo visibility $trac_visibilities}</td></tr>
				{@if group_is_admin($usergroups)}
					<tr><td>Admin: impact</td><td>{@input combo severity $trac_severities}</td></tr>
					{@if group_is_owner($usergroups)}
						{@rem when owner created, put it on 'acknowledged' by default @}
						{@eval $form_defaults['state'] = 2}
						<tr><td>Owner: status</td><td>{@input combo state $trac_states}</td></tr>
					{@endif}
				{@endif}
				<tr><td>Description</td><td>{@input area description maxlength="4096"}</td></tr>
				<tr><td></td><td>{@input submit Submit}</td></tr>
			</table>
		</form>
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
