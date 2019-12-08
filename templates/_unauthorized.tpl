<!DOCTYPE html>
<html lang="en">
<head>
	<title>unauthorized :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		<h2 id="main">Unauthorized</h2>
		<p class="msg error">You need to <a href="login.php?ret={@urlencode $ret}">log in</a> to see this page</p>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
