#include "GameState.h"
#include "SceneUtil.h"
#include "../res/System_res.h"

const u32 CPE_Tile[8] = {0x02222222, 0x2EEEEEEE, 0x2EEEEEEE, 0x2EEEEEEE, 0x2EEEEEEE, 0x2EEEEEEE, 0x2EEEEEEE, 0x2EEEEEEE};   // Color Pick edge tile
const u32 CP_Tile[8]  = {0xEEEEEEEE, 0xEEEEEEEE, 0xEEEEEEEE, 0xEEEEEEEE, 0xEEEEEEEE, 0xEEEEEEEE, 0xEEEEEEEE, 0xEEEEEEEE};   // Color Pick tile

// Saved color picker values when transparent textbox is in use; NOTE: These are inversed!
static u8 tbRed_OLD = 0x7;
static u8 tbGreen_OLD = 0x4;
static u8 tbBlue_OLD = 0x0;

// Previously in function
static u8 sIdx = 0;
static u8 c = 10;
static u8 tdDiv = 1;
static s8 sLR = 0;
static char str[7] = {0};

// RGB color picker values; NOTE: These are inversed!
static u8 tbRed = 0;
static u8 tbGreen = 0;
static u8 tbBlue = 0;

// Forward decl.
void Update_Selection();


void Enter_Options(u8 argc, const char *argv[])
{
    #ifdef DEBUG_STATE_MSG
    KLog("Entering options");
    #endif

    VDP_setEnable(FALSE);

    VDP_setHilightShadow(FALSE);
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    sIdx = 0;

    tdDiv = VNS_TextDelay >> 3;
    tbRed = 7-((VNS_TextBoxColor >> 1) & 7);
    tbGreen = 7-((VNS_TextBoxColor >> 5) & 7);
    tbBlue = 7-(VNS_TextBoxColor >> 9);

    VDP_setTextPlane(BG_A);
    VDP_setTextPriority(1);
    VDP_setTextPalette(PAL3);

    VDP_loadFontData(FONT_NORMAL.tiles, 96, DMA);

    PAL_setColor(50, 0xEEE); // Color picker outline
    PAL_setColor(59, 0xCC0); // Special character
    PAL_setColor(60, 0x000); // Text Outline
    PAL_setColor(61, 0xEEE); // Text FG

    // Draw background
    VDP_drawImageEx(BG_B, &IMG_Options, TILE_ATTR_FULL(PAL2, 0, 0, 0, 1), 0, 0, FALSE, DMA_QUEUE);

    VDP_loadTileData(CPE_Tile, 0x6ED, 1, DMA_QUEUE);
    VDP_loadTileData(CP_Tile, 0x6EE, 1, DMA_QUEUE);

    
    VDP_drawText("Text speed [           ] xx %", 3, 6); //18->27

    // Update text speed value text
    sprintf(str, "%d %c  ", (100-VNS_TextDelay), '%');
    VDP_drawText(str, 28, 6);

    c = 11; while (c--) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, ((11-c) > (tdDiv) ? 0x5FF : 0)), 15+c, 6);


    VDP_drawText("Textbox style", 3, 8);

    if (VNS_TextBoxStyle == 0)
    {
        VDP_drawText("[Transparent] Solid  ", 6, 9);
    }
    else if (VNS_TextBoxStyle == 1)
    {
        VDP_drawText("Transparent [Solid]  ", 6, 9);
    }

    VDP_drawText("Textbox color", 3, 11);
    VDP_drawText("Red   [       ] 0", 6, 12);
    VDP_drawText("Green [       ] 3", 6, 13);
    VDP_drawText("Blue  [       ] 7", 6, 14);

    // Update color value text
    sprintf(str, "%u  ", 7-tbRed);
    VDP_drawText(str, 22, 12);
    sprintf(str, "%u  ", 7-tbGreen);
    VDP_drawText(str, 22, 13);
    sprintf(str, "%u  ", 7-tbBlue);
    VDP_drawText(str, 22, 14);

    // Draw color picker box
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, 0x6ED), 26, 12);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, 0x6EE), 26, 13);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 1, 0, 0x6ED), 26, 14);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, 0x6EE), 27, 12);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, 0x6EE), 27, 13);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, 0x6EE), 27, 14);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 1, 0x6ED), 28, 12);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, 0x6EE), 28, 13);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 1, 1, 0x6ED), 28, 14);

    // Update color picker blocks
    c = 7; while (c--) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, ((7-c) > (tbRed) ? 0x5FF : 0)), 13+c, 12);
    c = 7; while (c--) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, ((7-c) > (tbGreen) ? 0x5FF : 0)), 13+c, 13);
    c = 7; while (c--) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, ((7-c) > (tbBlue) ? 0x5FF : 0)), 13+c, 14);

    VDP_drawText("Return", 3, 16);

    Update_Selection();

    SYS_doVBlankProcess();

    VDP_setEnable(TRUE);

    PAL_fadeIn(32, 47, IMG_Options.palette->data, 20, FALSE);
    
    return;
}

