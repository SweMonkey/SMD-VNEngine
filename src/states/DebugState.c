
#include "GameState.h"

void Enter_DebugState(u8 argc, const char *argv[])
{
    VDP_drawText("No debug code loaded.", 4, 4);
}

void ReEnter_DebugState()
{
}

void Exit_DebugState()
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
    Enter_DebugState, ReEnter_DebugState, Exit_DebugState, Run_DebugState, Input_DebugState, VBlank_DebugState
};

