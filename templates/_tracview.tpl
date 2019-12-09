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
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		<p><a href="trac.php">Tracker</a> &gt; {$trac_summary}</p>
		{@if isset($trac)}
			<h2 id="main">Tracker: {$trac->summary}</h2>
			<table id="main" class="trac">
				<tr>
					<td class="label">Summary</td><td colspan="5">{$trac->summary}</td>
				</tr>
				<tr>
					<td class="label">Status</td><td class="status{@unsafe $trac->status}">{@unsafe $trac_statuses[$trac_status]}</td>
					<td class="label">Impact</td><td>{@unsafe $trac_severities[$trac_impact]}</td>
					<td class="label">Reported</td><td>{@unsafe format_datetime($trac->stamp)}</td>
				</tr>
				<tr>
					<td class="label">Reporter</td><td>{@unsafe linkuser($trac)}</td>
					<td class="label">Visibility</td><td>{@unsafe $trac_visibility}</td>
					<td class="label">Updated</td><td>{@unsafe format_datetime($trac->updated)}</td>
				</tr>
				<tr>
					<td class="label" colspan="6">Description</td>
				</tr>
				<tr>
					<td colspan="6">{$trac->description}</td>
				</tr>
			</table>
		{@else}
			<h2 id="main">Tracker</h2>
			<p>This ticket does not exist or is not visible for you.</p>
		{@endif}
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
