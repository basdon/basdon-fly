<h2 id="main">Anticheat log</h2>
{@unsafe $pagination = simple_pagination('admin.php?action=acl&page=', $page, $totalrows, 100)}
<table class="new center" width="100%">
	<thead><tr><th>time</th><th>user</th><th>logged in</h><th>ip</th><th>type</th><th>extra info</th></tr></thead>
	<tbody>
		{@foreach $aclentries as $e}
			<tr>
				<td>{@unsafe date($datetimeformat, $e->t)}</td>
				<td>{@unsafe userlink($e)}</td>
				<td>{@unsafe loggedin($e->l)}</td>
				<td>{@unsafe $e->j}</td>
				<td>{@unsafe type($e->type)}</td>
				<td>{$e->e}</td>
			</tr>
		{@endforeach}
	</tbody>
</table>
{@unsafe $pagination}
