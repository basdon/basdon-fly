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
		table.trac input:not([type=submit]), table.trac textarea, table.trac td:last-child {
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
	<div class="singlebody">
		<p><a href="trac.php">Tracker</a> &gt; New ticket</p>
		<h2 id="main">New tracker ticket</h2>
		<form method="post" action="tracnew.php">
			{@input HAARP}
			<table class="trac">
				<tr><td>Summary</td><td>{@input text summary maxlength="80"}</td></tr>
				<tr><td>Description</td><td>{@input area description maxlength="4096"}</td></tr>
				<tr><td></td><td>{@input submit Submit}</td></tr>
			</table>
		</form>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
