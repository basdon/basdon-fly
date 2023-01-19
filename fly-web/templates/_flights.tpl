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
		select.flight-state {
			border: 1px solid #777;
			padding: .2em;
			border-radius: 5px;
			}
			select.flight-state:hover {
				filter: brightness(90%);
			}
			select.flight-state:active {
				filter: brightness(85%);
			}
			select.flight-state option {
				background: #fff;
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
						<select 
							name=flqstate 
							class="flight-state f{@unsafe $filter_status}" 
							onchange="this.className='flight-state f'+this.value"
						>
							<option value=""></option>
							{@foreach $flightstatuses as $tmp_id => $tmp_name}
								<option 
									class="flight-state f{@unsafe $tmp_id}" 
									value={@unsafe $tmp_id} 
									{@if $tmp_id == $filter_status}selected{@endif}
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
