#include "dynmodule.h"
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

NameMod *module_load(const char *filename)
{
	void *soHandle = dlopen(filename, RTLD_LAZY);
	if (!soHandle)
	{
		fprintf(stderr, "Cannot load %s: %s\n", filename, dlerror());
		return NULL;
	}
	dlerror();
	char *err;
	int (*funcGetFirstName)(char *[]) = dlsym(soHandle, "get_first_name");
	err = dlerror();
	if (err != NULL)
	{
		fprintf(stderr, "Cannot load get_first_name: %s\n", err);
		dlclose(soHandle);
		return NULL;
	}
	int (*funcGetLastName)(char *[]) = dlsym(soHandle, "get_last_name");
	err = dlerror();
	if (err != NULL)
	{
		fprintf(stderr, "Cannot load get_last_name: %s\n", err);
		dlclose(soHandle);
		return NULL;
	}
	NameMod *mod = (NameMod*)malloc(sizeof(NameMod));
	mod->soHandle = soHandle;
	mod->funcGetFirstName = funcGetFirstName;
	mod->funcGetLastName = funcGetLastName;
	return mod;
}

const char *module_get_first_name(NameMod *handle)
{
	char *name = NULL;
#ifdef DEBUG
	printf("Name: %p\n", name);
#endif
	int result = (handle->funcGetFirstName)(&name);
#ifdef DEBUG
	printf("Name: %p\n", name);
#endif
	if (result != 0)
	{
		fprintf(stderr, "The module returned an error: %d\n", result);
		if (name != NULL)
			free(name);
		return NULL;
	}
	return name;
}

const char *module_get_last_name(NameMod *handle)
{
	char *name = NULL;
	int result = (handle->funcGetLastName)(&name);
	if (result != 0)
	{
		fprintf(stderr, "The module returned an error: %d\n", result);
		if (name != NULL)
			free(name);
		return NULL;
	}
	return name;
}

void module_close(NameMod *handle)
{
	dlclose(handle->soHandle);
	handle->soHandle = NULL;
	handle->funcGetFirstName = NULL;
	handle->funcGetLastName = NULL;
	free(handle);
}
