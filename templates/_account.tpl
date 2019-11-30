<!DOCTYPE html>
<html lang="en">
<head>
	<title>my account :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		@media (max-width: 800px) {
			aside {
				border-top: 0;
				border-bottom: 5px solid #444;
				width: 100%;
			}
		}
		#flights td{text-align:center}
		#flights td:nth-child(5){text-align:right}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div>
		<aside>
			{@if isset($loggeduser)}
				<ul>
					<li><?php acclink('Public profile', 'publicprofile'); ?></li>
					<li><?php acclink('Failed logins', 'fal'); ?></li>
				</ul>
			{@endif}
		</aside>
		<div id="main">
			{@mayrequire ^account_notloggedin}
			{@mayrequire ^account_publicprofile}
			{@mayrequire ^account_fal}
			<?php include('../gen/^account_' . $action . '.php'); ?>
		</div>
		<div class="clear"></div>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
