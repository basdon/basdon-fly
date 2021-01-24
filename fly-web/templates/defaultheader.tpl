<header>
	<div>
		<small>
			{@if isset($loggeduser)}
				Welcome, {$loggeduser->name}&#32;
				{@if group_is_admin($loggeduser->groups)}
					<a href="admin.php">[Admin]</a>&#32;
				{@endif}
				<a href="account.php">[My account]</a>&#32;
				{@if $loggeduser->hasfailedlogins}
					<a href="account.php?action=fal#u{@unsafe $loggeduser->lastfal}"><strong>[New failed logins]</strong></a>&#32;
				{@endif}
				<a href="logout.php?k={@unsafe $loggeduser->logoutkey}">[Log out]</a>
			{@else}
				<a href="login.php">[Log In]</a>
			{@endif}
		</small>
		<h1><a href="./">basdon.net aviation server <span style="font-size:.7em;color:#28c">beta</span></a></h1>
	</div>
	<img src="{@unsafe $STATICPATH}/banner.png" style="width:100%" alt="header panorama img"/>
	<div>
		<p id="ip">IP: <a href="samp://basdon.net:7777">basdon.net:7777</a> (<a href="article.php?title=How_To_Join">How to join</a>)</p>
		<p><a href="./">Home</a> <!--:: <a href="news.php">News</a>--> :: <a href="flights.php">Flights</a> <!--:: <a href="map.php">Live Map</a>--> :: <a href="article.php">Articles</a> :: <a href="trac.php">Tracker</a> <!--:: <a href="bbs.php">Message Board</a>--> :: <a href="stats.php">Stats</a></p>
	</div>
</header>
{@foreach $__msgs as $msg}
	<p class="msg">{$msg}</p>
{@endforeach}
{@foreach $__rawmsgs as $msg}
	<p class="msg">{@unsafe $msg}</p>
{@endforeach}
