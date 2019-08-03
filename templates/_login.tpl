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
		form p {
			text-align: center;
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		<div id="login">
			<h2 id="main">Login</h2>
			<form method="post" action="login.php">
				<label><span>Nickname:</span><input type="text" name="usr" /></label><br/>
				<label><span>Password:</span><input type="password" name="pwd" /></label><br/>
				<label><span>Accept cookies (required):</span><input type="checkbox" name="yummie" checked="checked" /></label><br/>
				<label><span>Stay logged in:</span><input type="checkbox" name="stay" /></label>
				<p><input type="submit" value="Login"/></p>
			</form>
		</div>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
