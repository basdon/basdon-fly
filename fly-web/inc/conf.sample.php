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

$CSS_SUFFIX = 'css';
// uncomment to use minified css files
//$CSS_SUFFIX = 'm.css'

// set to true to reparse templates on every page load
// don't enable this in production
$__REPARSE__ = false;

// Discord webhook url to post flight messages in a Discord channel (edit channel, integrations, create webhook)
//$DISCORD_FLIGHTLOG_WEBHOOK = 'https://discord.com/api/webhooks/yada/yada';
