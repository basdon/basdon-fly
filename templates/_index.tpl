<!DOCTYPE html>
<html lang="en">
<head>
	<title>home :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
	.col {
		width: 48%;
	}
	.left {
		float: left;
	}
	@media (max-width: 800px) {
		float: none;
	}
	h3 {
		margin-bottom: 0;
		text-align: center;
		background: #8484f9;
		border-bottom: 0;
		border: 1px solid #000;
		border-bottom: 0;
		font-size: 1.3em;
	}
	table {
		border: 1px solid #000;
		border-top: 0;
		border-collapse: collapse;
	}
	table th {
		background: #8484f9;
	}
	td, th {
		padding: .2em .5em;
	}
	td + td {
		border-left: 1px solid #000;
	}
	thead tr {
		border-bottom: 1px solid #000;
	}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">

		<h2 id="main">Welcome!</h2>
		<p>
			basdon.net aviation server is a server for <a href="https://www.sa-mp.com/">San Andreas Multiplayer (SA-MP)</a>, 
			a multiplayer modification for the game <a href="http://www.rockstargames.com/sanandreas/">Grand Theft Auto: San Andreas</a>. 
			The main activity is to grab a plane or helicopter and enjoy flying around, either freely or by accepting missions towards various destinations. 
			Start your flight and have fun!
		</p>

		<h3>Online players</h3>
		<div class="fr49">
			<img style="width:100%;max-width:480px;" src="{@unsafe $STATICPATH}/gen/playergraph.png" />
		</div>
		<div class="fl49">
			{@foreach $onlineplayers as $p}
				{@unsafe linkuser($p)}&nbsp;({@unsafe $p->score})
			{@endforeach}
		</div>
		<div class="clear"></div>

		{@render flightstatuses.php}
		{@render aircraftnames.php}

		<h3>Active flights</h3>
		<table border="0" width="100%" class="center">
			<thead>
				<tr>
					<th>No</th>
					<th>Time since start</th>
					<th>Pilot</th>
					<th>Aircraft</th>
					<th>Departure</th>
					<th>Arrival</th>
					<th>Status</th>
				</tr>
			</thead>
			<tbody>
				{@if $activeflights !== false && ($activeflights = $activeflights->fetchAll()) && count($activeflights)}
					{@foreach $activeflights as $f}
						<tr>
							<td><a href="flight.php?id={@unsafe $f->id}">#{@unsafe $f->id}</a></td>
							<td>{@unsafe format_duration_short(time() - $f->tstart)}</td>
							<td>{@unsafe linkuser($f)}</td>
							<td>{@unsafe aircraft_name($f->vehmodel)}</td>
							<td><a href="airport.php?code={@unsafe $f->f}">{@unsafe $f->f}</a></td>
							<td><a href="airport.php?code={@unsafe $f->t}">{@unsafe $f->t}</a></td>
							<td class="flight-state flight-state-{@unsafe $f->state}">{@unsafe fmt_flight_status($f->state, $f->tload)}</td>
						</tr>
					{@endforeach}
				{@else}
					<tr><td colspan="7">None</td></tr>
				{@endif}
			</tbody>
		</table>

		<h3>Last 10 finished flights</h3>
		<table border="0" width="100%" class="center">
			<thead>
				<tr>
					<th>No</th>
					<th>Date</th>
					<th>Pilot</th>
					<th>Aircraft</th>
					<th>Departure</th>
					<th>Arrival</th>
					<th>Distance</th>
					<th>Status</th>
				</tr>
			</thead>
			<tbody>
				{@if $lastflights !== false && ($lastflights = $lastflights->fetchAll()) && count($lastflights)}
					{@foreach $lastflights as $f}
						<tr>
							<td><a href="flight.php?id={@unsafe $f->id}">#{@unsafe $f->id}</a></td>
							<td>{@unsafe date('j M H:i', $f->tlastupdate)}</td>
							<td>{@unsafe linkuser($f)}</td>
							<td>{@unsafe aircraft_name($f->vehmodel)}</td>
							<td><a href="airport.php?code={@unsafe $f->f}">{@unsafe $f->f}</a></td>
							<td><a href="airport.php?code={@unsafe $f->t}">{@unsafe $f->t}</a></td>
							<td>{@if $f->state != 1}{@unsafe round($f->adistance)}m{@endif}</td>
							<td class="flight-state flight-state-{@unsafe $f->state}">{@unsafe fmt_flight_status($f->state, $f->tload)}</td>
						</tr>
					{@endforeach}
				{@else}
					<tr><td colspan="7">None</td></tr>
				{@endif}
			</tbody>
		</table>

		<p class="center"><a href="flights.php">All flights</a></p>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
