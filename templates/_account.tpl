<!DOCTYPE html>
<html lang="en">
<head>
	<title>my account :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
	@media (max-width: 800px) {
		aside {
			border-top: 0;
			border-bottom: 2px solid #444;
			width: 100%;
		}
	}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div>
		<aside>
			<p>hi</p>
		</aside>
		<div id="main">
			<?php include('../gen/^account_' . $action . '.php'); ?>
		</div>
		<div class="clear"></div>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
