#include "GameState.h"
#include "SceneUtil.h"
#include "ScriptEngine.h"
#include "../res/System_res.h"

// Characters inside button box
static const u16 CharMap[2][40] =
{
{   // Upper case
     0x80,  0x88,  0x90,  0x98,  0xA0,  0xA8,  0xB0,  0xB8,  0xC0,  0xC8,   // 0 - 9
    0x108, 0x110, 0x118, 0x120, 0x128, 0x130, 0x138, 0x140, 0x148, 0x150,   // A - J
    0x158, 0x160, 0x168, 0x170, 0x178, 0x180, 0x188, 0x190, 0x198, 0x1A0,   // K - T
    0x1A8, 0x1B0, 0x1B8, 0x1C0, 0x1C8, 0x1D0,   0x0,  0xF8, 0x1F0,   0x0,   // U - Z + Space + ? + Case select + Accept
},
{   // Lower case
     0x80,  0x88,  0x90,  0x98,  0xA0,  0xA8,  0xB0,  0xB8,  0xC0,  0xC8,   // 0 - 9
    0x208, 0x210, 0x218, 0x220, 0x228, 0x230, 0x238, 0x240, 0x248, 0x250,   // a - h
    0x258, 0x260, 0x268, 0x270, 0x278, 0x280, 0x288, 0x290, 0x298, 0x2A0,   // k - t
    0x2A8, 0x2B0, 0x2B8, 0x2C0, 0x2C8, 0x2D0,   0x0,  0xF8, 0x1F0,   0x0,   // u - z + Space + ? + Case select + Accept
}
};

// 3x3 Tile button box
const u32 Box_TileS[] = 
{
    0x02222222, 0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, // Top Left
    0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, // Center Left
    0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, 0x2FFFFFFF, 0x02222222, // Bottom Left

    0x22222222, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, // Top Center
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, // Center
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x22222222, // Bottom Center

    0x22222220, 0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, // Top Right
    0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, // Center Right
    0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, 0xFFFFFFF2, 0x22222220  // Bottom Right
};

#define SPR_START 0x498
static u16 SprBank0;        // Character + Space + Case select + Accept buttons
static u16 SprBank1;        // Textbox with user typed string
static const u8 xOff = 32;
static const u8 yOff = 96;

static u8 xSel = 0; // Selected button X position
static u8 ySel = 0; // Selected button y position
static bool bStringChanged = FALSE; // Redraw user typed string on textbox if true
static bool bSwitchCase = FALSE;
static u8 CurrentCase = 0;          // 0=Upper case, 1=Lower case

static u8 BuffferPos = 0;   // Caret position in InputBuffer string
static u8 InputStart = 0;   // X position where to start drawing user typed string on textbox 
static u8 MaxStrLen;        // Desired lenght of InputBuffer string (Max 32)
char InputBuffer[32] = {'\0'};  // User typed string. To be accessed remotely in other parts

// External background
extern const Image IMG_TextInput;


void DrawInputText(const char *text, u8 yStart)
{
    u8 i = 0;
    while((text[i] != '\0') && (i+yStart < 30))
    {
        VDP_loadTileData(FONT_SCENESH.tiles+((text[i]-32)<<3), ((SPR_START-0x98)+4)+((i+yStart)*3), 1, CPU);    // ...
        i++;
    }
}