void ReEnter_Options()
{
    #ifdef DEBUG_STATE_MSG
    KLog("RE Entering options");
    #endif

    return;
}

void Exit_Options(GameState new_state)
{
    #ifdef DEBUG_STATE_MSG
    KLog("Exiting options");
    #endif

    PAL_fadeOut(0, 63, 20, FALSE);

    VDP_setEnable(FALSE);

    VDP_clearPlane(BG_B, TRUE);
    VDP_clearPlane(BG_A, TRUE);

    VDP_setEnable(TRUE);
    
    return;
}

void Run_Options()
{
    return;
}

void Input_Options(u16 joy, u16 changed, u16 state)
{
    if (changed & state & BUTTON_UP)
    {
        VDP_drawText(" ", 2, sIdx+6);

        if ((sIdx == 10) && VNS_TextBoxStyle) sIdx = 8;       // Jump to Color picker (blue) if using solid textbox
        else if ((sIdx == 10) && !VNS_TextBoxStyle) sIdx = 3; // Jump to Textbox style selector if using transparent textbox
        else if (sIdx == 6) sIdx = 3;   // Jump to Textbox style selector
        else if (sIdx == 3) sIdx = 0;   // Jump to Textspeed selector
        else if (sIdx == 0) sIdx = 10;  // Jump to bottom choice
        else sIdx--;

        Update_Selection();
    }

    if (changed & state & BUTTON_DOWN)
    {
        VDP_drawText(" ", 2, sIdx+6);

        if (sIdx == 8) sIdx = 10;       // Jump to Return
        else if ((sIdx == 3) && VNS_TextBoxStyle) sIdx = 6;     // Jump to Color picker (red) if using solid textbox
        else if ((sIdx == 3) && !VNS_TextBoxStyle) sIdx = 10;   // Jump to Return if using transparent textbox
        else if (sIdx == 0) sIdx = 3;   // Jump to Textbox style
        else if (sIdx == 10) sIdx = 0;  // Jump to top choice
        else sIdx++;

        Update_Selection();
    }

    if (changed & state & BUTTON_LEFT)
    {
        sLR = -1;

        Update_Selection();
    }
    else if (changed & state & BUTTON_RIGHT)
    {
        sLR = 1;

        Update_Selection();
    }
    else sLR = 0;

    if (changed & state & BUTTON_A)
    {
        if (sIdx == 10) 
        {
            if (GetPreviousState() == GS_INGAME_MENU) ReEnterState(GS_Scene);
            else RevertState();
            
        }
    }

    return;
}

void VBlank_Options()
{
}


