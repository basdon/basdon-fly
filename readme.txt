cronjobs
--------
*/5     *       *       *       *       cd cli; php genplayergraph.php
# backups (fdr, chatlogs, db)

site config
-----------
- www webroot: www
- static (see inc/conf.php) webroot: static
- static: ErrorDocument 404 "404 Not Found"
