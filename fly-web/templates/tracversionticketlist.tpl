{@rem expect $ticketlist_tickets with array of tickets, props: id,severity,state,stamp,updated,summary @}

{@if count($ticketlist_tickets)}
	<table class=trac>
		<thead>
			<tr>
				<th width=1%>Reporte date
				<th width=1%>Impact
				<th>Summary
		<tbody>
			{@foreach $ticketlist_tickets as $t}
				<tr class="state{@unsafe $t->state}">
					<td>{@unsafe format_datetime($t->stamp)}</td>
					<td class="severity{@unsafe $t->severity}">{@unsafe $trac_severities[$t->severity]}
					<td><a href="tracview.php?id={@unsafe $t->id}#u{@unsafe $t->updated}">{$t->summary}

			{@endforeach}
	</table>
{@else}
	<p>None!</p>
{@endif}
