<div style=text-align:center>
	<h2 style="border:2px solid #000;background:#ffa500">Welcome to basdon.net articles!</h2>
	<p>
		<strong>Here you can find resources to learn and enchance your experience on this server.</strong>
	<p>
		<a href=/articleindex.php>Article index</a>
</div>

<style>
	.article th {
		text-align: center;
		background: #ffa500;
	}
	.article tbody tr {
		background: #ddd;
	}
	.article table {
		border-spacing: 1px;
	}
	.article th, .article td {
		border: 0;
	}
</style>

<table width=100%>
	<tbody>
		<tr>
			<th width=25%><?php link_article_cat('Basics'); ?>
			<th width=75%><?php link_article_cat('Airports'); ?>
		<tr valign=top>
			<td>
				<ul>
					<li><?php link_article('How_To_Join'); ?>
					<li><?php link_article('Keys'); ?>
					<li><?php link_article('Fuel'); ?>
					<li><?php link_article('List_of_aircraft'); ?>
					<li><?php link_article('Service_points'); ?>
					<li><?php link_article('Score'); ?>
					<li><?php link_article('HUD'); ?>
				</ul>
			<td class=airportlist>
				<?php readfile('../gen/airportlist.html'); ?>
		<tr>
			<th>
				<?php link_article_cat('Missions'); ?>
			<th>
				<?php link_article_cat('Navigation'); ?>
		<tr valign=top>
			<td>
				<ul>
					<li><?php link_article('Missions'); ?>
					<li><?php link_article('Passenger_Satisfaction'); ?>
					<li><?php link_article('Mission_point_types'); ?>
				</ul>
			<td class=navinfo>
				<?php readfile('../gen/navinfotable.html'); ?>
	</tbody>
</table>
