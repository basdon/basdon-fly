<!DOCTYPE html>
<html lang="en">
<head>
	<title>stats :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<main>
		<h2 id="main">Stats</h2>
		<p>This page needs more content :)</p>
		{@if $weather_totalcount > 0}
			<h3>Weather</h3>
			<h4>Recent weather</h4>
			<table class="new rowseps">
				<thead>
					<tr><th>Weather</th><th>When</th><th>Duration</th></tr>
				</thead>
				<tbody>
					{@eval $last_time = 0}
					{@foreach $last_weathers as $w}
						<tr>
							<td>{@unsafe $weather_categorynames[$weather_categorymapping[$w->w]]}</td>
							<td>{@unsafe format_duration_short(time() - $w->t)} ago</td>
							{@if $last_time}
								<td>{@unsafe format_duration_short($last_time - $w->t)}</td>
							{@else}
								<td>{@unsafe format_duration_short(time() - $w->t)} and counting</td>
							{@endif}
							{@eval $last_time = $w->t}
						</tr>
					{@endforeach}
				</tbody>
			</table>
			<h4>All time stats</h4>
			<table class="new rowseps">
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
			<table class="new rowseps">
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
			{@eval unset($weather_categorymapping)}
			{@eval unset($weather_categorynames)}
			{@eval unset($weather_names)}
		{@endif}
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