void Update_Selection()
{
    switch (sIdx)
    {
        case 0: // Text speed
            if ((sLR==1) && (VNS_TextDelay >= 10)) VNS_TextDelay -= 10;
            else if ((sLR==-1) && (VNS_TextDelay <= 80)) VNS_TextDelay += 10;

            c = 11;
            tdDiv = ((VNS_TextDelay) >> 3);
            while (c--) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, ((11-c) > (tdDiv) ? 0x5FF : 0)), 15+c, 6);//15+c, 6);

            sprintf(str, "%d %c  ", (100-VNS_TextDelay), '%');
            VDP_drawText(str, 28, 6);

            waitMs(33);
        break;

        case 3: // Textbox style

            if ((sLR == -1) && (VNS_TextBoxStyle != 0))
            {
                VNS_TextBoxStyle = 0;
                VDP_drawText("[Transparent] Solid  ", 6, 9);

                // Disable color picker and redraw it
                tbRed_OLD = tbRed;
                tbGreen_OLD = tbGreen;
                tbBlue_OLD = tbBlue;
                tbRed = 6;
                tbGreen = 6;
                tbBlue = 6;

                c = 7; while (c--) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, (((7-c) > (tbRed) ? 0x5FF : 0))), 13+c, 12);
                c = 7; while (c--) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, (((7-c) > (tbGreen) ? 0x5FF : 0))), 13+c, 13);
                c = 7; while (c--) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, (((7-c) > (tbBlue) ? 0x5FF : 0))), 13+c, 14);

                VDP_drawText("1  ", 22, 12);
                VDP_drawText("1  ", 22, 13);
                VDP_drawText("1  ", 22, 14);
            }
            else if ((sLR == 1) && (VNS_TextBoxStyle != 1))
            {
                VNS_TextBoxStyle = 1;
                VDP_drawText("Transparent [Solid]  ", 6, 9);

                // Enable color picker and redraw it
                tbRed = tbRed_OLD;
                tbGreen = tbGreen_OLD;
                tbBlue = tbBlue_OLD;

                c = 7; while (c--) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, ((7-c) > (tbRed) ? 0x5FF : 0)), 13+c, 12);
                c = 7; while (c--) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, ((7-c) > (tbGreen) ? 0x5FF : 0)), 13+c, 13);
                c = 7; while (c--) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, ((7-c) > (tbBlue) ? 0x5FF : 0)), 13+c, 14);

                sprintf(str, "%u  ", 7-tbRed);
                VDP_drawText(str, 22, 12);
                sprintf(str, "%u  ", 7-tbGreen);
                VDP_drawText(str, 22, 13);
                sprintf(str, "%u  ", 7-tbBlue);
                VDP_drawText(str, 22, 14);
            }
        break;

        case 6: // Textbox red color

            if ((sLR==1) && (tbRed >= 1)) tbRed -= 1;
            else if ((sLR==-1) && (tbRed <= 6)) tbRed += 1;

            c = 7;
            while (c--) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, ((7-c) > (tbRed) ? 0x5FF : 0)), 13+c, 12);

            sprintf(str, "%u  ", 7-tbRed);
            VDP_drawText(str, 22, 12);

            waitMs(33);
        break;

        case 7: // Textbox green color

            if ((sLR==1) && (tbGreen >= 1)) tbGreen -= 1;
            else if ((sLR==-1) && (tbGreen <= 6)) tbGreen += 1;

            c = 7;
            while (c--) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, ((7-c) > (tbGreen) ? 0x5FF : 0)), 13+c, 13);

            sprintf(str, "%u  ", 7-tbGreen);
            VDP_drawText(str, 22, 13);

            waitMs(33);
        break;

        case 8: // Textbox blue color

            if ((sLR==1) && (tbBlue >= 1)) tbBlue -= 1;
            else if ((sLR==-1) && (tbBlue <= 6)) tbBlue += 1;

            c = 7;
            while (c--) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 0, 0, 0, ((7-c) > (tbBlue) ? 0x5FF : 0)), 13+c, 14);

            sprintf(str, "%u  ", 7-tbBlue);
            VDP_drawText(str, 22, 14);

            waitMs(33);
        break;

        case 10: // Return
        break;

        default:
        break;
    }

    VDP_drawText(">", 2, sIdx+6);

    // Update color picker box
    VNS_TextBoxColor = (((7-tbBlue) << 1)+1) << 8;
    VNS_TextBoxColor |= (((7-tbGreen) << 1)+1) << 4;
    VNS_TextBoxColor |= (((7-tbRed) << 1)+1);

    PAL_setColor(62, VNS_TextBoxColor);
}


const VN_GameState OptionsState = 
{
    Enter_Options, ReEnter_Options, Exit_Options, Run_Options, Input_Options, NULL, VBlank_Options
};

