<!DOCTYPE html>
<html lang="en">
<head>
	<title>stats :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div>
		<div id="main">
			<h2>Stats</h2>
			<h3>Weather</h3>
			<table>
				<thead>
					<tr><th>Weather category</th><th>Percentage</th></tr>
				</thead>
				<tbody>
					{@foreach $weather_percategory as $cat => $value}
						<tr>
							<td>{@unsafe $weather_categorynames[$cat]}</td>
							<td>{@unsafe round(100 * $value / $weather_totalcount, 2)}%</td>
						</tr>
					{@endforeach}
				</tbody>
			</table>
			{@eval unset($weather_categorynames)}
			{@eval unset($weather_percategory)}
			<table>
				<thead>
					<tr><th>Weather</th><th>Percentage</th></tr>
				</thead>
				<tbody>
					{@foreach $weather_all as $weather => $value}
						<tr>
							<td>{@unsafe $weather_names[$weather]}</td>
							<td>{@unsafe round(100 * $value / $weather_totalcount, 2)}%</td>
						</tr>
					{@endforeach}
				</tbody>
			</table>
			{@eval unset($weather_names)}
		</div>
		{@render aside.tpl}
		<div class="clear"></div>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>