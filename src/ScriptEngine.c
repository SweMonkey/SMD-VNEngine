#include "ScriptEngine.h"
#include "GameState.h"

#define DEF_NUM_OPCODE 13       // Number of opcodes available
#define DEF_MAX_VAR 64          // Number of max variables the VM can hold
#define DEF_MAX_LINEBUFFER 64   // Number of characters the linebuffer can hold

#define SETV 0x0    // Set <v1> to <v2> - <v1> is created if it does not exist - <v2> must exist or be a constant value
#define ADDV 0x1    // Add <v2> to <v1>
#define SUBV 0x2    // Subtract <v2> from <v1>
#define MULV 0x3    // Multiply <v1> with <v2>
#define DIVV 0x4    // Divide <v1> by <v2>
#define GOTO 0x5    // Goto (does nothing)
#define IEQL 0x6    // If <v1> is equal to <v2> set result bit to 1 otherwise result bit to 0     - if true; execute following code block until iend or else opcode
#define INEQ 0x7    // If <v1> is not equal to <v2> set result bit to 1 otherwise result bit to 0 - if true; execute following code block until iend or else opcode
#define IEND 0x8    // End of if block
#define DEFV 0x9    // Same as setv, except it does not touch <v1> if it already exists
#define CATS 0xA    // Concatenate string <v2> onto <v1>
#define DBPR 0xB    // Debug print to emulator log
#define ELSE 0xC    // If previous if statement was false then execute the following code block, otherwise continue execution after the next iend opcode
#define NOPE 0xFF   // Illegal OP

const char *OpCodeStr[DEF_NUM_OPCODE] =
{
    "setv",
    "addv",
    "subv",
    "mulv",
    "divv",
    "goto",
    "ieql",
    "ineq",
    "iend",
    "defv",
    "cats",
    "dbpr",
    "else"
};

#define SET_SR(x) (StatusRegister |= x)
#define UNSET_SR(x) (StatusRegister &= ~x)
#define GET_SR(x) (StatusRegister & x)

#define SR_CMP_SH 0x10  // Compare shift
#define SR_NEW_SH 0x11  // New variable shift
#define SR_SKP_SH 0x12  // Skip next OP shift

#define SR_LOP 0xFF            // Last OP executed (bits 0-7)
#define SR_CMP (1<<SR_CMP_SH)  // Bit 16 = Result of last comparison (boolean)
#define SR_NEW (1<<SR_NEW_SH)  // Bit 17 = 1 if a new variable was created
#define SR_SKP (1<<SR_SKP_SH)  // Bit 18 = Skip next OP if 1

static u32 StatusRegister;
static char *LineStr = NULL;

VN_ScriptVar *VarList[DEF_MAX_VAR];
u8 NumVar = 0;


