<h3>{@unsafe $groupname} ({@unsafe count($group)})</h3>
<ul>
{@foreach $group as $apt}
	<li><a href="locations.php?code={@unsafe $apt->c}">{$apt->n}</a>{@if !$apt->e} (disabled){@endif}</li>
{@endforeach}
</ul>
