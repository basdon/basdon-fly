<!DOCTYPE html>
<html lang="en">
<head>
	<title>{$trac_summary} :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		{@render trac_inline_style.css}
		table.trac td.label {
			background: #c8c8e8;
			font-weight: bold;
		}
		table.trac.respond td:first-child {
			width: 1%;
			white-space: nowrap;
		}
		table.trac.respond input[type=text] {
			min-width: 70%;
		}
		table.trac textarea {
			width: 100%;
			height: 8em;
		}
		table.trac td.visibility:not(.v{@unsafe $GROUPS_ALL}) {
			background: #fff1d4;
			font-weight: bold;
		}
		table.trac + table.trac {
			margin-top: 1.5em;
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<main>
		<p>
			<a href=trac.php>Tracker</a>
			{@if $trac_released}
				&#32;&gt;&#32;
				<a href="tracversion.php?rel={@unsafe $trac_released}">Release {@unsafe $trac_released_display}</a>
			{@endif}
			&#32;&gt;&#32;
			Ticket: "{$trac_summary}"

		<h2 id=main>Tracker ticket: {$trac_summary}</h2>

		{@if isset($trac)}
			<table id="main" class="trac">
				<tr>
					<td class=label colspan=4>
						{@unsafe linkuser($trac)} {@unsafe format_datetime($trac->stamp)}
						<span style=float:right><a href="tracview.php?id={@unsafe $id}">#0</a></span>
				</tr>
				<tr>
					<td class="label">Status</td><td class="state{@unsafe $trac->state}">{@unsafe $trac_states[$trac->state]}</td>
					<td class="label">Impact</td><td class="severity{@unsafe $trac->severity}">{@unsafe $trac_severities[$trac->severity]}</td>
				<tr>
					<td class=label>
						Released
					<td>
						{@if $trac_released}
							<a href="tracversion.php?rel={@unsafe $trac_released}">
								{@unsafe $trac_released_display}
							</a>
						{@else}
							-
						{@endif}
					<td class=label>
						Visibility
					<td class="visibility v{@unsafe $trac->visibility}">
						{@if array_key_exists($trac->visibility, $trac_visibilities)}
							{@unsafe $trac_visibilities[$trac->visibility]}
						{@else}
							[invalid value: {@unsafe $trac->visibility}]
						{@endif}
				<tr>
					<td class="label" colspan="4">Description</td>
				</tr>
				<tr>
					<td colspan="4"><p>{@nl2br $trac->description}</p></td>
				</tr>
			</table>
			{@eval $idx = 1}
			{@foreach $comments as $c}
				<table id="tracc{@unsafe $c->id}" class="trac">
					<tr>
						<td class="label">
							{@unsafe linkuser($c)} {@unsafe format_datetime($c->stamp)}
							<span style="float:right;"><a href="tracview.php?id={@unsafe $id}#tracc{@unsafe $c->id}">#{@unsafe $idx}</a></span>
							{@eval $idx++}
						</td>
					</tr>
					<tr>
						<td{@if $c->type == 1} style="background:#fff1d4"{@endif}>
							<p>
								{@if $c->type == 1}
									{@unsafe $c->comment}
								{@else}
									{@nl2br $c->comment}
								{@endif}
							</p>
						</td>
					</tr>
				</table>
			{@endforeach}
		{@else}
			<p class="msg error">This ticket does not exist or is not visible for you.</p>
		{@endif}

		<h3>Add comment</h3>
		<form method=post action="tracview.php?id={@unsafe $id}">
			{@input HAARP}
			<table class="trac respond">
				<tr>
					<td>{@input area comment}
				<tr>
					<td style=text-align:center>
						{@if isset($trac) && group_is_user_notbanned($usergroups)}
							{@input submit Comment}
						{@else}
							<p class="msg warning">You are not logged in or do not have permissions to place comments on this ticket
						{@endif}
			</table>
		</form>

		{@if isset($trac) && group_is_admin($usergroups)}
			<h3>Admin: Update ticket</h3>
			<form method=post action="tracview.php?id={@unsafe $id}">
				{@input HAARP}
				<table class="trac respond">
					{@eval $form_defaults['summary'] = $trac->summary}
					{@eval $form_defaults['state'] = $trac->state}
					{@eval $form_defaults['severity'] = $trac->severity}
					{@eval $form_defaults['visibility'] = $trac->visibility}
					<tr><td class=label>Summary<td>{@input text summary maxlength=80}
					{@if group_is_owner($usergroups)}
						<tr><td class=label>Owner: status<td>{@input combo state $trac_states}
					{@endif}
					<tr><td class=label>Impact<td>{@input combo severity $trac_severities}
					<tr><td class=label>Visibility<td>{@input combo visibility $trac_visibilities}
					<tr><td><td style=text-align:center>{@input submit Update}
				</table>
			</form>
		{@endif}
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
