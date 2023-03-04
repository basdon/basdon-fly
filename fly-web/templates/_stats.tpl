<!DOCTYPE html>
<html lang=en>
<head>
	<title>stats :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		.floatingbox {
			float: left;
			background: #f8f8f8;
			border: 1px solid #aab;
			border-radius: 3px;
			margin-right: 2em;
			margin-bottom: 2em;
		}
		table.extrapadding th ,
		table.extrapadding td {
			padding: .2em 1em;
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<main>
		<h2 id=main>Stats</h2>
		<p>This page needs more content :)</p>
		{@if $weather_totalcount > 0}
			<h3>Weather</h3>
			<div class=floatingbox>
				<h4 class=center>Recent weather</h4>
				<table class="rowseps extrapadding">
					<thead>
						<tr><th>Weather<th>When
					<tbody>
						{@foreach $last_weathers as $w}
							{@eval $icon = $weather_iconnames[$weather_categorymapping[$w->w]]}
							<tr>
								<td>
									<img src=/s/weather_{@unsafe $icon}.png aria-hidden=true alt=""> 
									{@unsafe $weather_categorynames[$weather_categorymapping[$w->w]]}
								<td>
									{@unsafe format_duration_short(time() - $w->t)} ago
						{@endforeach}
				</table>
			</div>
			<div class=floatingbox>
				<h4 class=center>All time stats</h4>
				<table class="rowseps extrapadding">
					<thead>
						<tr><th>Weather<th>Percentage
					<tbody>
						{@foreach $weather_percentage_per_category as $cat => $value}
							{@foreach $weather_percentage_per_weather as $weather => $val}
								{@if $cat == $weather_categorymapping[$weather]}
									<tr>
										<td style=text-indent:20px>
											{@unsafe $weather_names[$weather]}
										<td>
											{@unsafe round(100 * $val / $weather_totalcount, 2)}%
								{@endif}
							{@endforeach}
							<tr>
								<td colspan=2 style=padding:0>
							<tr>
								<td>
									{@eval $icon = $weather_iconnames[$cat]}
									<img src=/s/weather_{@unsafe $icon}.png aria-hidden=true alt=""> 
									<strong>{@unsafe $weather_categorynames[$cat]}</strong>
								<td>
									<strong>{@unsafe round(100 * $value / $weather_totalcount, 2)}%</strong>
							<tr>
								<td colspan=2 style=padding:0>
						{@endforeach}
				</table>
			</div>
			<div class=clear></div>
		{@endif}
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
