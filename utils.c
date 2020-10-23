#include "utils.h"

json_object *td_func_close()
{
	json_object *reqRoot = json_object_new_object();
	json_object *type = json_object_new_string("close");
	json_object_object_add(reqRoot, "@type", type);
	return reqRoot;
}

json_object *td_func_set_log_level(int level)
{
	json_object *reqRoot = json_object_new_object();
	json_object *pLevel = json_object_new_int(level);
	json_object *type = json_object_new_string("setLogVerbosityLevel");
	json_object_object_add(reqRoot, "@type", type);
	json_object_object_add(reqRoot, "new_verbosity_level", pLevel);
	return reqRoot;
}

