<h2 id="main">Admin</h2>
<h3>Health checks</h3>
<ul>
	<li>playtime &gt; onlinetime: <span class="indicator indicator-value-{@unsafe $healthchecks->playtime}">{@unsafe $healthchecks->playtime}</span></li>
	<li>flighttime &gt; playtime: <span class="indicator indicator-value-{@unsafe $healthchecks->flighttime}">{@unsafe $healthchecks->flighttime}</span></li>
	<li>distance &gt; 10000000: <span class="indicator indicator-value-{@unsafe $healthchecks->distance}">{@unsafe $healthchecks->distance}</span></li>
</ul>
<h3>Heartbeat</h3>
<ul>
	<li>
		Server uptime:&#32;
		{@if empty($heartbeat)}
			<strong class="indicator">OFFLINE</strong>
		{@else}
			{@unsafe format_duration_short_days(time() - $heartbeat[0]->tstart)}&#32;
			({@unsafe format_datetime($heartbeat[0]->tstart)})
			{@if time() - $heartbeat[0]->tstart > 86400 * 20}
				&#32;<span style="color:#ea0">restart suggested</span>
			{@endif}
		{@endif}
	</li>
	<li>Last heartbeat: {@if empty($lastheartbeat)}<strong class="indicator">NONE?</strong>{@else}{@unsafe format_duration_short_days(time() - $lastheartbeat[0]->tlast)} ago{@endif}</li>
</ul>
<h4>Last sessions</h4>
<table class="new center">
	<thead><tr><th>start</th><th>end</th><th>duration</th><th>cleanexit</th><th>offline time</th></tr></thead>
	<tbody>
		{@rem TODO this loop looks like shit @}
		{@for $i = 0; $i < count($lastsessions); $i++}
			{@eval $s = $lastsessions[$i]}
			<tr>
				<td>{@unsafe format_datetime($s->tstart)}</td>
				<td>{@unsafe format_datetime($s->tlast)}</td>
				<td>{@unsafe format_duration_short_days($s->tlast - $s->tstart)}</td>
				<td>{@unsafe $s->cleanexit}</td>
				<td>
					{@if $i < count($lastsessions)-1}
						{@eval $duration=$s->tstart - $lastsessions[$i+1]->tlast}
						{@if $duration>90}<strong>{@endif}
						{@unsafe format_duration_short_days($duration)}
						{@if $duration>90}</strong>{@endif}
					{@endif}
				</td>
			</tr>
		{@endfor}
	</tbody>
</table>
