<?php
$flight_list_query_opts = [
	'filter_pilot_id' => 'flqpid',
	'filter_pilot_name' => 'flqpn',
	'page' => 'page',
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
		$fltrs['flqpid'] = (int) $opts->filter_pilot_id;
	} else if (isset($opts->filter_pilot_name)) {
		$where[] = 'name=?';
		$parms[] = $opts->filter_pilot_name;
		$fltrs['flqpn'] = $opts->filter_pilot_name;
	}
	$where = implode($where, ' AND ');
	$limit = 100;
	if (isset($opts->limit)) {
		$limit = $opts->limit;
	}
	$offset = 0;
	if (isset($opts->page) && (int) $opts->page > 0) {
		$page = (int) $opts->page;
		$offset = ($page - 1) * $limit;
	}

	$flight_list = new stdClass();
	$flight_list->opts = $opts;
	$flight_list->filters = $fltrs;
	$flight_list->date_format = 'j M Y';
	$flight_list->show_pilot = true;
	$flight_list->to_flights_page_query_params = http_build_query($fltrs);
	$flight_list->view_from = 0;
	$flight_list->view_to = 0;
	$flight_list->total = 0;

	if (isset($opts->fetch_pagination_data) && $opts->fetch_pagination_data) {
		$flight_list->dbq = (new DBQ())->prepare('SELECT COUNT(id) AS c FROM flg_enriched WHERE '.$where)->executew0ia($parms);
		$meta = $flight_list->dbq->fetch_all();

		if (!is_null($flight_list->dbq->error)) {
			// Let the view use the meta dbq, put 'executed' on false so fetching rows is a nop.
			// Then it will not print any flights but will print the error from the meta query.
			$flight_list->dbq->executed = false;
			return $flight_list;
		}

		$count = $meta[0]->c;
		$flight_list->total = $count;
		if ($offset < $count) {
			$flight_list->view_from = $offset + 1;
			$flight_list->view_to = min($flight_list->total, $offset + $limit);
		} else {
			$flight_list->view_from = 0;
			$flight_list->view_to = 0;
		}

		$flight_list->last_page = ceil($count / $limit);

		if (isset($page) && $page > 1) {
			// Check the last page, so the 'newer' link always links to a page that is not empty,
			// eg if $page is 60, but there are only enough flights to fill 4 pages, we should link to
			// page 4 instead of page 59.
			$fltrs['page'] = min($flight_list->last_page, $page-1);
			$flight_list->newer_page_query_params = http_build_query($fltrs);
		}
		if ($offset + $limit < $count) {
			$fltrs['page'] = isset($page) ? $page+1 : 2;
			$flight_list->older_page_query_params = http_build_query($fltrs);
		}

		$flight_list->page = isset($page) ? $page : 1;
	}

	$flight_list->dbq = (new DBQ())->prepare('SELECT * FROM flg_enriched WHERE '.$where.' LIMIT '.$limit.' OFFSET '.$offset);
	$flight_list->dbq->executew0ia($parms);
	return $flight_list;
}
