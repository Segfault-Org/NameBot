#include <json-c/json.h>
#include "cmdline.h"

#define READY "READY"
#define CLOSE "CLOSE"

#define AUTH_SEND_UPDATE 1
#define AUTH_LOGIN_REQUIRED 2
#define AUTH_READY 3
#define AUTH_CLOSE 4

int handle_auth(const json_object *obj, json_object **returnValue, int no_prompt, CMDConfig *config);
