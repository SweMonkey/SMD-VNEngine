#include "GameState.h"
#include "../res/System_res.h"


void Enter_Crash(u8 argc, const char *argv[])
{    
    #ifdef DEBUG_STATE_MSG
    KLog("Entering crash state");
    #endif

    VDP_setEnable(FALSE);

    VDP_setHilightShadow(FALSE);
    
    PAL_setColors(1, palette_black, 62, DMA);
    PAL_setColor( 0, 0xCC0); // BG colour
    PAL_setColor(59, 0xEEE); // Special character
    PAL_setColor(60, 0x000); // Text Outline
    PAL_setColor(61, 0xEEE); // Text FG

    VDP_setTextPlane(BG_A);
    VDP_setTextPriority(1);
    VDP_setTextPalette(PAL3);
    VDP_loadFontData(FONT_NORMAL.tiles, 96, DMA);

    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    VDP_drawText("VN ENGINE CRASHED!", 5, 5);

    if (argc > 0)
    {        
        VDP_drawText("Crash reason:", 5, 7);
        VDP_drawText(argv[0], 5, 8);
    }

    VDP_setEnable(TRUE);

    while (1)
    {
        SYS_doVBlankProcess();
    }    
}


const VN_GameState CrashState = 
{
    Enter_Crash, NULL, NULL, NULL, NULL, NULL, NULL
};

