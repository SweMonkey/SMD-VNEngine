
#include "GameState.h"

void Enter_Dummy(u8 argc, const char *argv[])
{
}

void ReEnter_Dummy()
{
}

void Exit_Dummy(GameState new_state)
{
}

void Run_Dummy()
{
}

void Input_Dummy(u16 joy, u16 changed, u16 state)
{
}

HINTERRUPT_CALLBACK HBlank_Dummy()
{
}

void VBlank_Dummy()
{
}


const VN_GameState DummyState = 
{
    Enter_Dummy, ReEnter_Dummy, Exit_Dummy, Run_Dummy, Input_Dummy, HBlank_Dummy, VBlank_Dummy
};

