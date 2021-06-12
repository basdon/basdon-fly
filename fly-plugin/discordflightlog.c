/*Runs fly-web/cli/postdiscordflightlog.php that will post a discord message with finished flight details.*/

static char dfl_enabled;

static
void discordflightlog_execve(char *script, char *arg)
{
	char *argv[4];

	chdir("../fly-web/cli");
	argv[0] = conf_php_path;
	argv[1] = script;
	argv[2] = arg;
	argv[3] = 0;
	execve(conf_php_path, argv, environ);
	logprintf("discordflightlog: failed to execve: %d", errno);
}

static
void discordflightlog_init()
{
	struct sigaction action;
	pid_t pid;
	int status;

	if (conf_php_path[0]) {
		pid = fork();
		if (pid == 0) {
			/*Using test.php instead of '-r exit(0);' because then we ensure the chdir() was correct.*/
			discordflightlog_execve("test.php", 0);
			logprintf("discordflightlog: PHP_PATH in conf.ini might be incorrect");
			exit(1);
		} else if (pid == -1) {
			logprintf("discordflightlog: failed to fork to check PHP_PATH: %d", errno);
		}
		waitpid(pid, &status, 0);
		if (WIFEXITED(status)) {
			status = WEXITSTATUS(status);
			if (!status) {
				/*Check OK*/
				/*Do not transform children into zombies when they terminate.*/
				sigemptyset(&action.sa_mask);
				action.sa_handler = SIG_IGN;
				action.sa_flags = 0;
				sigaction(SIGCHLD, &action, NULL);
				return;
			}
			logprintf("discordflightlog: calling test exited %d, disabling", status);
		} else {
			status = WIFSIGNALED(status) ? WTERMSIG(status) : -1;
			logprintf("discordflightlog: calling test signaled %d, disabling", status);
		}
	} else {
		logprintf("discordflightlog: PHP_PATH missing in conf.ini, disabling");
	}
	dfl_enabled = 0;
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
			discordflightlog_execve("postdiscordflightlog.php", missionidbuf);
			exit(1);
		} else if (pid == -1) {
			logprintf("discordflightlog: failed to fork: %d", errno);
		}
	}
}
