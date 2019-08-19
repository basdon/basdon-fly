<!DOCTYPE html>
<html lang="en">
<head>
	<title>{@if isset($failedlogins)}failed logins{@else}login{@endif} :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		@media(width > 600px) {
			#login {
				width: 50%;
				margin: 2em auto;
			}
		}
		form span {
			display: inline-block;
			width: 43%;
			text-align: right;
			margin-right: 1ex;
		}
		@media(width < 25em) {
			form span {
				display: block;
				width: 100%;
				text-align: left;
				margin-right: 0;
			}
		}
		#login p {
			text-align: center;
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		{@if isset($failedlogins)}
			<h2 id="main">Failed login attempts</h2>
			{@if count($failedlogins) == 1}
				<p>There was 1 failed login attempt since your last visit.</p>
			{@else}
				<p>There were {@unsafe count($failedlogins)} failed login attempts since your last visit.</p>
			{@endif}
			<p><a href="{@unsafe $BASEPATH}/{@if isset($_GET['ret'])}{$_GET['ret']}{@endif}">continue</a></p>
			<table>
				<thead>
					<tr>
						<th>When</th><th>IP</th><th>Service</th>
					</tr>
				</thead>
				<tbody>
					{@foreach $failedlogins as $l}
						<tr>
							<td>{@unsafe format_time_since($l->stamp)} ({@unsafe date('j M Y H:i', $l->stamp)})</td>
							<td>{$l->ip}</td>
							<td>{@if $l}web{@else}game{@endif}</td>
						</tr>
					{@endforeach}
				</tbody>
			</table>
			<p><a href="account.php?action=fal">view all failed login attempts</a></p>
		{@else}
			<div id="login">
				{@if isset($loggeduser)}
					<p>You're already logged in! <a href="logout.php?k={@unsafe $loggeduser->logoutkey}">Logout</a></p>
				{@else}
					<h2 id="main">Login</h2>
					{@if isset($loginerr)}
						<p class="msg error">{$loginerr}</p>
					{@endif}
					<form method="post" action="login.php{@if isset($_GET['ret'])}?ret={@unsafe urlencode($_GET['ret'])}{@endif}">
						<label><span>Nickname:</span>{@input text usr}</label><br/>
						<label><span>Password:</span>{@input password pwd}</label><br/>
						<label><span>Accept cookies (required):</span>{@input checkbox yummie checked}</label><br/>
						<label><span>Stay logged in:</span>{@input checkbox stay}</label>
						<p>{@input submit Login}</p>
					</form>
					<p>
						Don't have an account?<br/>
						Register now by joining in-game: <a href="samp://basdon.net:7777">basdon.net:7777</a><br/>
						Need help? <a href="article.php?title=How_To_Join">How to join</a> or <a href="contact.php">contact us</a>.
					</p>
				{@endif}
			</div>
		{@endif}
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
