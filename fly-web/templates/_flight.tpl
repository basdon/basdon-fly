<!DOCTYPE html>
<html lang="en">
<head>
	<title>flight #{@unsafe $id} details :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		#paytable td + td {
			border-left: 1px solid #aab;
			font-family: 'Courier New',monospace;
		}
		#fm_mapcanvas {
			max-width:900px;
			margin:1em auto;
			display:none
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<main>
		<p><a href="flights.php{@if $returnpage != -1}?page={@unsafe $returnpage}{@endif}">Flights</a> &gt; Flight #{@unsafe $id}</p>
		<h2 id="main">Flight #{@unsafe $id} details</h2>
	{@if !isset($r)}
		<p>Flight not found (or something went wrong)!</p>
	{@elseif $r == false}
		<p style="color:#c22;">Failed to load flight info!</p>
	{@else}
		<div class="fl49">
			<h3>Overview</h3>
			<ul>
				<li><strong>Status:</strong> <span class="flight-state f{@unsafe $r->state}">{@unsafe fmt_flight_status($r->state, $r->tload)}</span></li>
				<li><strong>Pilot:</strong> {@unsafe linkuser($r)}</li>
				<li><strong>Origin:</strong> <a href="article.php?title={@unsafe $r->fromcode}">{@unsafe $r->fromname} ({@unsafe $r->fromcode})</a> {@unsafe $r->fromgate}</li>
				<li><strong>Destination:</strong> <a href="article.php?title={@unsafe $r->tocode}">{@unsafe $r->toname} ({@unsafe $r->tocode})</a> {@unsafe $r->togate}</li>
				<li><strong>Point-to-point distance:</strong> {@unsafe round($r->distance, 2)}m</li>
				<li><strong>Actual flown distance:</strong>{@if $r->state != 1} {@unsafe round($r->adistance, 2)}m{@endif}</li>
				<li><strong>Flight start:</strong> {@unsafe format_datetime($r->tstart)}</li>
				<li><strong>Flight end:</strong> {@if $r->is_finished}{@unsafe format_datetime($r->tlastupdate)}{@endif}</li>
				<li><strong>Flight duration:</strong> {@if $r->is_finished}{@unsafe sprintf('%02d:%02d', floor($r->duration/60), $r->duration%60)}{@endif}</li>
				<li><strong>Aircraft:</strong> {@unsafe aircraft_name($r->vehmodel)} (owned by: {@unsafe aircraftowner($r->ownername)})</li>
				<li><strong>Flight type:</strong> {@unsafe fmt_mission_type($r->missiontype)}</li>
				{@if $r->missiontype & $SETTING__PASSENGER_MISSIONTYPES}
					<li><strong>Passenger satisfaction:</strong> {@unsafe $r->satisfaction}%</li>
				{@endif}
				<li><strong>Fuel burned:</strong>{@if $r->state != 1} {@unsafe round($r->fuel * vehicle_fuel_cap($r->vehmodel), 2)}L{@endif}</li>
				<li><strong>Damage taken:</strong>{@if $r->state != 1} {@unsafe $r->damage}{@endif}</li>
			</ul>
			<h3>Detailed timing</h3>
			<ul>
				<li><strong>{@unsafe date('H:i:s', $r->tstart)}</strong> flight started</li>
				{@if $r->tload > 0}
					<li><strong>{@unsafe date('H:i:s', $r->tload)}</strong> cargo loaded</li>
				{@endif}
				<li id="fm_flightevents" style="display:none"></li>
				{@if $r->tunload > 0}
					<li><strong>{@unsafe date('H:i:s', $r->tunload)}</strong> cargo unloaded, flight ended</li>
				{@endif}
				{@if $r->state != 1 && $r->state != 8 && $r->state != 2048}
					<li><strong>{@unsafe date('H:i:s', $r->tlastupdate)}</strong> <span style="color:#c22">flight ended without finishing</span></li>
				{@endif}
			</ul>
		</div>
		<div class="fr49">
			<h3>Payment breakdown</h3>
			{@if $r->state != 8}
				<p>Flight is not finished, no payment information available.</p>
			{@else}
				<table id="paytable" class="new" style="text-align:right">
					<thead>
						<tr><th>Category</th><th>Amount</th></tr>
					</thead>
					<tbody>
						{@if $r->ptax}<tr><td>Airport tax</td><td>{@unsafe fmt_money($r->ptax)}</tr>{@endif}
						{@if $r->pweatherbonus}<tr><td>Weather bonus</td><td>{@unsafe fmt_money($r->pweatherbonus)}</tr>{@endif}
						{@if $r->pdistance}<tr><td>Distance pay</td><td>{@unsafe fmt_money($r->pdistance)}</tr>{@endif}
						{@if $r->psatisfaction > 0}
							<tr><td>Satisfaction bonus</td><td>{@unsafe fmt_money($r->psatisfaction)}</tr>
						{@elseif $r->psatisfaction < 0}
							<tr><td>Satisfaction penalty</td><td>{@unsafe fmt_money($r->psatisfaction)}</tr>
						{@endif}
						{@if $r->pdamage}<tr><td>Damage penalty</td><td>{@unsafe fmt_money($r->pdamage)}</tr>{@endif}
						{@if $r->pcheat}<tr><td>Cheat penalty</td><td>{@unsafe fmt_money($r->pcheat)}</tr>{@endif}
						{@if $r->pbonus}<tr><td>Bonus</td><td>{@unsafe fmt_money($r->pbonus)}</tr>{@endif}
						<tr>
							<td style="border-top:1px solid #aab"><strong>Total</strong></td>
							<td style="border-top:1px solid #aab"><strong>{@unsafe fmt_money($r->ptotal)}</strong></td>
						</tr>
					</tbody>
				</table>
			{@endif}
		</div>
		<div class="clear"></div>
		<h3>Flight map/info</h3>
		<noscript><p>Enable JavaScript to see the detailed flight map/info.</p></noscript>
		<div id="fm_mapmsg"><p>Loading...</p></div>
		<div id="fm_mapcanvas"><canvas id="fm_canvas" width="900" height="600" style="width:100%;border:1px solid"></canvas></div>
		<script src="/s/gen/fm_complete.js?v17"></script>
		<script>UTCoffset={@unsafe date('Z')};flightmap({@unsafe $id})</script>
	{@endif}
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
