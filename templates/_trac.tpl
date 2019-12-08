<!DOCTYPE html>
<html lang="en">
<head>
	<title>tracker :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		table.trac {
			width: 100%;
		}
		table.trac th {
			background: #c8c8e8;
		}
		table.trac td {
			padding: .1em .5em;
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
		{@unsafe $pagination = simple_pagination('trac.php?page=', $page, $totalrows, 50)}
		<table class="trac">
			<thead>
				<tr>
					<th>Last update</th><th>Impact</th><th>Status</th><th>Summary</th>
				</tr>
			</thead>
			<tbody>
				{@foreach $tracthreads as $t}
					<tr>
						<td>{@unsafe format_time_since($t->updated)}</td>
						<td>{$t->severity}</td>
						<td class="status{@unsafe $t->status}">{$t->status}</td>
						<td><a href="tracview.php?id={@unsafe $t->id}">{$t->summary}</a></td>
					<tr>
				{@endforeach}
			</tbody>
		</table>
		{@unsafe $pagination}
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
