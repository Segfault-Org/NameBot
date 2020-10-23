typedef struct cmdconfig
{
	char *selfPath;
	int daemonMode;
	unsigned int continuousModeInterval;
	char *dataDir;
	char *namemodPath;
} CMDConfig;

CMDConfig *parse_config(int argc, char **argv);
