#include "cmdline.h"
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

CMDConfig *_parse_config(int argc, char **argv, CMDConfig **config);

CMDConfig *parse_config(int argc, char **argv)
{
	CMDConfig *config = (CMDConfig*)malloc(sizeof(CMDConfig));
	if (_parse_config(argc, argv, &config) != NULL)
		return config;
	else
	{
		free(config);
		return NULL;
	}
}

CMDConfig *_parse_config(int argc, char **argv, CMDConfig **conf)
{
	CMDConfig *config = *conf;
	config->selfPath = NULL;
	config->daemonMode = 0;
	config->continuousModeInterval = 0;
	config->dataDir = getenv("DATA_DIR");
	config->namemodPath = getenv("NAME_MOD");
	char *endptr = NULL;
    long val;
	const char *rawApiId = getenv("API_ID");
	if (rawApiId == NULL)
	{
		fprintf(stderr, "API_ID variable must be supplied.\n");
		return NULL;
	}
	val = strtol(rawApiId, &endptr, 10);
	if (endptr == rawApiId)
	{
		fprintf(stderr, "API_ID must be a number.\n");
		return NULL;
	}
	if (val < INT_MIN || val > INT_MAX)
	{
		fprintf(stderr, "API_ID must be an integer.\n");
		return NULL;
	}
	config->apiId = val;
	const char *apiHash = getenv("API_HASH");
	if (apiHash == NULL)
	{
		fprintf(stderr, "API_HASH variable must be supplied.\n");
		return NULL;
	}
	config->apiHash = (char*)apiHash;
	for (int i = 0; i < argc; i ++)
	{
#ifdef DEBUG
		printf("Parsing %s\n", argv[i]);
#endif
		if (i == 0)
		{
			config->selfPath = argv[i];
			continue;
		}
		// Get data dir
		if (!strcmp(argv[i], "-p"))
		{
			if (i != (argc - 1))
			{
				config->dataDir = argv[++i];
			}
			else
			{
				fprintf(stderr, "-p must be followed by the data dir.\n");
				return NULL;
			}
			continue;
		}
		// Get name mod
		if (!strcmp(argv[i], "-n"))
		{
			if (i != (argc - 1))
			{
				config->namemodPath = argv[++i];
			}
			else
			{
				fprintf(stderr, "-n must be followed by the name mod.\n");
				return NULL;
			}
			continue;
		}
		// Check if we are running in a daemon
		if (!strcmp(argv[i], "-d"))
		{
			config->daemonMode = 1;
			continue;
		}
		// Check if we are running in continuous mode
		if (!strcmp(argv[i], "-c"))
		{
			if (i != (argc - 1))
			{
				endptr = NULL;
				val = strtol(argv[++i], &endptr, 10);
				if (endptr == argv[i])
				{
					fprintf(stderr, "-c must be followed by the amount of seconds to wait.\n");
					return NULL;
				}
				if (val <= 0 || val > UINT_MAX)
				{
					fprintf(stderr, "The seconds specified must be from 1 to %u\n", UINT_MAX);
					return NULL;
				}
				config->continuousModeInterval = val;
			}
			else
			{
				fprintf(stderr, "-c must be followed by the amount of seconds to wait.\n");
				return NULL;
			}
			continue;
		}
	}
#ifdef DEBUG
	printf("Current data dir: %s\n", config->dataDir);
	printf("Current name mod: %s\n", config->namemodPath);
#endif
	if (config->dataDir == NULL)
	{
		fprintf(stderr, "Either DATA_DIR variable or -p must be specified.\n");
		return NULL;
	}
	if (config->namemodPath == NULL)
	{
		fprintf(stderr, "Either NAME_MOD variable or -n must be specified.\n");
		return NULL;
	}
	return config;
}
