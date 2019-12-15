<!DOCTYPE html>
<html lang="en">
<head>
	<title>admin :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		.healthcheck-value-0 {
			color: #3b3;
		}
		.healthcheck:not(.healthcheck-value-0) {
			color: #e22;
			font-weight: bold;
		}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		<h2 id="main">Admin</h2>
		<h3>Health checks</h3>
		<ul>
			<li>playtime &gt; onlinetime: <span class="healthcheck healthcheck-value-{@unsafe $healthchecks->playtime}">{@unsafe $healthchecks->playtime}</span></li>
			<li>flighttime &gt; playtime: <span class="healthcheck healthcheck-value-{@unsafe $healthchecks->flighttime}">{@unsafe $healthchecks->flighttime}</span></li>
			<li>distance &gt; 10000000: <span class="healthcheck healthcheck-value-{@unsafe $healthchecks->distance}">{@unsafe $healthchecks->distance}</span></li>
		</ul>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
