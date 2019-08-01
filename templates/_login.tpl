<!DOCTYPE html>
<html lang="en">
<head>
	<title>login :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		<h2 id="main">Login</h2>
		<form method="post" action="login.php">
			<label>Nickname: <input type="text" name="usr" /></label><br/>
			<label>Password: <input type="password" name="pwd" /></label><br/>
			<label><input type="checkbox" name="yummie" checked="checked" /> accept cookies (required)</label><br/>
			<label><input type="checkbox" name="stay" /> stay logged in</label><br/>
			<input type="submit" value="Login"/>
		</form>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
