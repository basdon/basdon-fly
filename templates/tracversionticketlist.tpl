{@rem -

expect $list with list of tickets, props:
->id
->severity
->summary
@}

<ul>
	{@if count($list)}
		{@foreach $list as $r}
			<li><a href="tracview.php?id={@unsafe $r->id}#u{@unsafe $r->updated}">{@unsafe sprintf('%06d', $r->id)}</a>: [<span class="severity{@unsafe $r->severity}">{@unsafe $trac_severities[$r->severity]}</span>] {$r->summary}</li>

		{@endforeach}
	{@else}
		<li>None!</li>
	{@endif}
</ul>
