typedef struct namemod
{
	void *soHandle;
	int (*funcGetFirstName)(char *[]);
	int (*funcGetLastName)(char *[]);
} NameMod;

NameMod *module_load(const char *filename);
const char *module_get_first_name(NameMod *handle);
const char *module_get_last_name(NameMod *handle);
void module_close(NameMod *handle);
