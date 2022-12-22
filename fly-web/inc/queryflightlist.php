<?php
$flight_list_query_opts = [
	'filter_pilot_id' => 'flqpid',
	'filter_pilot_name' => 'flqpn',
	'before_flight_id' => 'flqbefore',
];

function flight_list_query_get_opts_from_query_parameters()
{
	global $flight_list_query_opts;

	$opts = new stdClass();
	foreach ($flight_list_query_opts as $opt_name => $opt_param) {
		if (isset($_GET[$opt_param])) {
			$opts->$opt_name = $_GET[$opt_param];
		}
	}
	return $opts;
}

/**delete when all queries are changed to work with ERRMODE_SILENT*/
function flight_list_query($opts)
{
	global $db;

	$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
	$res = _flight_list_query($opts);
	$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
	return $res;
}

function _flight_list_query($opts)
{
	$where = ['1'];
	$parms = [];
	$fltrs = [];
	if (isset($opts->filter_pilot_id)) {
		$where[] = 'i=?';
		$parms[] = (int) $opts->filter_pilot_id;
		$fltrs[] = 'flqpid='.((int) $opts->filter_pilot_id);
	} else if (isset($opts->filter_pilot_name)) {
		$where[] = 'name=?';
		$parms[] = $opts->filter_pilot_name;
		$fltrs[] = 'flqpn='.urlencode($opts->filter_pilot_name);
	}
	$where = implode($where, ' AND ');
	if (isset($opts->before_flight_id)) {
		$before = (int) $opts->before_flight_id;
		if ($before > 0) {
			$beforecond = ' AND id<'.$before;
			$aftercond = ' AND id>'.$before;
			goto havebefore;
		}
		unset($before);
	}
	$beforecond = '';
	$aftercond = '';
havebefore:
	if (!isset($opts->limit)) {
		$opts->limit = 100;
	}
	$limit = $opts->limit;

	$flight_list = new stdClass();
	$flight_list->opts = $opts;
	$flight_list->date_format = 'j M H:i';
	$flight_list->show_pilot = true;
	$flight_list->to_flights_page_query_params = implode($fltrs, '&');
	$flight_list->view_from = 0;
	$flight_list->view_to = 0;
	$flight_list->total = 0;

	if (isset($opts->fetch_pagination_data) && $opts->fetch_pagination_data) {
		// the meta query contains the same WHERE clause 5 times, so we need a few copies of the parameters :D
		$meta_parms = array_merge($parms, $parms);
		$meta_parms = array_merge($meta_parms, $meta_parms);
		$meta_parms = array_merge($meta_parms, $parms);
		$flight_list->dbq = (new DBQ())->prepare(
			$x =
			'(SELECT MIN(id) AS a, MAX(id) AS b, COUNT(id) AS c, 1 AS d
			FROM flg_enriched WHERE '.$where.')
			UNION
			(SELECT
				(SELECT id FROM flg_enriched WHERE '.$where.$beforecond.' ORDER BY id DESC LIMIT 1 OFFSET '.$limit.') AS a,
				(SELECT id FROM flg_enriched WHERE '.$where.$aftercond.' ORDER BY id ASC LIMIT 1 OFFSET '.($limit-1).') AS b,
				(SELECT id FROM flg_enriched WHERE '.$where.' ORDER BY id ASC LIMIT 1 OFFSET '.($limit-1).') AS c,
				(SELECT COUNT(id) FROM flg_enriched WHERE '.$where.$beforecond.') AS d
			)'
		)->executew0ia($meta_parms);
		$meta = $flight_list->dbq->fetch_all();

		if (!is_null($flight_list->dbq->error)) {
			// Let the view use the meta dbq, put 'executed' on false so fetching rows is a nop.
			// Then it will not print any flights but will print the error from the meta query.
			$flight_list->dbq->executed = false;
			return $flight_list;
		}

		if (count($meta) != 2) {
			// Not a single flight was found for this (maybe non-existing) account!
			// It probably returned 1 row, because the first query will still return something (NULL, NULL, 0).
			// Since we're early returning, num_rows_fetched will probably be still be 1 from the meta query,
			// while it should be the result of the actual query (0), so set that now.
			$flight_list->dbq->num_rows_fetched = 0;
			return $flight_list;
		}

		$meta[0]->older_page_before = $meta[1]->a; // may be NULL
		$meta[0]->newer_page_before = $meta[1]->b; // may be NULL
		$meta[0]->oldest_page_before = $meta[1]->c; // may be NULL
		$meta[0]->count_from_view_til_end = $meta[1]->d;
		$meta = $meta[0];
		$meta->min_id = $meta->a;
		$meta->max_id = $meta->b;
		$meta->count = $meta->c;

		$flight_list->view_from = $meta->count - $meta->count_from_view_til_end + 1;
		$flight_list->view_to = $flight_list->view_from + min($limit, $meta->count_from_view_til_end) - 1;
		$flight_list->total = $meta->count;

		if (isset($before) && $meta->count - $meta->count_from_view_til_end > 0) {
			$flight_list->newest_page_query_params = implode($fltrs, '&');
			if (is_null($meta->newer_page_before)) {
				// newer pages have <= $limit flights, so we don't need a 'before' and can show all most recents
				$flight_list->newer_page_query_params = implode($fltrs, '&');
			} else {
				$filters_with_before = $fltrs;
				$filters_with_before[] = 'flqbefore='.$meta->newer_page_before;
				$flight_list->newer_page_query_params = implode($filters_with_before, '&');
			}
		}
		if (!is_null($meta->older_page_before)) {
			$filters_with_before = $fltrs;
			// This is +1 because offset is $limit instead of $limit-1,
			// otherwise it could link to a page that has empty results;
			$filters_with_before[] = 'flqbefore='.($meta->older_page_before+1);
			$flight_list->older_page_query_params = implode($filters_with_before, '&');
		}
		if (!is_null($meta->oldest_page_before) && (!isset($before) || $meta->oldest_page_before + 1 < $before)) {
			$filters_with_before = $fltrs;
			// This is +1 because offset is $limit instead of $limit-1,
			// otherwise it could link to a page that has empty results;
			$filters_with_before[] = 'flqbefore='.($meta->oldest_page_before+1);
			$flight_list->oldest_page_query_params = implode($filters_with_before, '&');
		}
	}

	$flight_list->dbq = (new DBQ())->prepare('SELECT * FROM flg_enriched WHERE '.$where.$beforecond.' LIMIT '.$limit);
	$flight_list->dbq->executew0ia($parms);
	return $flight_list;
}
