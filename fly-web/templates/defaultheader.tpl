<header>
	<div class=wrapper>
		<h1>
			<a href=/>
				<img src=/s/brand/logo-full2-small.png aria-hidden=true>
			</a>
			{@rem display the logo image subtext in textual form for screen reader users @}
			<span style=position:absolute;top:-100em>basdon.net aviation server</span>
		</h1>
		<nav>
			{@rem reverse order because float @}
			<a href=stats.php>Stats</a>
			<a href=article.php>Articles</a>
			<a href=flights.php>Flights</a>
			<a href=/>Home</a>
		</nav>
		<div class=clear></div>
		<hr>
		<p>
			<span id=ip>IP: basdon.net:7777 (<a href="article.php?title=How_To_Join">How to join</a>)</span>
			{@if isset($loggeduser)}
				Welcome, {$loggeduser->name}! 
				{@if group_is_admin($loggeduser->groups)}
					<a href="admin.php">[Admin]</a> 
				{@endif}
				<a href="account.php">[My account]</a> 
				{@if $loggeduser->hasfailedlogins}
					<a href="account.php?action=fal#u{@unsafe $loggeduser->lastfal}"><strong>[New failed logins]</strong></a> 
				{@endif}
				<a href="logout.php?k={@unsafe $loggeduser->logoutkey}">[Log out]</a>
			{@else}
				<a href="login.php">[Log In]</a>
			{@endif}
		</p>
	</div>
</header>
{@if count($__msgs) + count($__rawmsgs)}
	<div class="wrapper wrapper-pad">
		{@foreach $__msgs as $msg}
			<p class=topmsg>{$msg}</p>
		{@endforeach}
		{@foreach $__rawmsgs as $msg}
			<p class=topmsg>{@unsafe $msg}</p>
		{@endforeach}
	</div>
{@endif}
