#include "GameState.h"


void Enter_DebugState(u8 argc, const char *argv[])
{
}

void ReEnter_DebugState()
{
}

void Exit_DebugState(GameState new_state)
{
}

void Run_DebugState()
{
}

void Input_DebugState(u16 joy, u16 changed, u16 state)
{
}

void VBlank_DebugState()
{
}


const VN_GameState DebugStateState = 
{
    Enter_DebugState, ReEnter_DebugState, Exit_DebugState, Run_DebugState, Input_DebugState, NULL, VBlank_DebugState
};
