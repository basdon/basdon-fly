<!DOCTYPE html>
<html lang="en">
<head>
	<title>stats :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		<h2 id="main">Stats</h2>
		<p>This page needs more content :)</p>
		{@if $weather_totalcount > 0}
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
		{@endif}
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
