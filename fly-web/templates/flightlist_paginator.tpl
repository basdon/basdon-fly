<form>
	<p class=center>
		{@if isset($flight_list->newer_page_query_params)}
			<a href="flights.php?{@unsafe $flight_list->newer_page_query_params}">&lt; newer</a>
			<span aria-hidden=true>&#32;|&#32;</span>
		{@endif}
		{@foreach $flight_list->filters as $filter_name => $filter_value}
			{@if is_array($filter_value)}
				{@foreach $filter_value as $value}
					<input type=hidden name={@unsafe $filter_name}[] value="{$value}">
				{@endforeach}
			{@else}
				<input type=hidden name={@unsafe $filter_name} value="{$filter_value}">
			{@endif}
		{@endforeach}
		page:
		<select name=page>
			{@for $p = 1; $p <= $flight_list->last_page; $p++}
				<option{@if $flight_list->page == $p} selected{@endif}>
					{@unsafe $p}
			{@endfor}
		</select>
		<input type=submit value=go>
		{@if isset($flight_list->older_page_query_params)}
			<span aria-hidden=true>&#32;|&#32;</span>
			<a href="flights.php?{@unsafe $flight_list->older_page_query_params}">older &gt;</a>
		{@endif}
	</p>
</form>
