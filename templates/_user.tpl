<!DOCTYPE html>
<html lang="en">
<head>
	<title>{@if $id == -1}user not found{@else}profile of {$name} ({@unsafe $id}){@endif} :: basdon aviation server</title>
	{@render defaulthead.tpl}
	<style>
		#flights td{text-align:center}
		#flights td:nth-child(5){text-align:right}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div>
		<div id="main">
			{@if $id == -1}
				<h2>User not found</h2>
				<p>Note that profiles of guest accounts can only be found by using their user account id.</p>
			{@else}
				{@render userpublicprofile.tpl}
			{@endif}
		</div>
		{@render aside.tpl}
		<div class="clear"></div>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
