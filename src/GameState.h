#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <genesis.h>

typedef enum e_gamestate {GS_Dummy = 0, GS_MainMenu = 1, GS_Options = 2, GS_Scene = 3, GS_DEBUG, GS_CRASH} GameState;

typedef void StateArg_CB(u8 argc, char *argv[]);

typedef struct s_gamestate
{
    StateArg_CB *Enter;
    VoidCallback *ReEnter;  // Return to this context
    VoidCallback *Exit;

    VoidCallback *Run;
    JoyEventCallback *Input;
    VoidCallback *VBlank;
} VN_GameState;

void ChangeState(GameState new_state, u8 argc, char *argv[]);
void RevertState();
bool isCurrentState(GameState this);
void StateTick();

#endif // GAMESTATE_H
