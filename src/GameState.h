#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <genesis.h>

typedef enum e_gamestate {GS_Dummy = 0, GS_MainMenu = 1, GS_Options = 2, GS_Scene = 3, GS_DEBUG = 4, GS_CRASH = 5, GS_TEXTINPUT} GameState;

typedef void StateArg_CB(u8 argc, const char *argv[]);
typedef void StateType_CB(GameState new_state);

typedef struct s_gamestate
{
    StateArg_CB *Enter;
    VoidCallback *ReEnter;
    StateType_CB *Exit;

    VoidCallback *Run;
    JoyEventCallback *Input;
    VoidCallback *HBlank;
    VoidCallback *VBlank;
} VN_GameState;

void ChangeState(GameState new_state, u8 argc, const char *argv[]);
void RevertState();
bool isCurrentState(GameState this);
void StateTick();

#endif // GAMESTATE_H
