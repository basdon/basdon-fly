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
			#filters select {
				min-height: 15em;
			}
		#filters-expanded ~ div {
			max-height: 0;
			overflow: hidden;
			transition: max-height .2s ease;
		}
		#filters-expanded:checked ~ div {
			max-height: 100vh;
		}
		#filters-expanded + p span.on,
		#filters-expanded:checked + p span.off {
			display: none;
		}
		#filters-expanded:checked + p span.on {
			display: inline;
		}
		label[for=filters-expanded] {
			text-decoration: underline;
			cursor: pointer;
			color: #06c;
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
				{@if $num_active_filters}
					<p>
						<strong>Active filters</strong> (<a href=/flights.php>clear</a>)
					</p>
					<p style=line-height:1.5em>
						{@if !empty($filter_pilot_id)}
							<strong>Pilot id:</strong> {$filter_pilot_id}<br>
						{@endif}
						{@if !empty($filter_pilot_name)}
							<strong>Pilot name:</strong> {$filter_pilot_name}<br>
						{@endif}
						{@if !empty($filter_status)}
							<strong>Status:</strong> 
							{@foreach $filter_status as $status}
								<span class="flight-state f{@unsafe $status}">
									{@unsafe fmt_flight_status($status, 1)}
								</span> 
							{@endforeach}
							<br>
						{@endif}
					</p>
				{@endif}
				<input style=display:none type=checkbox id=filters-expanded>
				<p>
					<label for=filters-expanded>
						<span class=off>Edit </span>
						<span class=on>Close </span>
						filters
					</label>
				</p>
				<div>
					<p>
						<em>Ctrl+click to select or unselect mutliple.</em>
					</p>
					<fieldset>
						<legend>Flight status</legend>
						<select name=flqstate[] multiple>
							{@foreach $flightstatuses as $tmp_id => $tmp_name}
								<option 
									value={@unsafe $tmp_id}
									{@if in_array($tmp_id, $filter_status)} selected{@endif}
								>
									{$tmp_name}
							{@endforeach}
						</select>
					</fieldset>
					<p>
					<fieldset>
						<legend>Pilot ID</legend>
						<input type=text name=flqpid value="{$filter_pilot_id}" style=max-width:5em>
					</fieldset>
					<fieldset>
						<legend>Pilot name</legend>
						<input type=text name=flqpn value="{$filter_pilot_name}">
					</fieldset>
					<p>
						<input type=submit value=Apply>
					</p>
				</div>
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
