<!DOCTYPE html>
<html lang="en">
<head>
	<title>home :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div><div>
		<h2 id="main">Welcome!</h2>
		<p>
			basdon.net aviation server is a server for <a href="https://www.sa-mp.com/">San Andreas Multiplayer (SA-MP)</a>{@globe}, 
			a multiplayer modification for the game <a href="http://www.rockstargames.com/sanandreas/">Grand Theft Auto: San Andreas</a>{@globe}. 
			The main activity is to grab a plane or helicopter and enjoy flying around, either freely or by accepting missions towards various destinations. 
			Start your flight and have fun!
		</p>

		<h2>Server Info</h2>
		<p>
			Join by adding <a href="samp://basdon.net:7777">basdon.net:7777</a> to your list of servers and clicking 'connect' (<a href="article.php?title=How_To_Join">How to join</a>). Our server is running using the latest official release: 0.3.7.
		</p>
		<p>
			Our IRC channels are #basdon (for general talk) and #basdon.echo (with relayed server chat) on the irc.tl network: <a href="irc://chat.irc.tl">chat.irc.tl</a>{@globe}. Chatlogs can be found at <a href="https://static.basdon.net/chatlogs">https://static.basdon.net/chatlogs</a>.
		</p>
		<p>
			<p>We also have a Discord server, invite link: <a href="https://discordapp.com/invite/w4jFpqd" rel="nofollow">https://discordapp.com/invite/w4jFpqd</a>{@globe}.
		</p>
		<p>
			<strong>Last updates:</strong>
			<ul>
				{@foreach $last_update_articles as $update_article}
					<li><a href="article.php?title={@urlencode $update_article->name}">{$update_article->title}</a></li>
				{@endforeach}
			</ul>
		</p>

		<h3 class="newtableheader">Players</h3>
		<table border="0" width="100%" class="new center">
			<thead>
				<tr><th>Online over last 24h</th></tr>
			</thead>
			<tbody>
				<tr><td style="padding:0"><img style="width:100%;border-bottom:1px solid #000" src="{@unsafe $STATICPATH}/gen/playergraph.png" /></td></tr>
				<tr><td><strong>Online now</strong></td></tr>
				<tr>
					<td>
						{@if count($onlineplayers)}
							{@foreach $onlineplayers as $p}
								{@unsafe linkuser($p)}&nbsp;({@unsafe $p->score})&#32;
							{@endforeach}
						{@else}
							None
						{@endif}
					</td>
				</tr>
				<tr><td><strong>Online last 48h</strong></td></tr>
				<tr>
					<td>
						{@if count($last48)}
							{@foreach $last48 as $p}
								{@unsafe linkuser($p)}&nbsp;({@unsafe $p->score})&#32;
							{@endforeach}
						{@else}
							None
						{@endif}
					</td>
				</tr>
			</tbody>
		</table>

		{@render flightstatuses.php}
		{@render aircraftnames.php}

		<h3 class="newtableheader">Active flights</h3>
		<table border="0" width="100%" class="new highlight-row center">
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
				{@if $activeflights !== false && ($activeflights = $activeflights->fetchAll()) && count($activeflights)}
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
				{@else}
					<tr><td colspan="7">None</td></tr>
				{@endif}
			</tbody>
		</table>

		<h3 class="newtableheader">Last 10 finished flights</h3>
		<table border="0" width="100%" class="new highlight-row center">
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
				{@if $lastflights !== false && ($lastflights = $lastflights->fetchAll()) && count($lastflights)}
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
				{@else}
					<tr><td colspan="8">None</td></tr>
				{@endif}
			</tbody>
		</table>

		<p class="center"><a href="flights.php">All flights</a></p>

		<h3 class="newtableheader">Member stats</h3>
		<table border="0" width="100%" class="new center">
			<thead>
				<tr><th>All time</th></tr>
			</thead>
			<tbody>
				<tr><td>
					Members: {@unsafe $membercount->c} &bull; Guest sessions: {@unsafe $guestcount->c} &bull; Latest: {@unsafe linkuser($latestmember)}
					{@if $combinedstats != null}
						{@render fmt_money.php}
						<br/>Combined online time: {@unsafe round($combinedstats->o)}h &bull; Combined flight time: {@unsafe round($combinedstats->f)}h &bull; Combined distance flown: {@unsafe round($combinedstats->d,1)}km<br/>
						Total flights: {@unsafe $flightcount->c} &bull; Total cash: {@unsafe fmt_money($combinedstats->c)}<br/>
					{@endif}
				</td></tr>
			</tbody>
		</table>

		<p class="center"><a href="stats.php">More stats</a></p>
	</div></div>
	{@render defaultfooter.tpl}
</body>
</html>
