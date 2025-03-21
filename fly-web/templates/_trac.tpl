<!DOCTYPE html>
<html lang="en">
<head>
	<title>tracker :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		{@render trac_inline_style.css}
		table.trac td {
			text-align: center;
		}
		table.trac td:last-child {
			text-align: left;
		}
		table.trac td:first-child {
			white-space: nowrap;
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<main>
		<h2 id="main">Tracker</h2>
		<p>This is the place where bugs and todo's are tracked. Currently this tracks <strong>{@unsafe $totalrows}</strong> tickets, whereof <strong>{@unsafe $unresolvedcount}</strong> unresolved.</p>
		<p>Everyone can make a bug report: <a href="tracnew.php">report a bug</a>. <span style="background:#f00">TODO </span>To make a suggestion, head over to the message board.</p>
		<p>
			<a href="tracversion.php">[Change log]</a>
			{@if group_is_owner($usergroups)}
				&#32;<a href="tracversion.php?releaseconfirm">[Owner: release]</a>
			{@endif}
		</p>
		{@unsafe $pagination = simple_pagination('trac.php?page=', $page, $totalrows, 100)}
		<table class="trac">
			<thead>
				<tr>
					<th width=1%>Last update
					<th width=1%>Impact
					<th width=1%>Status
					<th>Summary
			</thead>
			<tbody>
				{@foreach $tracthreads as $t}
					<tr class="state{@unsafe $t->state}">
						<td>{@unsafe format_datetime($t->updated)}</td>
						<td class="severity{@unsafe $t->severity}">{@unsafe $trac_severities[$t->severity]}</td>
						<td>{@unsafe $trac_states[$t->state]}</td>
						<td><a href="tracview.php?id={@unsafe $t->id}#u{@unsafe $t->updated}">{$t->summary}</a>
					</tr>
				{@endforeach}
			</tbody>
		</table>
		{@unsafe $pagination}
		<table class="trac">
			<tbody>
				<tr>
					{@foreach $trac_states as $n => $s}
						<td class="state{@unsafe $n}" style="width:{@unsafe 100/count($trac_states)}%">{@unsafe $s}</td>
					{@endforeach}
				</tr>
			</tbody>
		</table>
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
