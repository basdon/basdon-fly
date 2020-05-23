<!DOCTYPE html>
<html lang="en">
<head>
	<title>{$apt->n} :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	{@render aircraftnames.php}
	{@render fmt_money.php}
	<div class="singlebody">
		<h2>{$apt->n}</h2>

		<p>For for information, check out the <a href="article.php?title={@unsafe $apt->c}">article: {$apt->n}</a></p>

		<div class="fl49">
			<h3 class="newtableheader">Last inbound flights</h3>
			<table class="new center" width="100%">
				<thead>
					<tr><th>Flight</th><th>Date</th><th>Aircraft</th><th>From</th><th>To</th></tr>
				</thead>
				<tbody>
					{@if count($inbounds)}
						{@foreach $inbounds as $i}
							<tr>
								<td><a href="flight.php?id={@unsafe $i->id}">Flight #{@unsafe $i->id}</a></td>
								<td>{@unsafe date('j M H:i', $i->tunload)}</td>
								<td>{@unsafe aircraft_name($i->vehmodel)}</td>
								<td><a href="locations.php?code={@unsafe $i->fromcode}">{@unsafe $i->fromcode}</a></td>
								<td>{@unsafe $i->missionpointname}</td>
							</tr>
						{@endforeach}
					{@else}
						<tr><td colspan="5">None!</td></tr>
					{@endif}
				</tbody>
			</table>
		</div>
		<div class="fr49">
			<h3 class="newtableheader">Last outbound flights</h3>
			<table class="new center" width="100%">
				<thead>
					<tr><th>Flight</th><th>Date</th><th>Aircraft</th><th>From</th><th>To</th></tr>
				</thead>
				<tbody>
					{@if count($outbounds)}
						{@foreach $outbounds as $i}
							<tr>
								<td><a href="flight.php?id={@unsafe $i->id}">Flight #{@unsafe $i->id}</a></td>
								<td>{@unsafe date('j M H:i', $i->tload)}</td>
								<td>{@unsafe aircraft_name($i->vehmodel)}</td>
								<td>{@unsafe $i->missionpointname}</td>
								<td><a href="locations.php?code={@unsafe $i->tocode}">{@unsafe $i->tocode}</a></td>
							</tr>
						{@endforeach}
					{@else}
						<tr><td colspan="5">None!</td></tr>
					{@endif}
				</tbody>
			</table>
		</div>

		<div class="clear"></div>

		<h3>Stats</h3>
		<ul>
			<li><strong>Mission flights:</strong> {@unsafe $stat_num_inbound} inbound + {@unsafe $stat_num_outbound} outbound (={@unsafe $stat_num_inbound + $stat_num_outbound})</li>
			{@rem using round() for int values as well, because they can be NULL when no svps at airport (yukkie) @}
			<li><strong>Service points refuels:</strong> {@unsafe $stat_refuels}</li>
			<li><strong>Service points refuel spending:</strong> {@unsafe fmt_money(-$stat_refuel_spending)}</li>
			<li><strong>Service points fuel loaded:</strong> {@unsafe round($stat_fuel_loaded, 1)}L</li>
			<li><strong>Service points repairs:</strong> {@unsafe $stat_repairs}</li>
			<li><strong>Service points repair spending:</strong> {@unsafe fmt_money(-$stat_repair_spending)}</li>
			<li><strong>Service points damage repaired:</strong> {@unsafe round($stat_dmg_repaired, 0)}</li>
		</ul>

		<h3>Map</h3>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
