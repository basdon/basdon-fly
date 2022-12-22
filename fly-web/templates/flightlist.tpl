<table border="0" width="100%" id="flights" class="flightlist center">
	<thead>
		<tr>
			<th width="1px">#</th>
			<th>Aircraft</th>
			{@if $flight_list->show_pilot}<th>Pilot</th>{@endif}
			<th>Route</th>
			<th width="1px">Status</th>
			<th class="right no800">Distance</th>
			<th>Date</th>
		</tr>
	</thead>
	<tbody>
		{@foreach $flight_list->dbq->fetch_generator() as $f}
			{@eval $aircraftname = aircraft_name($f->vehmodel)}
			<tr>
				<td>
					<a href="flight.php?id={@unsafe $f->id}">#{@unsafe $f->id}</a>
				</td>
				<td style="padding:0">
					<img src="/s/s{@unsafe $f->vehmodel}.png" title="{@unsafe $aircraftname}" alt="{@unsafe $aircraftname}" />
				</td>
				{@if $flight_list->show_pilot}
					<td>{@unsafe linkuser($f)}</td>
				{@endif}
				<td class="route">
					<a href="article.php?title={@unsafe $f->f}">{@unsafe $f->f}</a>
					<span>&rarr;</span>
					<a href="article.php?title={@unsafe $f->t}">{@unsafe $f->t}</a>
				</td>
				<td>
					<span class="flight-state f{@unsafe $f->state}">
						{@unsafe fmt_flight_status($f->state, $f->tload)}
					</span>
				</td>
				<td class="right no800">
					{@if $f->state != 1}
						{@unsafe round($f->adistance)}m
					{@endif}
				</td>
				<td>{@unsafe date($flight_list->date_format, $f->tlastupdate)}</td>
			</tr>
		{@endforeach}
	</tbody>
</table>
{@if !empty($flight_list->dbq->error)}
	<p class="msg error">Could not load flight list: {$flight_list->dbq->error}</p>
{@elseif $flight_list->dbq->num_rows_fetched == 0}
	<p class="center">None!</p>
{@endif}