s32 atoi(const char *str)
{
    s32 res = 0;

    for (s32 i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';

    return res;
}

static char *itoa_helper(char *dest, s32 i)
{
    if (i <= -10) dest = itoa_helper(dest, i/10);

    *dest++ = '0' - i%10;

    return dest;
}

char *itoa(char *dest, s32 i)
{
    char *s = dest;

    if (i < 0) *s++ = '-';
    else i = -i;

    *itoa_helper(s, i) = '\0';

    return dest;
}

// XXXX YY,<ZZ>
void SearchP2(char *line, VN_ScriptVar *vret)
{
    char varname[DEF_MAX_VARVALUE] = {'\0'}; // Needs to be able to contain atleast a 48 char string, unless its a variable name (16 char max)
    u16 i = 5;
    u16 j = 0;

    while (1)
    {
        if (line[i] == ',')
        {
            i++;
            while (line[i] == ' ')
            {
                i++;
                if (i >= DEF_MAX_LINEBUFFER) break;
            }

            break;
        }

        i++;
        if (i >= DEF_MAX_LINEBUFFER) break;
    }

    while (1)
    {
        // Don't allow " in strings unless it is prefixed with \ (escape)
        if ((line[i-1] != '\\') && (line[i] == '\"')) 
        {
            i++;
            continue;
        }

        if ((line[i] != '\n') && (line[i] != ';'))
        {
            varname[j] = line[i];

            i++;
            j++;
            if (i >= DEF_MAX_LINEBUFFER) break;
            if (j >= DEF_MAX_VARVALUE) break;
        }
        else break;
    }

    // Look for var with name varname
    for (u8 n = 0; n < NumVar; n++)
    {
        if (strcmp(VarList[n]->name, varname) == 0)
        {
            strcpy(vret->value, VarList[n]->value);

            return;
        }
    }

    strcpy(vret->value, varname);

    return;
}

// XXXX <YY>,ZZ
VN_ScriptVar *SearchP1(char *line)
{
    char varname[DEF_MAX_VARVALUE] = {'\0'};
    u16 i = 5;
    u16 j = 0;

    while (1)
    {
        if (line[i] != ',')
        {
            varname[j] = line[i];

            i++;
            j++;
            if (i >= DEF_MAX_LINEBUFFER) break;
            if (j >= DEF_MAX_VARVALUE) break;
        }
        else break;
    }

    StatusRegister &= ~(1<<SR_NEW_SH);  // Clear new result bit

    for (u8 n = 0; n < NumVar; n++)
    {
        if (strcmp(VarList[n]->name, varname) == 0)
        {
            return VarList[n];
        }
    }

    if (NumVar >= DEF_MAX_VAR)
    {
        #ifdef DEBUG_STATE_MSG
        KLog("Max integer variables reached.");
        #endif

        return NULL;
    }

    StatusRegister |= 1 << SR_NEW_SH;

    return Script_AddVar(varname, "\0");
}

u16 GetOP(char *line)
{
    u8 i = 0;
    char opstr[5] = {line[0], line[1], line[2], line[3], '\0'};

    while (i < DEF_NUM_OPCODE)
    {
        if (strcmp(opstr, OpCodeStr[i]) == 0)
        {
            return i;
            break;
        }
        i++;
    }

    return NOPE;
}

void ParseLine(char *line)
{
    VN_ScriptVar tmpP2;
    VN_ScriptVar *tmpP1;
    u8 opcode = NOPE;

    opcode = GetOP(line);

    StatusRegister &= ~SR_LOP;
    StatusRegister |= opcode;

    if (opcode == NOPE)
    {
        #ifdef DEBUG_STATE_MSG
        KLog("VM Warning: Trying to run NOPE opcode!");
        #endif
        
        return;
    }

    if ((opcode != GOTO) || (opcode != IEND) || (opcode != ELSE))
    {
        SearchP2(line, &tmpP2);
        tmpP1 = SearchP1(line);

        if (tmpP1 == NULL) return;
    }

    switch (opcode)
    {
    case SETV:
    {
        strcpy(tmpP1->value, tmpP2.value);
        break;
    }
    case ADDV:
    {
        s32 v1 = atoi(tmpP1->value);
        s32 v2 = atoi(tmpP2.value);

        v1 += v2;
        itoa(tmpP1->value, v1);

        break;
    }
    case SUBV:
    {        
        s32 v1 = atoi(tmpP1->value);
        s32 v2 = atoi(tmpP2.value);

        v1 -= v2;
        itoa(tmpP1->value, v1);

        break;
    }
    case MULV:
    {
        s32 v1 = atoi(tmpP1->value);
        s32 v2 = atoi(tmpP2.value);

        v1 *= v2;
        itoa(tmpP1->value, v1);

        break;
    }
    case DIVV:
    {
        s32 v1 = atoi(tmpP1->value);
        s32 v2 = atoi(tmpP2.value);

        v1 /= v2;
        itoa(tmpP1->value, v1);

        break;
    }
    case IEQL:
    {
        u8 cmp = strcmp(tmpP1->value, tmpP2.value) == 0 ? 1 : 0;

        StatusRegister &= ~(1<<SR_CMP_SH);  // Clear cmp result bit
        StatusRegister |= (cmp) << SR_CMP_SH;
        break;
    }
    case INEQ:
    {
        u8 cmp = strcmp(tmpP1->value, tmpP2.value) != 0 ? 1 : 0;

        StatusRegister &= ~(1<<SR_CMP_SH);  // Clear cmp result bit
        StatusRegister |= (cmp) << SR_CMP_SH;
        break;
    }
    case GOTO:
    {
        break;
    }
    case IEND:
    {
        break;
    }
    case DEFV:
    {
        if ((StatusRegister & SR_NEW) == SR_NEW) strcpy(tmpP1->value, tmpP2.value);
        break;
    }
    case CATS:  // meow
    {
        strcat(tmpP1->value, tmpP2.value);
        break;
    }
    case DBPR:
    {
        KLog(tmpP1->value);
        break;
    }
    case ELSE:
    {
        break;
    }

    default:
    {
        KLog_U1("Error handling opcode ", opcode);
        break;
    }
    }

    return;
}

u32 GetLine(const char *str, char *ret, u32 start, u32 end)
{
    u32 i = start;
    u32 j = 0;

    while (1)
    {
        if (str[i] != '\n')
        {
            ret[j] = str[i];
            j++;
        }

        i++;

        if (j >= DEF_MAX_LINEBUFFER) break;

        if (i >= end) break;

        if ((str[i] == '\0') || (str[i] == '\n')) break;
    }

    return i;
}

/// @brief Parse and run script in VM
/// @param script Pointer to a script
void Script_Execute(const char *script)
{
    u32 pos = 0;
    u32 size = strlen(script);

    if (LineStr == NULL) LineStr = (char*)malloc(DEF_MAX_LINEBUFFER);

    while (pos < size)
    {
        memset(LineStr, '\0', DEF_MAX_LINEBUFFER);
        pos = GetLine(script, LineStr, pos, size);

        if (LineStr[0] == '\0') continue;

        // IF (CMP EQUAL OR NOT EQUAL) AND (CMP = FALSE)
        if ( ((GET_SR(SR_LOP) == IEQL) || (GET_SR(SR_LOP) == INEQ)) && (GET_SR(SR_CMP) == 0) )
        {
            SET_SR(SR_SKP);
        }

        if (GetOP(LineStr) == ELSE)
        {
            if (GET_SR(SR_CMP) == SR_CMP) SET_SR(SR_SKP);
            else if (GET_SR(SR_CMP) == 0) UNSET_SR(SR_SKP);
        }

        if (GET_SR(SR_SKP) == 0)
        {
            ParseLine(LineStr);
        }
        else if (GetOP(LineStr) == IEND)
        {
            UNSET_SR(SR_SKP);
        }

        UNSET_SR(SR_LOP);
        SET_SR(GetOP(LineStr));

        //kprintf("VM SR: $%08lX - OP: %s - Skip: %s", StatusRegister, opcodes[GET_SR(SR_LOP)], (GET_SR(SR_SKP) == 0 ? "no":"yes"));
    }

    return;
}

/// @brief Add a variable to the VM (Warning: Does not check for existing variable with the same name)
/// @todo Does not check for existing variable with same name
/// @param name Name of the new variable
/// @param value Value of the new variable
/// @return New pointer to new variable if added or NULL if out of memory slots in VM
VN_ScriptVar *Script_AddVar(const char *name, const char *value)
{
    if (NumVar >= DEF_MAX_VAR-1) return NULL;

    VN_ScriptVar *ptr = (VN_ScriptVar*)malloc(sizeof(VN_ScriptVar));

    strcpy(ptr->name, name);
    strcpy(ptr->value, value);

    VarList[NumVar] = ptr;
    NumVar++;

    return ptr;
}

/// @brief Delete all allocated variables in VM
void Script_DeleteVariables()
{
    for (u8 i = 0; i < NumVar; i++)
    {
        memset(VarList[i]->name, '\0', DEF_MAX_VARNAME);
        memset(VarList[i]->value, '\0', DEF_MAX_VARVALUE);

        free(VarList[i]);
    }

    free(LineStr);
    LineStr = NULL;

    return;
}

/// @brief Set VM variable value. The variable is created if it does not exist.
/// @param name Name of the variable to set
/// @param value New value of variable
void Script_SetVar(const char *name, const char *value)
{
    for (u8 n = 0; n < NumVar; n++)
    {
        if (strcmp(VarList[n]->name, name) == 0)
        {
            strcpy(VarList[n]->name, name);
            strcpy(VarList[n]->value, value);

            return;
        }
    }

    // Variable not found if we're here, create it instead
    Script_AddVar(name, value);

    return;
}

/// @brief Get variable value from VM
/// @param name Name of the variable to get
/// @param ret Recieving string
/// @param len Size of recieving string
/// @return 0 if successful, 1 if variable was not found
u16 Script_GetVar(const char *name, char *ret, u16 len)
{
    for (u8 n = 0; n < NumVar; n++)
    {
        if (strcmp(VarList[n]->name, name) == 0)
        {
            strncpy(ret, VarList[n]->value, len);

            return 0;
        }
    }

    kprintf("Script_GetVar: variable \"%s\" not found.", name);

    return 1;
}
