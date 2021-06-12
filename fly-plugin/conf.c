static char conf_php_path[100];
static char *conf_mysql_user, *conf_mysql_db, *conf_mysql_pw;

static
void conf_load()
{
	char line[2048], *val, *target;
	int key_len;
	FILE *f;

	f = fopen("conf.ini", "r");
	assert(((void) "conf.ini doesn't exist", f));
	/*Check usages when adjusting this.*/
	conf_mysql_pw = (conf_mysql_db = (conf_mysql_user = malloc(2048 * 3)) + 2048) + 2048;
	while (fgets(line, sizeof(line) - 1, f)) {
		if (line[0] && line[0] != '\n' && line[0] != '#') {
			val = strstr(line, " ");
			key_len = val - line;
			if (!val || key_len <= 0) {
				val = alloca(1000);
				sprintf(val, "conf.ini: invalid line: %s", line);
				logprintf(val);
				abort();
			}
			if (!memcmp(line, "PHP_PATH", key_len)) {
				target = conf_php_path;
			} else if (!memcmp(line, "MYSQL_USER", key_len)) {
				target = conf_mysql_user;
			} else if (!memcmp(line, "MYSQL_DB", key_len)) {
				target = conf_mysql_db;
			} else if (!memcmp(line, "MYSQL_PW", key_len)) {
				target = conf_mysql_pw;
			} else {
				val = alloca(1000);
				sprintf(val, "conf.ini: unknown config key: %s", line);
				logprintf(val);
				abort();
			}
			val++;
			while (*val != 0 && *val != '\n') {
				*(target++) = *val;
				val++;
			}
			*target = 0;
		}
	}
	fclose(f);
}
