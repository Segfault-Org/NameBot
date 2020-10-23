#include "dynmodule_client.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define FIRST_NAME "TestName"

int get_first_name(char *ptr[])
{
	*ptr = strdup(FIRST_NAME);
#ifdef DEBUG
	printf("GetFirstName: %p %s\n", ptr, *ptr);
#endif
	return 0;
}

int get_last_name(char *ptr[])
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	unsigned long long millisecondsSinceEpoch =
    (unsigned long long)(tv.tv_sec) * 1000 +
    (unsigned long long)(tv.tv_usec) / 1000;

	*ptr = calloc(65, sizeof(char));
	sprintf(*ptr, "@UNIX: %llu", millisecondsSinceEpoch);
	return 0;
}
