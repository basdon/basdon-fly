<!DOCTYPE html>
<html lang="en">
<head>
	<title>tracker :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		{@render trac_inline_style.css}
		table.trac th {
			background: #c8c8e8;
		}
		table.trac td {
			text-align: center;
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		<h2 id="main">Tracker</h2>
		<p>This is the place where bugs and todo's are tracked.</p>
		<p>Everyone can make a bug report: <a href="tracnew.php">report a bug</a>. <span style="background:#f00">TODO </span>To make a suggestion, head over to the message board.</p>
		{@unsafe $pagination = simple_pagination('trac.php?page=', $page, $totalrows, 100)}
		<table class="trac">
			<thead>
				<tr>
					<th>Last update</th><th>Impact</th><th>Status</th><th>Summary</th>
				</tr>
			</thead>
			<tbody>
				{@foreach $tracthreads as $t}
					<tr class="status{@unsafe $t->status}">
						<td>{@unsafe format_time_since($t->updated)}</td>
						<td class="severity{@unsafe $t->severity}">{@unsafe $trac_severities[$t->severity]}</td>
						<td>{@unsafe $trac_statuses[$t->status]}</td>
						<td style="text-align:left;"><a href="tracview.php?id={@unsafe $t->id}">{$t->summary}</a></td>
					</tr>
				{@endforeach}
			</tbody>
		</table>
		{@unsafe $pagination}
		<table class="trac">
			<tbody>
				<tr>
					{@foreach $trac_statuses as $n => $s}
						<td class="status{@unsafe $n}" style="width:{@unsafe 100/count($trac_statuses)}%">{@unsafe $s}</td>
					{@endforeach}
				</tr>
			</tbody>
		</table>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
