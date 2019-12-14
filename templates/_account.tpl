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
		{@if isset($loggeduser)}
			<hr/>
			<p><?php acclink('Public profile', 'publicprofile'); ?> &bull; <?php acclink('Failed logins', 'fal'); ?></li></p>
			<hr/>
		{@endif}
		{@rem every subview needs the #main element! @}
		{@mayrequire ^account_notloggedin}
		{@mayrequire ^account_publicprofile}
		{@mayrequire ^account_fal}
		<?php include('../gen/^account_' . $action . '.php'); ?>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
