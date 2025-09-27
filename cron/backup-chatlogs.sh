# see crontab-example.txt for the env var config

# doing -rt instead of -a because no need to keep owner/permissions etc
rsync -rt --rsh=ssh $BASDON_HOME/fly-web/www/s/chatlogs/ $BASDON_BACKUP_TARGET/chatlogs-rsync
