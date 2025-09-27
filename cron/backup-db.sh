# see crontab-example.txt for the env var config

DATE=$(date +%j-%b-%d-%H-%M-%S)
mysqldump --opt -h 127.0.0.1 --databases basdonfly -u $DB_USER_NAME -p"$DB_USER_PW" | gzip > $DATE.basdonfly.db.gz
scp $SCP_OPTIONS $DATE.basdonfly.db.gz $BASDON_BACKUP_TARGET/db
rm $DATE.basdonfly.db.gz
