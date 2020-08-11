<!DOCTYPE html>
<html lang="en">
<head>
	<title>login :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div><div>
		<p style="text-align:center;margin: 2em 0">
			{@if isset($loggeduser)}
				Something went wrong, please try again: <a href="logout.php?k={@unsafe $loggeduser->logoutkey}">logout</a>
			{@else}
				You're not logged in! <a href="login.php">Login</a>
			{@endif}
		</p>
	</div></div>
	{@render defaultfooter.tpl}
</body>
</html>
