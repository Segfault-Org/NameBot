#include <stdio.h>
#include <string.h>
#include <td/telegram/td_json_client.h>
#include <json-c/json.h>
#include <limits.h>
#include "auth.h"
#include <sys/time.h>
#include <unistd.h>
#include "utils.h"
#include "dynmodule.h"

#define WAIT_TIMEOUT 10.0

#define PROCESS_VOID 0
#define PROCESS_SEND_UPDATE 1
#define PROCESS_AUTH_READY 2
#define PROCESS_TD_CLOSE 3
#define PROCESS_RUN_BEGIN_AGAIN 4

int process_update(json_object *obj, json_object **returnValue);
void begin(void *td);

void *td;

// We have to make sure that auth is ready
// before doing real stuff (begin())
// Some auth functions will return OK after begin is received.
int updatesSent = 0;
int okReceived = 0;

int authReady = 0;
int beginCalled = 0;
int closeCalled = 0;

CMDConfig *config = NULL;
NameMod *mod = NULL;

int exitCode = 0;

int main(int argc, char **argv)
{
	config = parse_config(argc, argv);
	if (config == NULL)
		return 1;
	mod = module_load(config->namemodPath);
	if (mod == NULL)
	{
		return 2;
	}
	td = td_json_client_create();

	json_object *setLogLevel = td_func_set_log_level(0);
	const char *setLogLevelJ = json_object_to_json_string(setLogLevel);
	td_json_client_execute(td, setLogLevelJ);
	json_object_put(setLogLevel);

	if (td == NULL)
	{
		fprintf(stderr, "Cannot create TDLib client.\n");
		module_close(mod);
		return 1;
	}
	while (1) {
		if (authReady && 
				(updatesSent == okReceived) && 
				!beginCalled && 
				!closeCalled)
		{
			begin(td);
			beginCalled = 1;
			continue;
		}
		const char *result = td_json_client_receive(td, WAIT_TIMEOUT);
  		if (!result) 
		{
			continue;
		}
#ifdef DEBUG
		printf("> %s\n", result);
#endif
		json_object *update = json_tokener_parse(result);
#ifdef DEBUG
		printf("JSON Parsed: %p\n", update);
#endif
		json_object *func = NULL;
		int res = process_update(update, &func);
#ifdef DEBUG
		printf("process_update(): %d\n", res);
#endif
		json_object_put(update);
		update = NULL;
		switch (res)
		{
			case PROCESS_SEND_UPDATE:;
#ifdef DEBUG
				printf("Sending update. Json ptr %p\n", func);
#endif
				char *updateJson = (char*)json_object_to_json_string(func);
#ifdef DEBUG
				printf("< %s\n", updateJson);
#endif
				td_json_client_send(td, updateJson);
				updatesSent++;
				break;
			case PROCESS_TD_CLOSE:
#ifdef DEBUG
				printf("PROCESS_TD_CLOSE\n");
#endif
				td_json_client_destroy(td);
				return exitCode;
			case PROCESS_AUTH_READY:
#ifdef DEBUG
				printf("PROCESS_AUTH_READY\n");
#endif
				authReady = 1;
				break;
			case PROCESS_RUN_BEGIN_AGAIN:
#ifdef DEBUG
				printf("PROCESS_RUN_BEGIN_AGAIN\n");
#endif
				beginCalled = 0;
				break;
			case PROCESS_VOID:
#ifdef DEBUG
				printf("PROCESS_VOID\n");
#endif
				break;
		}
		if (func != NULL)
		{
#ifdef DEBUG
			printf("Free func: %p\n", func);
#endif
			json_object_put(func);
			func = NULL;
		}
	}
	module_close(mod);
	return exitCode;
}

int process_update(json_object *obj, json_object **returnValue)
{
	json_object *typeObj = json_object_object_get(obj, "@type");
	if (typeObj == NULL)
	{
		return PROCESS_VOID;
	}
	
	const char *type = json_object_get_string(typeObj);
#ifdef DEBUG
	printf("%s\n", type);
#endif
	if (!strcmp(type, "updateAuthorizationState"))
	{
		int result = handle_auth(obj, returnValue, config->daemonMode, config);
#ifdef DEBUG
		printf("handle_auth: %o\n", result);
#endif
		switch (result)
		{
			case AUTH_READY:
				return PROCESS_AUTH_READY;
			case AUTH_CLOSE:
				return PROCESS_TD_CLOSE;
			case AUTH_SEND_UPDATE:
				return PROCESS_SEND_UPDATE;
			case AUTH_LOGIN_REQUIRED:
				fprintf(stderr, "Login required. Use '%s' without '-d' to login.\n", config->selfPath);
				closeCalled = 1;
				exitCode = 2;
				*returnValue = td_func_close();
				return PROCESS_SEND_UPDATE;
		}
	}
	else if (!strcmp(type, "ok"))
	{
		okReceived++;
		// This must be the update name call.
		if (beginCalled && !closeCalled)
		{
#ifdef DEBUG
			printf("OK from begin\n");
#endif
			if (config->continuousModeInterval > 0)
			{
				sleep(config->continuousModeInterval);
				return PROCESS_RUN_BEGIN_AGAIN;
			}
			else
			{
				*returnValue = td_func_close();
				return PROCESS_SEND_UPDATE;
			}
		}
		return PROCESS_VOID;
	}
	else if (!strcmp(type, "error"))
	{
		json_object *jCode = json_object_object_get(obj, "code");
		json_object *jMsg = json_object_object_get(obj, "message");
		fprintf(stderr, "Error (%d): %s\n", json_object_get_int(jCode), json_object_get_string(jMsg));

		closeCalled = 1;
		exitCode = 3;
		*returnValue = td_func_close();
		return PROCESS_SEND_UPDATE;
	}
#ifdef DEBUG
	printf("Not handling.\n");
#endif
	return PROCESS_VOID;
}

void begin(void *td)
{
#ifdef DEBUG
	printf("begin()\n");
#endif
	
	const char *first = module_get_first_name(mod);
#ifdef DEBUG
	printf("First: %p %s\n", first, first);
#endif
	const char *last = module_get_last_name(mod);
#ifdef DEBUG
	printf("Last: %p %s\n", last, last);
#endif

	if (first == NULL || !strcmp(first, ""))
	{
		fprintf(stderr, "The mod returned an empty string for first name.\n");
		return;
	}


	json_object *reqRoot = json_object_new_object();
	json_object *pFirstName = json_object_new_string(first);
	json_object_object_add(reqRoot, "first_name", pFirstName);
	if (last != NULL)
	{
		json_object *pLastName = json_object_new_string(last);
		json_object_object_add(reqRoot, "last_name", pLastName);
	}
	json_object *type = json_object_new_string("setName");
	json_object_object_add(reqRoot, "@type", type);
	const char *json = (char*)json_object_to_json_string(reqRoot);

	td_json_client_send(td, json);
	updatesSent++;
#ifdef DEBUG
	printf("Sent update, %s\n", json);
#endif

	json_object_put(reqRoot);
	free((char*)first);
	free((char*)last);
}
