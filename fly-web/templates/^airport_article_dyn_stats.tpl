{@render aircraftnames.php}
{@render fmt_money.php}
{@render missiontypes.php}

{@rem clear since this is rendered in an article, and the factsheet might still be floated right @}
<div class="clear"></div>

<h3>Mission points</h3>

{@if $missionpoints}
	<table class="new rowseps">
		<thead>
			<tr><th>Name</th><th><a href="article.php?title=Mission_point_types">Available mission types</a></th></tr>
		</thead>
		<tbody>
			{@foreach $missionpoints as $msp}
			<tr>
				<td>{$msp->name}</td>
				<td>
					<ul class="compact">
						{@for $i = 0; $i < 31; $i++}
							{@if (1 << $i) & $msp->t}
								<li>
									<a href="article.php?title=Mission_point_types#mpta">{@unsafe fmt_mission_type(1 << $i)}</a>
								</li>
							{@endif}
						{@endfor}
					</ul>
				</td>
			</tr>
			{@endif}
		</tbody>
	</table>
{@else}
	<p>Failed to retrieve data.</p>
{@endif}

<h3>Latest flights</h3>

{@rem TODO too much margin in between here (but only when page is wide enough, it's okay when it's not wide and under each other @}

<div class="fl49">
	<h4>Last 5 inbound flights</h4>
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
						<td><a href="article.php?title={@unsafe $i->fromcode}">{@unsafe $i->fromcode}</a></td>
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
	<h4>Last 5 outbound flights</h4>
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
						<td><a href="article.php?title={@unsafe $i->tocode}">{@unsafe $i->tocode}</a></td>
					</tr>
				{@endforeach}
			{@else}
				<tr><td colspan="5">None!</td></tr>
			{@endif}
		</tbody>
	</table>
</div>

<div class="clear"></div>


<h3>Miscellaneous stats</h3>

<ul>
	<li><strong>Mission flights:</strong> {@unsafe $stat_num_inbound} inbound + {@unsafe $stat_num_outbound} outbound (={@unsafe $stat_num_inbound + $stat_num_outbound})</li>
	{@rem using round() for int values as well, because they can be NULL when no svps at airport (yukkie) @}
	<li><strong>Service points refuels:</strong> {@unsafe $stat_refuels}</li>
	<li><strong>Service points refuel spending:</strong> {@unsafe fmt_money(-$stat_refuel_spending)}</li>
	<li><strong>Service points repairs:</strong> {@unsafe $stat_repairs}</li>
	<li><strong>Service points repair spending:</strong> {@unsafe fmt_money(-$stat_repair_spending)}</li>
	<li><strong>Service points damage repaired:</strong> {@unsafe round($stat_dmg_repaired, 0)}</li>
</ul>
