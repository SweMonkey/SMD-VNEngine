#include "GameState.h"
#include "SceneUtil.h"
#include "../res/System_res.h"

// External main menu background
extern const Image IMG_Title;

static u8 ySel = 0, ySelPrev = 0;
static void SetupState();
static void SetSelection();


void Enter_MainMenu(u8 argc, const char *argv[])
{
    #ifdef DEBUG_STATE_MSG
    KLog("Entering main menu");
    #endif

    VDP_setEnable(FALSE);

    SetupState();

    VDP_setEnable(TRUE);

    return;
}

void ReEnter_MainMenu()
{
    #ifdef DEBUG_STATE_MSG
    KLog("RE Entering main menu");
    #endif

    SetupState();

    return;
}

void Exit_MainMenu(GameState new_state)
{
    VDP_setEnable(FALSE);

    VDP_clearPlane(BG_B, TRUE);
    VDP_clearPlane(BG_A, TRUE);

    VDP_setSpriteLink(0, 0);
    VDP_setSpritePosition(0, -32, -32);
    VDP_updateSprites(1, CPU);

    VDP_setEnable(TRUE);

    return;
}

void Run_MainMenu()
{
    return;
}

void Input_MainMenu(u16 joy, u16 changed, u16 state)
{
    if (changed & state & BUTTON_UP)
    {
        ySelPrev = ySel;
        
        if (ySel == 0) ySel = 2;
        else ySel--;

        SetSelection();
    }

    if (changed & state & BUTTON_DOWN)
    {
        ySelPrev = ySel;

        if (ySel == 2) ySel = 0;
        else ySel++;

        SetSelection();
    }

    if (changed & state & BUTTON_A)
    {
        switch (ySel)
        {
            case 0: // New game
                PAL_fadeOut(0, 63, 20, FALSE);
                VDP_setHilightShadow(FALSE);

                waitMs(1000);

                ChangeState(GS_Scene, 0, NULL);
            break;

            case 1: // Load game
            break;

            case 2: // Options
                PAL_fadeOut(0, 63, 20, FALSE);
                VDP_setHilightShadow(FALSE);

                ChangeState(GS_Options, 0, NULL);
            break;

            default:
            break;
        }
    }

    return;
}

void VBlank_MainMenu()
{
    return;
}


static void SetupState()
{
    u16 SprBank0 = SPR_BANK0; // $D000 - $DFFF
    const u8 PL = PAL3;       // Sprite textbox palette
    const u8 PR = 1;          // Sprite textbox priority
    const s16 xPos = 124;     // Sprite box y postion
    const s16 yPos = 168;     // Sprite box y postion

    VDP_setEnable(FALSE);

    ySel = 0;
    ySelPrev = 0;

    VDP_setSpriteFull( 0, xPos   , yPos   , SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 1 ); SprBank0 += 0x4;
    VDP_setSpriteFull( 1, xPos+32, yPos   , SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 2 ); SprBank0 += 0x4;
    VDP_setSpriteFull( 2, xPos+64, yPos   , SPRITE_SIZE(2, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 3 ); SprBank0 += 0x2;

    VDP_setSpriteFull( 3, xPos   , yPos+8 , SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0 + 72), 4 ); SprBank0 += 0x4;
    VDP_setSpriteFull( 4, xPos+32, yPos+8 , SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0 + 76), 5 ); SprBank0 += 0x4;
    VDP_setSpriteFull( 5, xPos+64, yPos+8 , SPRITE_SIZE(2, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0 + 80), 6 ); SprBank0 += 0x2;
    
    VDP_setSpriteFull( 6, xPos   , yPos+16, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 7 ); SprBank0 += 0x4;
    VDP_setSpriteFull( 7, xPos+32, yPos+16, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 8 ); SprBank0 += 0x4;
    VDP_setSpriteFull( 8, xPos+64, yPos+16, SPRITE_SIZE(2, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 9 ); SprBank0 += 0x2;
    
    VDP_setSpriteFull( 9, xPos   , yPos+24, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 10); SprBank0 += 0x4;
    VDP_setSpriteFull(10, xPos+32, yPos+24, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 11); SprBank0 += 0x4;
    VDP_setSpriteFull(11, xPos+64, yPos+24, SPRITE_SIZE(2, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 12); SprBank0 += 0x2;

    VDP_setSpriteFull(12, xPos   , yPos+32, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 13); SprBank0 += 0x4;
    VDP_setSpriteFull(13, xPos+32, yPos+32, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 14); SprBank0 += 0x4;
    VDP_setSpriteFull(14, xPos+64, yPos+32, SPRITE_SIZE(2, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 0 ); SprBank0 += 0x2;

    SetSelection();

    VDP_loadTileData(BG_MAIN_MENU.tiles, SPR_BANK0, 80, DMA_QUEUE);

    PAL_setColors(0, palette_black, 64, DMA);

    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    VDP_drawImageEx(BG_B, &IMG_Title, TILE_ATTR_FULL(PAL0, 1, 0, 0, 1), 1, 1, FALSE, DMA_QUEUE);

    VDP_setHilightShadow(TRUE);
    SYS_doVBlankProcess();

    VDP_setEnable(TRUE);

    PAL_setColor(59, 0xEEE); // Special character
    PAL_setColor(60, 0x000); // Text Outline
    PAL_setColor(61, 0xEEE); // Text FG

    PAL_fadeIn(0, 47, IMG_Title.palette->data, 20, FALSE);
}

static void SetSelection()
{
    u8 in = (ySel * 3);      // Sprite index current selection
    u8 tn = (ySel * 10);     // Sprite tile current selection
    u8 ip = (ySelPrev * 3);  // Sprite index previous selection
    u8 tp = (ySelPrev * 10); // Sprite tile previous selection

    // Previous selection
    VDP_setSpriteTile(ip+3, SPR_BANK0 + 10 + tp);
    VDP_setSpriteTile(ip+4, SPR_BANK0 + 14 + tp);
    VDP_setSpriteTile(ip+5, SPR_BANK0 + 18 + tp);

    // Current selection
    VDP_setSpriteTile(in+3, SPR_BANK0 + 50 + tn);
    VDP_setSpriteTile(in+4, SPR_BANK0 + 54 + tn);
    VDP_setSpriteTile(in+5, SPR_BANK0 + 58 + tn);

    VDP_updateSprites(15, DMA_QUEUE);
}


const VN_GameState MainMenuState = 
{
    Enter_MainMenu, ReEnter_MainMenu, Exit_MainMenu, Run_MainMenu, Input_MainMenu, NULL, VBlank_MainMenu
};

