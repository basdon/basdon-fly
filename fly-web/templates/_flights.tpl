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
				margin: 0 1em;
			}
			#filters select {
				min-height: 15em;
			}
		option.fs {
			{@rem /*12x12 rounded rectangle border #404040 brush width 1 corner size 4*/@}
			background-image: url('data:image/png;base64,
				iVBORw0KGgoAAAANSUhEUgAAAAwAAAAMBAMAAACkW0HUAAAABGdBTUEAALGPC/xhBQAAAAFzUkdC
				AK7OHOkAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAAeUExURUBAQEdwTEBAQEBAQEBAQEBAQEBAQOHD
				w//d3UBAQDgE91wAAAAHdFJOU4MA4Qyrx6OtsUq6AAAANklEQVQI12MQZpo5U8GQwdS9o6MkmIGl
				AwgcGNRBVBHDDBDViZ2CKmEDUQkMpukdHWXBDBDDABlnH1kbPBThAAAAAElFTkSuQmCC');
			background-repeat: no-repeat;
			background-position: 4px center;
			padding-left: 22px;
			}
			option.fs1 { filter: hue-rotate(240deg); }
			option.fs8 { filter: hue-rotate(120deg); }
			option.fs16, option.fs.fs128 { filter: hue-rotate(30deg) saturate(200%); }
			option.fs64 { filter: hue-rotate(270deg); }
			option.fs2048 { filter: hue-rotate(60deg); }
		#filters-expanded ~ div {
			max-height: 0;
			overflow: hidden;
			transition: max-height .2s ease;
		}
		#filters-expanded:checked ~ div {
			max-height: 100vh;
		}
		label[for=filters-expanded] {
			display: inline-block;
			background: #ddd;
			border: 1px solid #777;
			border-radius: 4px;
			padding: .2em .4em;
			cursor: pointer;
			user-select: none;
			}
			label[for=filters-expanded]:hover {
				filter: brightness(90%);
			}
			label[for=filters-expanded]:active {
				filter: brightness(85%);
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
						<strong>Search parameters:</strong> (<a href=/flights.php>clear</a>)
					</p>
					<p style=line-height:1.5em>
						{@if !empty($filter_aircraft)}
							<strong>Aircraft:</strong> 
							{@foreach $filter_aircraft as $model}
								{@unsafe aircraft_name($model)} 
							{@endforeach}
							<br>
						{@endif}
						{@if !empty($filter_pilot_id)}
							<strong>Pilot id:</strong> {$filter_pilot_id}<br>
						{@endif}
						{@if !empty($filter_pilot_name)}
							<strong>Pilot name:</strong> {$filter_pilot_name}<br>
						{@endif}
						{@if !empty($filter_from)}
							<strong>From:</strong> {@unsafe implode($filter_from, ', ')}<br>
						{@endif}
						{@if !empty($filter_to)}
							<strong>To:</strong> {@unsafe implode($filter_to, ', ')}<br>
						{@endif}
						{@if !empty($filter_route_includes)}
							<strong>From/To:</strong> {@unsafe implode($filter_route_includes, ', ')}<br>
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
					{@if isset($flight_list_opts->error_too_many_filters)}
						<p class="msg error">
							Too many filters! Some filters have been dropped.
						</p>
					{@endif}
				{@endif}
				<input style=display:none type=checkbox id=filters-expanded>
				<p>
					<label for=filters-expanded>Change search</label>
				</p>
				<div>
					<p>
						<em>Ctrl+click to select or unselect mutliple.</em>
					</p>
					<fieldset>
						<legend>Aircraft</legend>
						<select name=flqvm[] multiple>
							{@foreach $aircraft_names as $tmp_id => $tmp_name}
								<option 
									value={@unsafe $tmp_id}
									{@if in_array($tmp_id, $filter_aircraft)} selected{@endif}
								>
									{@unsafe $tmp_name}
							{@endforeach}
						</select>
					</fieldset>
					<fieldset>
						<legend>From</legend>
						<select name=flqf[] multiple>
							{@foreach $all_airport_codes as $code}
								<option 
									value={@unsafe $code}
									{@if in_array($code, $filter_from)} selected{@endif}
								>
									{@unsafe $code}
							{@endforeach}
						</select>
					</fieldset>
					<fieldset>
						<legend>To</legend>
						<select name=flqt[] multiple>
							{@foreach $all_airport_codes as $code}
								<option 
									value={@unsafe $code}
									{@if in_array($code, $filter_to)} selected{@endif}
								>
									{@unsafe $code}
							{@endforeach}
						</select>
					</fieldset>
					<fieldset>
						<legend>From/To</legend>
						<select name=flqri[] multiple>
							{@foreach $all_airport_codes as $code}
								<option 
									value={@unsafe $code}
									{@if in_array($code, $filter_route_includes)} selected{@endif}
								>
									{@unsafe $code}
							{@endforeach}
						</select>
					</fieldset>
					<fieldset>
						<legend>Flight status</legend>
						<select name=flqstate[] multiple>
							{@foreach $flightstatuses as $tmp_id => $tmp_name}
								<option 
									class="fs fs{@unsafe $tmp_id}" 
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
