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
	<main class=center>
		<h2 id="main" class="left">Welcome!</h2>
		<p class="left">
			basdon.net aviation server is a server for <a href="https://www.sa-mp.com/">San Andreas Multiplayer (SA-MP)</a>{@globe}, 
			a multiplayer modification for the game <a href="http://www.rockstargames.com/sanandreas/">Grand Theft Auto: San Andreas</a>{@globe}. 
			The main activity is to grab a plane or helicopter and enjoy flying around, either freely or by accepting missions towards various destinations. 
			Start your flight and have fun!
		</p>

		<h3 style="border:0">Server IP: basdon.net:7777</h3>
		<p>
			<a href="https://files.sa-mp.com/sa-mp-0.3.7-R5-1-install.exe">SA-MP 0.3.7</a> - <a href="article.php?title=How_To_Join">How to join</a>
		</p>
		<p>
			Discord: <a href="https://discordapp.com/invite/w4jFpqd" rel="nofollow">https://discordapp.com/invite/w4jFpqd</a>{@globe}
		</p>
		<p>
			IRC: #basdon (<a href="/s/chatlogs/basdon/?C=M;O=D">chatlogs</a>) and #basdon.echo (<a href="/s/chatlogs/basdon.echo/?C=M;O=D">chatlogs</a>) on LUNet (<a href="irc://irc.liberty-unleashed.co.uk">irc.liberty-unleashed.co.uk</a>{@globe})
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
			<a href="/s/showcase-spitfire.jpg"><img src="/s/showcase-spitfire.jpg" title="Jenis landing at DUNE"></a>
			<a href="/s/showcase-jobmap.jpg"><img src="/s/showcase-jobmap.jpg" title="mission map"></a>
			<a href="/s/showcase-spitfire2.png"><img src="/s/showcase-spitfire2.png" title="Jenis"></a>
			<a href="flight.php?id=2572"><img src="/s/showcase-map.png" title="flight map"></a>
		</p>

		<h3>Player activity</h3>
		<p><img style=width:980px src=/s/gen/playergraph.png></p>

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

		<h3>Recent flights</h3>
		{@render flightlist.tpl}

		<p><a href="flights.php">All flights</a></p>

		<h3>Member stats</h3>
		<p>
			Members: {@unsafe $membercount->c} &bull; Guest sessions: {@unsafe $guestcount->c} &bull; Latest: {@unsafe linkuser($latestmember)}
			{@if $combinedstats != null}
				<br/>
				Combined online time: {@unsafe number_format($combinedstats->o, 0, '', ',')}h 
				&bull; 
				Combined flight time: {@unsafe number_format($combinedstats->f, 0, '', ',')}h 
				&bull; 
				Combined distance flown: {@unsafe number_format($combinedstats->d, 0, '', ',')}km
				<br/>
				Total flights: {@unsafe $flightcount->c} 
				&bull; 
				Total cash: {@unsafe fmt_money($combinedstats->c)}
			{@endif}
		</p>
		<p><a href="stats.php">More stats</a></p>
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
