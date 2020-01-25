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
		table.trac.respond input:not([type=submit]), table.trac.respond textarea, table.trac.respond select, table.trac.respond td:last-child {
			width: 100%;
		}
		textarea {
			height: 8em;
		}
		table.trac p {
			margin: .5em 0;
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		<p><a href="trac.php">Tracker</a> {@if $trac_released != null}&gt; <a href="tracversion.php?rel={@unsafe $trac_released}">Release {@unsafe $trac_released_display}</a> {@endif}&gt; {$trac_summary}</p>
		{@if isset($trac)}
			<h2 id="main">Tracker: {$trac->summary}</h2>
			<table id="main" class="trac">
				<tr>
					<td class="label">Summary</td><td colspan="3">{$trac->summary}</td>
				</tr>
				<tr>
					<td class="label">Status</td><td class="state{@unsafe $trac->state}">{@unsafe $trac_states[$trac->state]}</td>
					<td class="label">Reporter</td><td>{@unsafe linkuser($trac)}</td>
				</tr>
				<tr>
					<td class="label">Impact</td><td class="severity{@unsafe $trac->severity}">{@unsafe $trac_severities[$trac->severity]}</td>
					<td class="label">Reported</td><td>{@unsafe format_datetime($trac->stamp)}</td>
				</tr>
				<tr>
					<td class="label">Visibility</td><td class="visibility{@unsafe $trac->visibility}">{@unsafe $trac_visibility}</td>
					<td class="label">Updated</td><td>{@unsafe format_datetime($trac->updated)}</td>
				</tr>
				<tr>
					<td class="label">Released</td><td colspan="3">{@if $trac_released != null}<a href="tracversion.php?rel={@unsafe $trac_released}">{@unsafe $trac_released_display}</a>{@else}-{@endif}</td>
				</tr>
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
			{@if group_is_user_notbanned($usergroups)}
				<h4>Add comment</h4>
				<form method="post" action="tracview.php?id={@unsafe $id}">
					{@input HAARP}
					<table class="trac respond">
						<tr>
							<td>{@input area comment}</td>
						</tr>
						<tr>
							<td style="text-align:center">{@input submit Comment}</td>
						</tr>
					</table>
				</form>
				{@if group_is_admin($usergroups)}
					<h4>Admin: Update ticket</h4>
					<form method="post" action="tracview.php?id={@unsafe $id}">
						{@input HAARP}
						<table class="trac respond">
							{@eval $form_defaults['summary'] = $trac->summary}
							{@eval $form_defaults['state'] = $trac->state}
							{@eval $form_defaults['severity'] = $trac->severity}
							{@eval $form_defaults['visibility'] = $trac->visibility}
							<tr><td class="label">Summary</td><td>{@input text summary maxlength="80"}</td></tr>
							{@if group_is_owner($usergroups)}
								<tr><td class="label">Owner: status</td><td>{@input combo state $trac_states}</td></tr>
							{@endif}
							<tr><td class="label">Impact</td><td>{@input combo severity $trac_severities}</td></tr>
							<tr><td class="label">Visibility</td><td>{@input combo visibility $trac_visibilities}</td></tr>
							<tr>
								<td></td><td style="text-align:center">{@input submit Update}</td>
							</tr>
						</table>
					</form>
				{@endif}
			{@endif}
		{@else}
			<h2 id="main">Tracker</h2>
			<p>This ticket does not exist or is not visible for you.</p>
		{@endif}
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
