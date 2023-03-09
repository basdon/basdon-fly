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
		<h3>Weather</h3>
		<div class=floatingbox>
			<h4 class=center>Recent weather</h4>
			<table class="rowseps extrapadding">
				<thead>
					<tr><th>Weather<th>When
				<tbody>
					{@foreach stats_query_last_weathers() as $w}
						<tr>
							<td>
								<img src=/s/weather_{@unsafe $w->icon}.png aria-hidden=true alt=""> 
								{@unsafe $w->name}
							<td>
								{@unsafe format_duration_short($w->when)} ago
					{@endforeach}
			</table>
			{@if !is_null(DBQ::$last_error)}
				<p class="msg error hmargin">
					{@escape DBQ::$last_error}
			{@endif}
		</div>
		<div class=floatingbox>
			<h4 class=center>All time stats</h4>
			<table class="rowseps extrapadding">
				<thead>
					<tr><th>Weather<th>Percentage
				<tbody>
					{@foreach stats_query_weather_percentages() as $category}
						<tr>
							<td colspan=2 style=padding:0>
						<tr>
							<td>
								<img src=/s/weather_{@unsafe $category->icon}.png aria-hidden=true alt=""> 
								<strong>{@unsafe $category->name}</strong>
							<td>
								<strong>{@unsafe $category->percentage}%</strong>
						<tr>
							<td colspan=2 style=padding:0>
						{@foreach $category->entries as $weather}
							<tr>
								<td style=text-indent:20px>
									{@unsafe $weather->name}
								<td>
									{@unsafe $weather->percentage}%
						{@endforeach}
					{@endforeach}
			</table>
			{@if !is_null(DBQ::$last_error)}
				<p class="msg error hmargin">
					{@escape DBQ::$last_error}
			{@endif}
		</div>
		<div class=clear></div>
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
