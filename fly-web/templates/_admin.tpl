<!DOCTYPE html>
<html lang="en">
<head>
	<title>admin :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		.indicator-value-0 {
			color: #3b3;
		}
		.indicator:not(.indicator-value-0) {
			color: #e22;
			font-weight: bold;
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div><div>
		<hr/>
		<p><?php adminlink('Home', 'home'); ?> &bull; <?php adminlink('Anticheat log', 'acl'); ?></p>
		<hr/>
		{@rem every subview needs the #main element! @}
		{@mayrequire ^admin_home}
		{@mayrequire ^admin_acl}
		<?php include('../templates/gen/^admin_' . $action . '.php'); ?>
	</div></div>
	{@render defaultfooter.tpl}
</body>
</html>
