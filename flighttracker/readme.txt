flighttracker
-------------
Flight tracker for fly-server, implemented as an anna module.
See github.com/yugecin/anna

configuration
-------------
fdr.path                -- directory where to save the fdr files. will attempt
                           to create if doesn't exist.
critical.output.channel -- channel where to send critical messages, defaults to
                           the channel where this mod was enabled when its value
                           is an empty string.

commands
--------
ft                      -- prints status of flight tracker, with currently
                           active flights. If the flighttracker is not running
                           for whatever reason, it will attempt to restart it.
ft-debug                -- toggles debug output, currently only prints incoming
                           packets. Only available for users with owner perms.
