<!DOCTYPE html>
<html lang="en">
<head>
	<title>home :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		#showcase{
			margin-top:1em;
			text-align:center;
		}
		#showcase a{
			display:inline-block;
			background:#f6f6ff;
			border:1px solid #b5cdff;
			font-size:.8em;
			padding:.5em;
			margin:.5em;
		}
		#showcase img{
			width: 175px;
		}
		@media(max-width:800px){
			#showcase img{
				width:100px;
			}
		}
		h3{
			border-bottom: 1px dotted #000;
			margin-top:1.5em;
		}
		.left{
			text-align:left;
		}
		h4+p{
			margin-top:0;
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div><div class="center">
		<h2 id="main" class="left">Welcome!</h2>
		<p class="left">
			basdon.net aviation server is a server for <a href="https://www.sa-mp.com/">San Andreas Multiplayer (SA-MP)</a>{@globe}, 
			a multiplayer modification for the game <a href="http://www.rockstargames.com/sanandreas/">Grand Theft Auto: San Andreas</a>{@globe}. 
			The main activity is to grab a plane or helicopter and enjoy flying around, either freely or by accepting missions towards various destinations. 
			Start your flight and have fun!
		</p>

		<p style="font-size:1.5em">
			basdon.net:7777
		</p>
		<p>
			<a href="https://files.sa-mp.com/sa-mp-0.3.7-R4-install.exe">SA-MP 0.3.7</a> - <a href="article.php?title=How_To_Join">How to join</a>
		</p>
		<p>
			Join our Discord: <a href="https://discordapp.com/invite/w4jFpqd" rel="nofollow">https://discordapp.com/invite/w4jFpqd</a>{@globe}
		</p>
		<p>
			IRC: #basdon (<a href="{@unsafe $STATICPATH}/chatlogs/basdon/?C=M;O=D">chatlogs</a>) and #basdon.echo (<a href="{@unsafe $STATICPATH}/chatlogs/basdon.echo/?C=M;O=D">chatlogs</a>) on LUNet (<a href="irc://irc.liberty-unleashed.co.uk">irc.liberty-unleashed.co.uk</a>{@globe})
		</p>

		<h3>Our destinations</h3>
		<div class="airportlist">
			<?php readfile('../gen/airportlist.html'); ?>
		</div>
		<h3>Nagivation features</h3>
		<div class="navinfo">
			<?php readfile('../gen/navinfotable.html'); ?>
		</div>
		<h3>Screenshots</h3>
		<p id="showcase">
			<a href="{@unsafe $STATICPATH}/showcase-dipa.jpg"><img src="{@unsafe $STATICPATH}/showcase-dipa.jpg" title="dipa taxiing"></a>
			<a href="{@unsafe $STATICPATH}/showcase-jobmap.jpg"><img src="{@unsafe $STATICPATH}/showcase-jobmap.jpg" title="mission map"></a>
			<a href="{@unsafe $STATICPATH}/showcase-spitfire2.png"><img src="{@unsafe $STATICPATH}/showcase-spitfire2.png" title="spitfire"></a>
			<a href="flight.php?id=2572"><img src="{@unsafe $STATICPATH}/showcase-map.png" title="flight map"></a>
		</p>

		<h3>Changelog</h3>
		<ul style="list-style:none;text-align:left;padding:0;display:table;margin:0 auto">
			{@foreach $last_changelog_entries as $change}
				<li><strong>{@unsafe $change->stamp}</strong>: {$change->entry}</li>
			{@endforeach}
		</ul>
		<p><a href="changelog.php">More...</a></p>

		<h3>Player activity</h3>
		<p><img style="width:100%" src="{@unsafe $STATICPATH}/gen/playergraph.png" /></p>

		<h4>Online now ({@unsafe count($onlineplayers)})</h4>
		<p>
			{@if count($onlineplayers)}
				{@foreach $onlineplayers as $p}
					{@unsafe linkuser($p)}&nbsp;({@unsafe $p->score})&#32;
				{@endforeach}
			{@else}
				None
			{@endif}
		</p>

		<h4>Online last 48h ({@unsafe count($last48)})</h4>
		<p>
			{@if count($last48)}
				{@foreach $last48 as $p}
					{@unsafe linkuser($p)}&nbsp;({@unsafe $p->score})&#32;
				{@endforeach}
			{@else}
				None
			{@endif}
		</p>

		{@render flightstatuses.php}
		{@render aircraftnames.php}

		<h3>Active flights</h3>
		{@if $activeflights !== false && ($activeflights = $activeflights->fetchAll()) && count($activeflights)}
			<table width="100%" class="new rowseps highlight-row">
				<thead>
					<tr>
						<th>#</th>
						<th class="no800">Time since start</th>
						<th>Pilot</th>
						<th>Aircraft</th>
						<th>From</th>
						<th>To</th>
						<th class="no600">Status</th>
					</tr>
				</thead>
				<tbody>
					{@foreach $activeflights as $f}
						<tr>
							<td><a href="flight.php?id={@unsafe $f->id}">#{@unsafe $f->id}</a></td>
							<td class="no800">{@unsafe format_duration_short(time() - $f->tstart)}</td>
							<td>{@unsafe linkuser($f)}</td>
							<td>{@unsafe aircraft_name($f->vehmodel)}</td>
							<td><a href="article.php?title={@unsafe $f->f}">{@unsafe $f->f}</a></td>
							<td><a href="article.php?title={@unsafe $f->t}">{@unsafe $f->t}</a></td>
							<td class="no600 flight-state flight-state-{@unsafe $f->state}">{@unsafe fmt_flight_status($f->state, $f->tload)}</td>
						</tr>
					{@endforeach}
				</tbody>
			</table>
		{@else}
			<p>None</p>
		{@endif}

		<h3>Last 10 finished flights</h3>
		{@if $lastflights !== false && ($lastflights = $lastflights->fetchAll()) && count($lastflights)}
			<table width="100%" class="new rowseps highlight-row">
				<thead>
					<tr>
						<th>#</th>
						<th class="no600">Date</th>
						<th>Pilot</th>
						<th>Aircraft</th>
						<th>From</th>
						<th>To</th>
						<th class="no800">Distance</th>
						<th class="no600">Status</th>
					</tr>
				</thead>
				<tbody>
					{@foreach $lastflights as $f}
						<tr>
							<td><a href="flight.php?id={@unsafe $f->id}">#{@unsafe $f->id}</a></td>
							<td class="no600">{@unsafe date('j M H:i', $f->tlastupdate)}</td>
							<td>{@unsafe linkuser($f)}</td>
							<td>{@unsafe aircraft_name($f->vehmodel)}</td>
							<td><a href="article.php?title={@unsafe $f->f}">{@unsafe $f->f}</a></td>
							<td><a href="article.php?title={@unsafe $f->t}">{@unsafe $f->t}</a></td>
							<td class="no800">{@if $f->state != 1}{@unsafe round($f->adistance)}m{@endif}</td>
							<td class="no600 flight-state flight-state-{@unsafe $f->state}">{@unsafe fmt_flight_status($f->state, $f->tload)}</td>
						</tr>
					{@endforeach}
				</tbody>
			</table>
		{@else}
			<p>None</p>
		{@endif}

		<p><a href="flights.php">All flights</a></p>

		<h3>Member stats</h3>
		<p>
			Members: {@unsafe $membercount->c} &bull; Guest sessions: {@unsafe $guestcount->c} &bull; Latest: {@unsafe linkuser($latestmember)}
			{@if $combinedstats != null}
				{@render fmt_money.php}
				<br/>Combined online time: {@unsafe round($combinedstats->o)}h &bull; Combined flight time: {@unsafe round($combinedstats->f)}h &bull; Combined distance flown: {@unsafe round($combinedstats->d,1)}km<br/>
				Total flights: {@unsafe $flightcount->c} &bull; Total cash: {@unsafe fmt_money($combinedstats->c)}
			{@endif}
		</p>
		<p><a href="stats.php">More stats</a></p>
	</div></div>
	{@render defaultfooter.tpl}
</body>
</html>
