
#include "GameState.h"

void Enter_Dummy(u8 argc, char *argv[])
{
}

void ReEnter_Dummy()
{
}

void Exit_Dummy()
{
}

void Run_Dummy()
{
}

void Input_Dummy(u16 joy, u16 changed, u16 state)
{
}

void VBlank_Dummy()
{
}


const VN_GameState DummyState = 
{
    Enter_Dummy, ReEnter_Dummy, Exit_Dummy, Run_Dummy, Input_Dummy, VBlank_Dummy
};

