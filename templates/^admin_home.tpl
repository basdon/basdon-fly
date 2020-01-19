<h2 id="main">Admin</h2>
<h3>Health checks</h3>
<ul>
	<li>playtime &gt; onlinetime: <span class="indicator indicator-value-{@unsafe $healthchecks->playtime}">{@unsafe $healthchecks->playtime}</span></li>
	<li>flighttime &gt; playtime: <span class="indicator indicator-value-{@unsafe $healthchecks->flighttime}">{@unsafe $healthchecks->flighttime}</span></li>
	<li>distance &gt; 10000000: <span class="indicator indicator-value-{@unsafe $healthchecks->distance}">{@unsafe $healthchecks->distance}</span></li>
</ul>
<h3>Heartbeat</h3>
<ul>
	<li>Server uptime: {@if empty($heartbeat)}<strong class="indicator">OFFLINE</strong>{@else}{@unsafe format_duration_short(time() - $heartbeat[0]->tstart)} ({@unsafe format_datetime($heartbeat[0]->tstart)}){@endif}</li>
	<li>Last heartbeat: {@if empty($lastheartbeat)}<strong class="indicator">NONE?</strong>{@else}{@unsafe format_duration_short_days(time() - $lastheartbeat[0]->tlast)} ago{@endif}</li>
</ul>
<h4>Last sessions</h4>
<table class="default center">
	<thead><tr><th>start</th><th>end</th><th>duration</th><th>cleanexit</th></tr></thead>
	<tbody>
		{@for $i = 0; $i < count($lastsessions); $i++}
			{@eval $s = $lastsessions[$i]}
			{@if $i > 0}
			<tr>
				<td colspan="4"><strong>offline for {@unsafe format_duration_short_days($lastsessions[$i - 1]->tstart - $s->tlast)}</strong></td>
			</tr>
			{@endif}
			<tr>
				<td>{@unsafe format_datetime($s->tstart)}</td>
				<td>{@unsafe format_datetime($s->tlast)}</td>
				<td>{@unsafe format_duration_short_days($s->tlast - $s->tstart)}</td>
				<td>{@unsafe $s->cleanexit}</td>
			</tr>
		{@endfor}
	</tbody>
</table>
