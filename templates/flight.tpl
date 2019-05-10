<!DOCTYPE html>
<html lang="en">
<head>
	<title>basdon.net aviation server :: flight #{@unsafe $id} details</title>
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
	$r = $db->query('SELECT _f.*,_u.n playername,_a.n fromname,_a.c fromcode,_b.n toname,_b.c tocode,_v.m vehmodel 
	                 FROM flg _f 
	                 JOIN usr _u ON _f.player=_u.i 
	                 JOIN apt _a ON _f.fapt=_a.i 
	                 JOIN apt _b ON _f.tapt=_b.i 
	                 JOIN veh _v ON _f.vehicle=_v.i 
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
				<li><strong>Status:</strong> <span class="flight-state-{@unsafe $r->state}" style="padding:.1em .4em">{@unsafe fmt_flight_status($r->state, $r->tload)}</span></li>
				<li><strong>Pilot:</strong> <a href="user.php?name={@urlencode $r->playername}">{@unsafe $r->playername}</a></li>
				<li><strong>Departure Airport:</strong> <a href="airport.php?code={@unsafe $r->fromcode}">{@unsafe $r->fromname} ({@unsafe $r->fromcode})</a></li>
				<li><strong>Arrival Airport:</strong> <a href="airport.php?code={@unsafe $r->tocode}">{@unsafe $r->toname} ({@unsafe $r->tocode})</a></li>
				<li><strong>Point-to-point distance:</strong> {@unsafe $r->distance}m</li>
				<li><strong>Actual flown distance:</strong> {@unsafe $r->adistance}m</li>
				<li><strong>Flight start:</strong> {@unsafe date('j M Y H:i', $r->tstart)} (GMT{@unsafe date('O')})</li>
				<li><strong>Flight end:</strong> {@if $r->state != 1}{@unsafe date('j M Y H:i', $r->tlastupdate)} (GMT{@unsafe date('O')}){@endif}</li>
				<li><strong>Flight duration:</strong> {@if $r->state != 1}{@unsafe sprintf('%02d:%02d', floor(($diff=$r->tlastupdate-$r->tstart)/60), $diff%60)}{@endif}</li>
				<li><strong>Aircraft:</strong> {@unsafe aircraft_name($r->vehmodel)}</li>
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
				<table id="paytable" style="border:1px solid #ccc;border-bottom:none;text-align:right">
<?php 
function fmt_pay($amount)
{
	$s = '<span style="color:#';
	if ($amount < 0) {
		$s .= 'c22">-$';
		$amount = -$amount;
	} else {
		$s .= '2a2">$';
	}
	$amount = "$amount";
	$l = $len = strlen($amount);
	while ($l--) {
		$s .= $amount[$len - $l - 1];
		if ($l && $l % 3 == 0) {
			$s .= ',';
		}
	}
	return $s . '</span>';
}
?>
					{@if $r->ptax}<tr><td>Airport tax</td><td>{@unsafe fmt_pay($r->ptax)}</tr>{@endif}
					{@if $r->pweatherbonus}<tr><td>Weather bonus</td><td>{@unsafe fmt_pay($r->pweatherbonus)}</tr>{@endif}
					{@if $r->pdistance}<tr><td>Distance pay</td><td>{@unsafe fmt_pay($r->pdistance)}</tr>{@endif}
					{@if $r->psatisfaction > 0}
						<tr><td>Satisfaction bonus</td><td>{@unsafe fmt_pay($r->psatisfaction)}</tr>
					{@elseif $r->psatisfaction < 0}
						<tr><td>Satisfaction penalty</td><td>{@unsafe fmt_pay($r->psatisfaction)}</tr>
					{@endif}
					{@if $r->pdamage}<tr><td>Damage penalty</td><td>{@unsafe fmt_pay($r->pdamage)}</tr>{@endif}
					{@if $r->pcheat}<tr><td>Cheat penalty</td><td>{@unsafe fmt_pay($r->pcheat)}</tr>{@endif}
					{@if $r->pbonus}<tr><td>Bonus</td><td>{@unsafe fmt_pay($r->pbonus)}</tr>{@endif}
					<tr>
						<td style="border-top:1px solid #ccc"><strong>Total</strong></td>
						<td style="border-top:1px solid #ccc"><strong>{@unsafe fmt_pay($r->ptotal)}</strong></td>
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
			<div class="clear"></div>
		</div>
		<script src="{@unsafe $STATICPATH}/flightmap.js"></script>
		<script>flightmap('{@unsafe $STATICPATH}', {@unsafe $id})</script>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
