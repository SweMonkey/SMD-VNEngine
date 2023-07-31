#ifndef SCRIPT_H
#define SCRIPT_H

#include <genesis.h>

#define DEF_MAX_VARNAME 16      // Max lenght of a variable name
#define DEF_MAX_VARVALUE 48     // Max lenght of a variable value (as characters)

typedef struct s_Integer
{
    char name[DEF_MAX_VARNAME];
    char value[DEF_MAX_VARVALUE];
} ScriptVar;

s32 atoi(const char *str);
char *itoa(char *dest, s32 i);

void RunScript(const char *script);
ScriptVar *Script_AddVar(const char *name, const char *value);
void Script_DeleteVariables();
void Script_SetVar(const char *name, const char *value);
u16 Script_GetVar(const char *name, char *ret, u16 len);

#endif // SCRIPT_H
