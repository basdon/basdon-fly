<h2>Failed logins</h2>
{@if isset($falcleared)}
	<p class="msg success">Unseen failed logins cleared.</p>
{@endif}
<table width="100%">
	<thead>
		<tr>
			<th>When</th><th>IP</th><th>Service</th>
		</tr>
	</thead>
	<tbody>
		{@eval ++$db_querycount}
		{@eval $lastseen = $db->query('SELECT falnw,falng FROM usr WHERE i='.$loggeduser->i)->fetchAll()[0]}
		{@eval $lastseen = max($lastseen->falnw, $lastseen->falng)}
		{@eval $hasnew = false}
		{@eval $maxstamp = 0}
		{@eval ++$db_querycount}
		{@foreach $db->query('SELECT stamp,ip,isweb FROM fal WHERE u='.$loggeduser->i.' ORDER BY stamp DESC LIMIT 50') as $l}
			{@if !$maxstamp}{@eval $maxstamp = $l->stamp + 1}{@endif}
			{@if $hasnew && $l->stamp < $lastseen}
				<tr><td colspan="3" style="text-align:center"><a href="account.php?action=fal&amp;clear&amp;cleartime={@unsafe $maxstamp}&amp;confirm={@unsafe $clearkey}">Clear new notifications</a></td></tr>
			{@endif}
			<tr>
				<td>{@unsafe format_time_since($l->stamp)} ({@unsafe date('j M Y H:i', $l->stamp)}){@if $hasnew = ($l->stamp > $lastseen)} <strong>(new!)</strong>{@endif}</td>
				<td>{$l->ip}</td>
				<td>{@if $l}web{@else}game{@endif}</td>
			</tr>
		{@endforeach}
	</tbody>
</table>
