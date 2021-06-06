/*Runs fly-web/cli/postdiscordflightlog.php that will post a discord message with finished flight details.*/

static char dfl_enabled;

static
int discordflightlog_execve(char *script, char *arg)
{
	char *argv[4];
	int result;

	chdir("../fly-web/cli");
	argv[0] = conf_php_path;
	argv[1] = script;
	argv[2] = arg;
	argv[3] = 0;
	result = execve(conf_php_path, argv, environ);
	if (result == -1) {
		logprintf("discordflightlog: failed to execve: %d", errno);
	}
	return result;
}

static
void discordflightlog_init()
{
	struct sigaction action;
	pid_t pid;

	dfl_enabled = conf_php_path[0];
	if (!dfl_enabled) {
		logprintf("discordflightlog: PHP_PATH missing in conf.ini, disabling");
	} else {
		/*do not transform children into zombies when they terminate*/
		sigemptyset(&action.sa_mask);
		action.sa_handler = SIG_IGN;
		action.sa_flags = 0;
		sigaction(SIGCHLD, &action, NULL);

		pid = fork();
		if (pid == 0) {
			if (discordflightlog_execve("test.php", 0) == -1) {
				logprintf("discordflightlog: PHP_PATH in conf.ini might be incorrect, disabling");
				dfl_enabled = 0;
			}
		} else if (pid == -1) {
			logprintf("discordflightlog: failed to fork to check PHP_PATH: %d", errno);
		}
	}
}

static
void discordflightlog_trigger(int missionid)
{
	char missionidbuf[15];
	pid_t pid;

	if (dfl_enabled) {
		pid = fork();
		if (pid == 0) {
			sprintf(missionidbuf, "%d", missionid);
			if (discordflightlog_execve("postdiscordflightlog.php", missionidbuf) == -1) {
				exit(1);
			}
		} else if (pid == -1) {
			logprintf("discordflightlog: failed to fork: %d", errno);
		}
	}
}
