<h2>Failed logins</h2>
{@if isset($falcleared)}
	<p class="msg success">Unseen failed logins cleared.</p>
{@endif}
{@unsafe $pagination}
<table width="100%">
	<thead>
		<tr>
			<th>When</th><th>IP</th><th>Service</th>
		</tr>
	</thead>
	<tbody>
		{@while $l = $failedlogins->fetch()}
			<tr>
				<td>
					{@if $l->stamp > $lastseen}
						<strong>(new!) </strong>
					{@endif}
					{@unsafe format_time_since($l->stamp)} ({@unsafe date('j M Y H:i', $l->stamp)})
				</td>
				<td>{$l->ip}</td>
				<td>{@if $l->isweb}web{@else}game{@endif}</td>
			</tr>
			{@if $l->stamp == $firstunseen}
				<tr><td colspan="3" style="text-align:center"><a href="account.php?action=fal&amp;cleartime={@unsafe $cleartime}&amp;confirm={@unsafe $clearkey}">Mark all as seen</a></td></tr>
			{@endif}
		{@endwhile}
	</tbody>
</table>
{@unsafe $pagination}
