<?php
$trac_statuses = array(
	0 => 'new',
	1 => 'feedback',
	2 => 'acknowledged',
	3 => 'ready',
	4 => 'resolved',
	5 => 'closed',
);
$trac_severities_raw = array(
	0 => 'unrated',
	1 => 'tweak',
	2 => 'text',
	4 => 'trivial',
	5 => 'minor',
	6 => 'major',
	7 => 'critical',
	8 => 'crash',
);
$trac_severities = array(
	0 => 'unrated',
	1 => 'tweak',
	2 => 'text',
	4 => 'trivial',
	5 => 'minor',
	6 => '<strong>major</strong>',
	7 => '<strong>critical</strong>',
	8 => '<strong style="color:#f00">crash</strong>',
);
$trac_visibilities = array(
	$GROUPS_ALL => 'public',
	$GROUPS_ADMIN => 'admins',
	$GROUP_OWNER => 'owner',
);
