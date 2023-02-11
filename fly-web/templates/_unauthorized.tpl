<!DOCTYPE html>
<html lang="en">
<head>
	<title>unauthorized :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<main>
		<h2 id="main">Unauthorized</h2>
		{@if isset($loggeduser)}
			<p class="msg error">You don't have acces to this page</p>
		{@else}
			<p class="msg error">You need to <a href="login.php?ret={@urlencode $ret}">log in</a> to see this page</p>
		{@endif}
	</main>
	{@render defaultfooter.tpl}
</body>
</html>
