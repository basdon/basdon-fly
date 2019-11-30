<div>
	<header>
		<small>
			{@if isset($loggeduser)}
				Welcome, {$loggeduser->name}&#32;
				<a href="account.php">[My account]</a>&#32;
				{@if $loggeduser->lastfal>max($loggeduser->falnw,$loggeduser->falng)}<a href="account.php?action=fal"><strong>[New failed logins]</strong></a>&#32;{@endif}
				<a href="logout.php?k={@unsafe $loggeduser->logoutkey}">[Log out]</a>
			{@else}
				<a href="login.php">[Log In]</a>
			{@endif}
		</small>
		<h1>basdon.net aviation server</h1>
		<p id="ip">IP: <a href="samp://basdon.net:7777">basdon.net:7777</a> (<a href="article.php?title=How_To_Join">How to join</a>)</p>
		<p><a href="index.php">Home</a> :: <a href="news.php">News</a> :: <a href="flights.php">Flights</a> :: <a href="map.php">Live Map</a> :: <a href="article.php">Articles</a> :: <a href="bbs.php">Message Board</a> :: <a href="stats.php">Stats</a></p>
	</header>
</div>
{@foreach $__msgs as $msg}
	<p class="msg">{$msg}</p>
{@endforeach}
{@foreach $__rawmsgs as $msg}
	<p class="msg">{@unsafe $msg}</p>
{@endforeach}
