<p class="center">
	{@if isset($flight_list->newer_page_query_params)}
		<a href="flights.php{@if !empty($flight_list->newer_page_query_params)}?{@endif}
			{@unsafe $flight_list->newer_page_query_params}"
		>
			&lt; newer
		</a>
	{@else}
		&lt; newer
	{@endif}
	&#32;|&#32;
	{@if isset($flight_list->older_page_query_params)}
		<a href="flights.php?{@unsafe $flight_list->older_page_query_params}">older &gt;</a>
	{@else}
		older &gt;
	{@endif}
</p>
