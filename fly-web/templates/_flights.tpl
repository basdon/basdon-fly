<!DOCTYPE html>
<html lang="en">
<head>
	<title>flights :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
	<style>
		#filters {
			background: #eee;
			padding: 1px 1em;
			border-radius: .5em;
			display: inline-block;
			margin: 0 auto;
			}
			#filters fieldset {
				display: inline-block;
				border-radius: .3em;
				margin: 0 .5em;
			}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div><div>
		<h2 id="main">Flights</h2>
		<div class=center>
			<form id=filters>
				<p>
					<strong>Filters</strong>
				</p>
				<p>
					<fieldset>
						<legend>Flight status</legend>
						<select name=flqstate>
							<option value=""></option>
							{@foreach $flightstatuses as $tmp_id => $tmp_name}
								<option class="flight-state f{$tmp_id}" value={$tmp_id}
									{@if $tmp_id == $filter_status} selected{@endif}
								>
									{$tmp_name}
								</option>
							{@endforeach}
						</select>
					</fieldset>
					<fieldset>
						<legend>Pilot ID</legend>
						<input type=text name=flqpid value="{$filter_pilot_id}" style=max-width:5em>
					</fieldset>
					<fieldset>
						<legend>Pilot name</legend>
						<input type=text name=flqpn value="{$filter_pilot_name}">
					</fieldset>
				</p>
				<p>
					<input type=submit value=Apply>
				</p>
			</form>
			<p>
				Showing results
				{@unsafe " {$flight_list->view_from}-{$flight_list->view_to} of {$flight_list->total} "}
				flights.
			</p>
		</div>
		{@render flightlist_paginator.tpl}
		{@render flightlist.tpl}
		{@render flightlist_paginator.tpl}
	</div></div>
	{@render defaultfooter.tpl}
</body>
</html>
