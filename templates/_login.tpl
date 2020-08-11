<!DOCTYPE html>
<html lang="en">
<head>
	<title>login :: basdon.net aviation server</title>
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
	<div><div>
		<div id="login">
			{@if isset($loggeduser)}
				<p>You're already logged in! <a href="logout.php?k={@unsafe $loggeduser->logoutkey}">Logout</a></p>
			{@else}
				<h2 id="main">Login</h2>
				{@if isset($attemptedlogin)}
					<p class="msg error">Invalid credentials</p>
				{@endif}
				<form method="post" action="login.php{@if isset($_GET['ret'])}?ret={@unsafe urlencode($_GET['ret'])}{@endif}">
					<label><span>Nickname:</span>{@input text usr autofocus="autofocus"}</label><br/>
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
	</div></div>
	{@render defaultfooter.tpl}
</body>
</html>
