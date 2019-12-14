<!DOCTYPE html>
<html lang="en">
<head>
	<title>{@if $id == -1}user not found{@else}profile of {$name} ({@unsafe $id}){@endif} :: basdon aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		{@if $id == -1}
			<h2 id="main">User not found</h2>
			<p>Note that profiles of guest accounts can only be found by using their user account id.</p>
		{@else}
			{@render userpublicprofile.tpl}
		{@endif}
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
