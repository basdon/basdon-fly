<!DOCTYPE html>
<html lang="en">
<head>
	<title>flight #{@unsafe $id} details :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		#paytable td + td{
			border-left:1px solid #ccc;
			font-family: 'Courier New',monospace;
		}
		#rm {
			display:none
		}
		.rmi {
			float: right;
			width: 24%;
			padding-left: 1%
		}
		#rmm {
			float: left;
			width: 49%
		}
		canvas {
			width: 100%
		}
		@media (max-width: 800px) {
			#rmm {
				float: none;
				width: 100%
			}
			.rmi {
				width: 49%
			}
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		<p><a href="flights.php{@if $returnpage != -1}?page={@unsafe $returnpage}{@endif}">Flights</a> &gt; Flight #{@unsafe $id}</p>
		<h2 id="main">Flight #{@unsafe $id} details</h2>
<?php 
try{
	++$db_querycount;
	$r = $db->query('SELECT _f.*,_u.name,_u.i,_a.n fromname,_a.c fromcode,_b.n toname,_b.c tocode,_v.m vehmodel ,_m.name fromgate,_n.name togate,_o.name ownername 
	                 FROM flg _f 
	                 JOIN usr _u ON _f.player=_u.i 
	                 JOIN apt _a ON _f.fapt=_a.i 
	                 JOIN apt _b ON _f.tapt=_b.i 
	                 JOIN veh _v ON _f.vehicle=_v.i 
	                 JOIN msp _m ON _f.fmsp = _m.i 
	                 JOIN msp _n ON _f.fmsp = _n.i 
	                 LEFT OUTER JOIN usr _o ON _v.ownerplayer=_o.i 
	                 WHERE id=' . $id);
	if ($r === false || ($r = $r->fetchAll()) === false || empty($r)) {
		echo '<p>Flight not found (or something went wrong)!</p>';
	} else {
		$r = $r[0];
?>
		{@render flightstatuses.php}
		{@render aircraftnames.php}
		{@render missiontypes.php}
		<div class="fl49">
			<h3>Overview</h3>
			<ul>
				<li><strong>Status:</strong> <span class="flight-state-{@unsafe $r->state}" style="padding:.1em .4em;border-radius:3px">{@unsafe fmt_flight_status($r->state, $r->tload)}</span></li>
				<li><strong>Pilot:</strong> {@unsafe linkuser($r)}</li>
				<li><strong>Origin:</strong> <a href="airport.php?code={@unsafe $r->fromcode}">{@unsafe $r->fromname} ({@unsafe $r->fromcode})</a> {@unsafe $r->fromgate}</li>
				<li><strong>Destination:</strong> <a href="airport.php?code={@unsafe $r->tocode}">{@unsafe $r->toname} ({@unsafe $r->tocode})</a> {@unsafe $r->togate}</li>
				<li><strong>Point-to-point distance:</strong> {@unsafe $r->distance}m</li>
				<li><strong>Actual flown distance:</strong> {@unsafe $r->adistance}m</li>
				<li><strong>Flight start:</strong> {@unsafe format_datetime($r->tstart)}</li>
				<li><strong>Flight end:</strong> {@if $r->state != 1}{@unsafe format_datetime($r->tlastupdate)}{@endif}</li>
				<li><strong>Flight duration:</strong> {@if $r->state != 1}{@unsafe sprintf('%02d:%02d', floor(($diff=$r->tlastupdate-$r->tstart)/60), $diff%60)}{@endif}</li>
				<li><strong>Aircraft:</strong> {@unsafe aircraft_name($r->vehmodel)} (owned by: {@unsafe aircraftowner($r->ownername)})</li>
				<li><strong>Flight type:</strong> {@unsafe fmt_mission_type($r->missiontype)}</li>
				{@if $r->missiontype & $passenger_mission_types}
					<li><strong>Passenger satisfaction:</strong> {@unsafe $r->satisfaction}%</li>
				{@endif}
				{@if $r->state == 8}
					<li><strong>Fuel burned:</strong> {@unsafe round($r->fuel, 2)}L</li>
					<li><strong>Damage taken:</strong> {@unsafe $r->damage}</li>
				{@endif}
			</ul>
			<h3>Detailed timing</h3>
			<ul>
				<li><strong>{@unsafe date('H:i:s', $r->tstart)}</strong> flight started</li>
				{@if $r->tload > 0}
					<li><strong>{@unsafe date('H:i:s', $r->tload)}</strong> cargo loaded</li>
				{@endif}
				<li id="rmt" style="display:none"></li>
				{@if $r->tunload > 0}
					<li><strong>{@unsafe date('H:i:s', $r->tunload)}</strong> cargo unloaded, flight ended</li>
				{@endif}
				{@if $r->state != 1 && $r->state != 8}
					<li><strong>{@unsafe date('H:i:s', $r->tlastupdate)}</strong> <span style="color:#c22">flight ended without finishing</span></li>
				{@endif}
			</ul>
		</div>
		<div class="fr49">
			<h3>Payment breakdown</h3>
			{@if $r->state != 8}
				<p>Flight is not finished, no payment information available.</p>
			{@else}
				<table id="paytable" class="default" style="border:1px solid #ccc;border-bottom:none;text-align:right">
					{@render fmt_money.php}
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
						<td style="border-top:1px solid #ccc"><strong>Total</strong></td>
						<td style="border-top:1px solid #ccc"><strong>{@unsafe fmt_money($r->ptotal)}</strong></td>
					</tr>
				</table>
			{@endif}
		</div>
		<div class="clear"></div>
<?php 
	}
} catch (PDOException $e) {
?>
		<p style="color:red;">Failed to load flight info!</p>
<?php } ?>
		<h3>Flight map/info</h3>
		<noscript><p>Enable JavaScript to see the detailed flight map/info.</p></noscript>
		<div id="rmmsg">Loading...</div>
		<div id="rm">
			<div id="rmm">
				<h4>Map</h4>
				<canvas id="crmm" width="1000" height="1000"></canvas>
				<p style="font-size:.8em"><a href="https://forum.sa-mp.com/showthread.php?t=564899">San Andreas Vector Map</a> made by Blantas</p>
			</div>
			<div class="rmi">
				<h4>Speed</h4>
				<canvas id="crms" width="500" height="200"></canvas>
			</div>
			<div class="rmi">
				<h4>Altitude</h4>
				<canvas id="crma" width="500" height="200"></canvas>
			</div>
			<div class="rmi">
				<h4>Aircraft health</h4>
				<canvas id="crmh" width="500" height="200"></canvas>
			</div>
			<div class="rmi">
				<h4>Passenger satisfaction</h4>
				<canvas id="crmp" width="500" height="200"></canvas>
			</div>
			<div class="rmi">
				<h4>Fuel</h4>
				<canvas id="crmf" width="500" height="200"></canvas>
			</div>
			<div class="clear"></div>
		</div>
		<script src="{@unsafe $STATICPATH}/flightmap.js"></script>
		<script>UTCoffset={@unsafe date('Z')};flightmap('{@unsafe $STATICPATH}', {@unsafe $id})</script>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
