<!DOCTYPE html>
<html lang="en">
<head>
	<title>my account :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		<h2 id="main">My account</h2>
		{@if isset($loggeduser)}
			<ul>
				<li><?php acclink('Public profile', 'publicprofile'); ?></li>
				<li><?php acclink('Failed logins', 'fal'); ?></li>
			</ul>
		{@endif}
		{@mayrequire ^account_notloggedin}
		{@mayrequire ^account_publicprofile}
		{@mayrequire ^account_fal}
		<?php include('../gen/^account_' . $action . '.php'); ?>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
