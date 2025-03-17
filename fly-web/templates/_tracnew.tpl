<!DOCTYPE html>
<html lang="en">
<head>
	<title>new tracker ticket :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		{@render trac_inline_style.css}
		table.trac td:first-child {
			background: #c8c8e8;
			font-weight: bold;
			width: 1%;
			white-space: nowrap;
		}
		table.trac input[type=text] {
			min-width: 70%;
		}
		table.trac textarea {
			width: 100%;
			height: 8em;
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<main>
		<p><a href="trac.php">Tracker</a> &gt; New ticket</p>
		<h2 id=main>Tracker: create new ticket</h2>
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
				<tr>
					<td>
					<td style=text-align:center>
						{@if isset($trac) && group_is_user_notbanned($usergroups)}
							{@input submit Comment}
						{@else}
							<p class="msg warning">You are not logged in or do not have permissions to create new tickets
						{@endif}
			</table>
		</form>
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