void Enter_TextInput(u8 argc, const char *argv[])
{
    const u8 PL = PAL3; // Sprite textbox palette
    const u8 PR = 1;    // Sprite textbox priority

    VDP_setEnable(FALSE);

    VDP_clearPlane(BG_B, TRUE);
    VDP_clearPlane(BG_A, TRUE);

    // Reset variables
    SprBank0 = SPR_START;
    SprBank1 = SPR_START-0x98;
    memset(InputBuffer, '\0', 32);
    BuffferPos = 0;
    CurrentCase = 0;
    xSel = 0;
    ySel = 0;

    VDP_setHilightShadow(1);

    VDP_drawImageEx(BG_B, &IMG_TextInput, TILE_ATTR_FULL(PAL0, 1, 0, 0, 1), 1, 1, TRUE, DMA_QUEUE);
    SYS_doVBlankProcess();

    PAL_setColor(50, 0x666); // Box Outline
    PAL_setColor(60, 0x000); // Text Outline
    PAL_setColor(61, 0xEEE); // Text FG
    PAL_setColor(63, 0xAAA); // Text BG

    // Upload tiles... the dumb way
    for (u8 c = 0; c < 39; c++)
    {
        VDP_loadTileData(Box_TileS, SprBank0+(c*9), 9, DMA_QUEUE);
        VDP_loadTileData(FONT_SCENESH.tiles+((CharMap[0][c])), SprBank0+(c*9)+4, 1, DMA_QUEUE);
    }

    VDP_loadTileSet(&BG_BUTTON_OK, SprBank0+(39*9), DMA_QUEUE);
    VDP_loadTileSet(&BG_TEXTINPUT, SprBank1, DMA_QUEUE);
    SYS_doVBlankProcess();

    // Characters
    VDP_setSpriteFull( 0,   0+xOff,    yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  1); SprBank0 += 0x9;
    VDP_setSpriteFull( 1,  26+xOff,    yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  2); SprBank0 += 0x9;
    VDP_setSpriteFull( 2,  52+xOff,    yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  3); SprBank0 += 0x9;
    VDP_setSpriteFull( 3,  78+xOff,    yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  4); SprBank0 += 0x9;
    VDP_setSpriteFull( 4, 104+xOff,    yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  5); SprBank0 += 0x9;
    VDP_setSpriteFull( 5, 130+xOff,    yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  6); SprBank0 += 0x9;
    VDP_setSpriteFull( 6, 156+xOff,    yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  7); SprBank0 += 0x9;
    VDP_setSpriteFull( 7, 182+xOff,    yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  8); SprBank0 += 0x9;
    VDP_setSpriteFull( 8, 208+xOff,    yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  9); SprBank0 += 0x9;
    VDP_setSpriteFull( 9, 234+xOff,    yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 10); SprBank0 += 0x9;
    
    VDP_setSpriteFull(10,   0+xOff, 26+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 11); SprBank0 += 0x9;
    VDP_setSpriteFull(11,  26+xOff, 26+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 12); SprBank0 += 0x9;
    VDP_setSpriteFull(12,  52+xOff, 26+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 13); SprBank0 += 0x9;
    VDP_setSpriteFull(13,  78+xOff, 26+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 14); SprBank0 += 0x9;
    VDP_setSpriteFull(14, 104+xOff, 26+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 15); SprBank0 += 0x9;
    VDP_setSpriteFull(15, 130+xOff, 26+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 16); SprBank0 += 0x9;
    VDP_setSpriteFull(16, 156+xOff, 26+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 17); SprBank0 += 0x9;
    VDP_setSpriteFull(17, 182+xOff, 26+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 18); SprBank0 += 0x9;
    VDP_setSpriteFull(18, 208+xOff, 26+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 19); SprBank0 += 0x9;
    VDP_setSpriteFull(19, 234+xOff, 26+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 20); SprBank0 += 0x9;
    
    VDP_setSpriteFull(20,   0+xOff, 52+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 21); SprBank0 += 0x9;
    VDP_setSpriteFull(21,  26+xOff, 52+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 22); SprBank0 += 0x9;
    VDP_setSpriteFull(22,  52+xOff, 52+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 23); SprBank0 += 0x9;
    VDP_setSpriteFull(23,  78+xOff, 52+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 24); SprBank0 += 0x9;
    VDP_setSpriteFull(24, 104+xOff, 52+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 25); SprBank0 += 0x9;
    VDP_setSpriteFull(25, 130+xOff, 52+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 26); SprBank0 += 0x9;
    VDP_setSpriteFull(26, 156+xOff, 52+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 27); SprBank0 += 0x9;
    VDP_setSpriteFull(27, 182+xOff, 52+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 28); SprBank0 += 0x9;
    VDP_setSpriteFull(28, 208+xOff, 52+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 29); SprBank0 += 0x9;
    VDP_setSpriteFull(29, 234+xOff, 52+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 30); SprBank0 += 0x9;
    
    VDP_setSpriteFull(30,   0+xOff, 78+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 31); SprBank0 += 0x9;
    VDP_setSpriteFull(31,  26+xOff, 78+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 32); SprBank0 += 0x9;
    VDP_setSpriteFull(32,  52+xOff, 78+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 33); SprBank0 += 0x9;
    VDP_setSpriteFull(33,  78+xOff, 78+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 34); SprBank0 += 0x9;
    VDP_setSpriteFull(34, 104+xOff, 78+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 35); SprBank0 += 0x9;
    VDP_setSpriteFull(35, 130+xOff, 78+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 36); SprBank0 += 0x9;
    VDP_setSpriteFull(36, 156+xOff, 78+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 37); SprBank0 += 0x9;
    VDP_setSpriteFull(37, 182+xOff, 78+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 38); SprBank0 += 0x9;
    VDP_setSpriteFull(38, 208+xOff, 78+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 39); SprBank0 += 0x9;
    VDP_setSpriteFull(39, 234+xOff, 78+yOff, SPRITE_SIZE(3, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 40); SprBank0 += 0x9;

    // Top string box
    VDP_setSpriteFull(40,  32, 32, SPRITE_SIZE(4, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 41); SprBank1 += 0xC;
    VDP_setSpriteFull(41,  64, 32, SPRITE_SIZE(4, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 42); SprBank1 += 0xC;
    VDP_setSpriteFull(42,  96, 32, SPRITE_SIZE(4, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 43); SprBank1 += 0xC;
    VDP_setSpriteFull(43, 128, 32, SPRITE_SIZE(4, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 44); SprBank1 += 0xC;
    VDP_setSpriteFull(44, 160, 32, SPRITE_SIZE(4, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 45); SprBank1 += 0xC;
    VDP_setSpriteFull(45, 192, 32, SPRITE_SIZE(4, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 46); SprBank1 += 0xC;
    VDP_setSpriteFull(46, 224, 32, SPRITE_SIZE(4, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 47); SprBank1 += 0xC;
    VDP_setSpriteFull(47, 256, 32, SPRITE_SIZE(4, 3), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1),  0); SprBank1 += 0xC;

    // Highlight currently selected character box
    VDP_setSpritePalette((ySel*10)+xSel, PAL2);

    if (argc >= 1)
    {
        InputStart = strlen(argv[0])+1;
        DrawInputText(argv[0], 0);
        DrawInputText("_", InputStart);
    }
    if (argc >= 2)
    {
        MaxStrLen = (u8)atoi(argv[1]);
    }

    VDP_setEnable(TRUE);
}

void ReEnter_TextInput()
{
}

void Exit_TextInput(GameState new_state)
{
    PAL_fadeOutAll(3, FALSE);
    VDP_clearPlane(BG_B, TRUE);
    VDP_clearPlane(BG_A, TRUE);
    VDP_setSpriteLink(0, 0);
    VDP_setSpritePosition(0, 32, 320);
    VDP_updateSprites(1, DMA_QUEUE);
    VDP_setHilightShadow(0);
    InputBuffer[BuffferPos] = '\0';
    SYS_doVBlankProcess();
}

void Run_TextInput()
{
    if (bSwitchCase)
    {
        CurrentCase = !CurrentCase;

        for (u8 c = 0; c < 39; c++)
        {
            VDP_loadTileData(FONT_SCENESH.tiles+((CharMap[CurrentCase][c])), SPR_START+(c*9)+4, 1, DMA_QUEUE);
        }

        bSwitchCase = FALSE;
    }

    if (bStringChanged)
    {
        DrawInputText((const char*)InputBuffer, InputStart);
        bStringChanged = FALSE;
    }
}

void Input_TextInput(u16 joy, u16 changed, u16 state)
{
    VDP_setSpritePalette((ySel*10)+xSel, PAL3); // Reset previous selected button palette

    if (changed & state & BUTTON_UP)
    {
        if (ySel == 0) ySel = 3;
        else ySel--;
    }

    if (changed & state & BUTTON_DOWN)
    {
        if (ySel == 3) ySel = 0;
        else ySel++;
    }

    if (changed & state & BUTTON_LEFT)
    {
        if (xSel == 0) xSel = 9;
        else xSel--;
    }

    if (changed & state & BUTTON_RIGHT)
    {
        if (xSel == 9) xSel = 0;
        else xSel++;
    }

    if (changed & state & BUTTON_A)
    {
        if ((ySel*10)+xSel == 38) bSwitchCase = TRUE;   // ^ button - Select case
        else if ((ySel*10)+xSel == 37) {}               // ? button - ?
        else if ((ySel*10)+xSel == 39) RevertState();   // Ok button - Accept and return
        else if (BuffferPos < MaxStrLen)                // All other buttons
        {
            InputBuffer[BuffferPos++] = (CharMap[CurrentCase][(ySel*10)+xSel] >> 3) + 32; // Insert currently selected character into string
            if (BuffferPos < MaxStrLen) InputBuffer[BuffferPos]   = '_';  // Advance caret, but only if there is empty space in string

            bStringChanged = TRUE;
        }
    }

    // Backspace shortcut
    if (changed & state & BUTTON_B)
    {
        if (BuffferPos > 0)
        {
            InputBuffer[BuffferPos] = ' ';
            BuffferPos--;
            InputBuffer[BuffferPos] = '_';

            bStringChanged = TRUE;
        }
    }

    // Case selection shortcut
    if (changed & state & BUTTON_C)
    {
        bSwitchCase = TRUE;
    }

    VDP_setSpritePalette((ySel*10)+xSel, PAL2); // Set newly selected button palette
}

void VBlank_TextInput()
{
    VDP_updateSprites(48, DMA);
}


const VN_GameState TextInputState = 
{
    Enter_TextInput, ReEnter_TextInput, Exit_TextInput, Run_TextInput, Input_TextInput, NULL, VBlank_TextInput
};

