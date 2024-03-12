
#include "GameState.h"
#include "../res/system.h"

void Enter_Crash(u8 argc, const char *argv[])
{    
    PAL_setPalette(PAL0, palette_black, CPU);
    PAL_setPalette(PAL1, palette_black, CPU);
    PAL_setPalette(PAL2, palette_black, CPU);
    PAL_setPalette(PAL3, palette_black, CPU);

    VDP_setTextPlane(BG_A);
    VDP_setTextPriority(1);
    VDP_setTextPalette(PAL3);
    VDP_loadFontData(FONT_NORMAL.tiles, 96, DMA);

    PAL_setColor(59, 0xEEE); // Special character
    PAL_setColor(60, 0x000); // Text Outline
    PAL_setColor(61, 0xEEE); // Text FG

    KLog("Entering crash state");

    VDP_drawText("ENGINE CRASHED!", 5, 5);
}

void ReEnter_Crash()
{
}

void Exit_Crash(GameState new_state)
{
}

void Run_Crash()
{
    while (1)
    {
        SYS_doVBlankProcess();
    }
    
}

void Input_Crash(u16 joy, u16 changed, u16 state)
{
}

void VBlank_Crash()
{
}


const VN_GameState CrashState = 
{
    Enter_Crash, ReEnter_Crash, Exit_Crash, Run_Crash, Input_Crash, NULL, VBlank_Crash
};

