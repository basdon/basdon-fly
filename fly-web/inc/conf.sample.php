<?php

$db_host = '127.0.0.1';
$db_name = '';
$db_uname = '';
$db_passw = '';

$ABS_URL = 'http://basdon-local.net'; // no trailing slash
$COOKIENAME = 'jsessionid';
$COOKIEPATH = '';
$COOKIEDOMAIN = '';
$COOKIEHTTPS = false;
$SECRET1 = die('conf.php: change this to some long, completely random string (that should not be shared) (without the die...)');

// should not be enabled on production because templates don't change in prod (between 'make' invocations) so it would be a waste of cpu time
$__REPARSE_TEMPLATES_EVERY_PAGE_LOAD__ = false;

// Discord webhook url to post flight messages in a Discord channel (edit channel, integrations, create webhook)
//$DISCORD_FLIGHTLOG_WEBHOOK = 'https://discord.com/api/webhooks/yada/yada';
// Discord webhook url to post server crash messages in a Discord channel (edit channel, integrations, create webhook)
//$DISCORD_CRASHLOG_WEBHOOK = 'https://discord.com/api/webhooks/yada/yada';
