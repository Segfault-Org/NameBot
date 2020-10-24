#include "auth.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PHONE_NUMBER_MAX_LENGTH 20 // + ___ _______________ (NUL)

int handle_auth(const json_object *obj, json_object **returnValue, int no_prompt, CMDConfig *config)
{
	json_object *authorizationState = json_object_object_get(obj, "authorization_state");
	json_object *authStateType = json_object_object_get(authorizationState, "@type");
	const char *state = json_object_get_string(authStateType);
#ifdef DEBUG
	printf("%s\n", state);
#endif
	if (!strcmp(state, "authorizationStateWaitTdlibParameters"))
	{
		json_object *reqRoot = json_object_new_object();
		json_object *parametersObj = json_object_new_object();
		json_object *pApiHash = json_object_new_string(config->apiHash);
		json_object_object_add(parametersObj, "api_hash", pApiHash);
		json_object *pApiCode = json_object_new_int(config->apiId);
		json_object_object_add(parametersObj, "api_id", pApiCode);
		json_object *pVersion = json_object_new_string("VERSION");
		json_object_object_add(parametersObj, "application_version", pVersion);
		json_object *pDir = json_object_new_string(config->dataDir);
		json_object_object_add(parametersObj, "database_directory", pDir);
		json_object *pDev = json_object_new_string("PC");
		json_object_object_add(parametersObj, "device_model", pDev);
		json_object *pLang = json_object_new_string(getenv("LANG"));
		json_object_object_add(parametersObj, "system_language_code", pLang);

		json_object *type = json_object_new_string("setTdlibParameters");
		json_object_object_add(reqRoot, "@type", type);
		json_object_object_add(reqRoot, "parameters", parametersObj);
		*returnValue = reqRoot;
		return AUTH_SEND_UPDATE;
	}
	else if (!strcmp(state, "authorizationStateWaitEncryptionKey"))
	{
		json_object *reqRoot = json_object_new_object();

		json_object *type = json_object_new_string("checkDatabaseEncryptionKey");
		json_object_object_add(reqRoot, "@type", type);
		*returnValue = reqRoot;
		return AUTH_SEND_UPDATE;
	}
	else if (!strcmp(state, "authorizationStateWaitPhoneNumber"))
	{
		if (no_prompt)
		{
			return AUTH_LOGIN_REQUIRED;
		}
		printf("Phone Number: ");
		char *phoneNumberRaw = calloc(PHONE_NUMBER_MAX_LENGTH, sizeof(char));
		fgets(phoneNumberRaw, PHONE_NUMBER_MAX_LENGTH, stdin);
		strtok(phoneNumberRaw, "\n");

		json_object *reqRoot = json_object_new_object();

		json_object *pPhoneNumber = json_object_new_string(phoneNumberRaw);

		json_object *type = json_object_new_string("setAuthenticationPhoneNumber");
		json_object_object_add(reqRoot, "@type", type);
		json_object_object_add(reqRoot, "phone_number", pPhoneNumber);
		*returnValue = reqRoot;
		return AUTH_SEND_UPDATE;
	}
	else if (!strcmp(state, "authorizationStateWaitCode"))
	{
		if (no_prompt)
		{
			return AUTH_LOGIN_REQUIRED;
		}
		printf("Verify Code: ");
		char *verifyCodeRaw = calloc(6, sizeof(char));
		fgets(verifyCodeRaw, 6, stdin);
		strtok(verifyCodeRaw, "\n");

		json_object *reqRoot = json_object_new_object();

		json_object *pVerifyCode = json_object_new_string(verifyCodeRaw);

		json_object *type = json_object_new_string("checkAuthenticationCode");
		json_object_object_add(reqRoot, "@type", type);
		json_object_object_add(reqRoot, "code", pVerifyCode);
		*returnValue = reqRoot;
		return AUTH_SEND_UPDATE;
	}
	else if (!strcmp(state, "authorizationStateWaitPassword"))
	{
		if (no_prompt)
		{
			return AUTH_LOGIN_REQUIRED;
		}
		printf("Password: ");
		char *pwdRaw = calloc(1025, sizeof(char));
		fgets(pwdRaw, 1025, stdin);
		strtok(pwdRaw, "\n");

		json_object *reqRoot = json_object_new_object();

		json_object *pPwd = json_object_new_string(pwdRaw);

		json_object *type = json_object_new_string("checkAuthenticationPassword");
		json_object_object_add(reqRoot, "@type", type);
		json_object_object_add(reqRoot, "password", pPwd);
		*returnValue = reqRoot;
		return AUTH_SEND_UPDATE;
	}
	else if (!strcmp(state, "authorizationStateReady"))
	{
		return AUTH_READY;
	}
	else if (!strcmp(state, "authorizationStateClosed"))
	{
		return AUTH_CLOSE;
	}
	else
	{
		return 0;
	}
	return 0;
}
