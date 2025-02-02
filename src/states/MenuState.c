#include "GameState.h"
#include "SceneUtil.h"
#include "SceneFX.h"
#include "../res/System_res.h"

extern VN_GameState SceneState;
void ReEnter_Scene();

static u8 ySel = 0, ySelPrev = 0;
static void SetupState();
static void SetSelection();
static void ResetScroll();


void Enter_Menu(u8 argc, const char *argv[])
{
    SetupState();
}

void ReEnter_Menu()
{
    SetupState();
}

void Exit_Menu(GameState new_state)
{
    VDP_setSpriteLink(0, 0);
    VDP_setSpritePosition(0, -32, -32);
    VDP_updateSprites(1, CPU);
}

void Run_Menu()
{
    FX_RunEffect();
}

void Input_Menu(u16 joy, u16 changed, u16 state)
{
    if (changed & state & BUTTON_UP)
    {
        ySelPrev = ySel;
        
             if (ySel == 0) ySel = 5;
        else if (ySel == 5) ySel = 3;
        else ySel--;

        SetSelection();
    }

    if (changed & state & BUTTON_DOWN)
    {
        ySelPrev = ySel;

             if (ySel == 5) ySel = 0;
        else if (ySel == 3) ySel = 5;
        else ySel++;

        SetSelection();
    }

    if (changed & state & BUTTON_A)
    {
        switch (ySel)
        {
            case 2: // Options
                PAL_fadeOutAll(20, FALSE);
                VDP_setHilightShadow(FALSE);
                SceneState.ReEnter = ReEnter_Scene;
                ResetScroll();
                ChangeState(GS_Options, 0, NULL);
            break;

            case 3: // Return
                ReEnterState(GS_Scene);
            break;

            case 5: // Quit
                PAL_fadeOutAll(20, FALSE);
                VDP_setHilightShadow(FALSE);
                SceneState.ReEnter = ReEnter_Scene;
                FX_ResetEffect();
                waitMs(1000);
                ChangeState(GS_MainMenu, 0, NULL);
            break;

            case 0: // Save
            case 1: // Load
            default:
            break;
        }
    }
}

void VBlank_Menu()
{
    FX_UpdateScroll();
}


static void SetupState()
{
    u16 SprBank0 = SPR_BANK0; // $D000 - $DFFF
    const u8 PL = PAL3;       // Sprite textbox palette
    const u8 PR = 1;          // Sprite textbox priority
    const s8 xPos = 124;      // Sprite box y postion
    const s8 yPos = 80;       // Sprite box y postion

    ySel = 0;
    ySelPrev = 0;

    VDP_setSpriteFull( 0, xPos   , yPos   , SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 1 ); SprBank0 += 0x4;
    VDP_setSpriteFull( 1, xPos+32, yPos   , SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 2 ); SprBank0 += 0x4;
    VDP_setSpriteFull( 2, xPos+64, yPos   , SPRITE_SIZE(1, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 3 ); SprBank0 += 0x1;

    VDP_setSpriteFull( 3, xPos   , yPos+8 , SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0 + 72), 4 ); SprBank0 += 0x4;
    VDP_setSpriteFull( 4, xPos+32, yPos+8 , SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0 + 76), 5 ); SprBank0 += 0x4;
    VDP_setSpriteFull( 5, xPos+64, yPos+8 , SPRITE_SIZE(1, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0 + 80), 6 ); SprBank0 += 0x1;
    
    VDP_setSpriteFull( 6, xPos   , yPos+16, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 7 ); SprBank0 += 0x4;
    VDP_setSpriteFull( 7, xPos+32, yPos+16, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 8 ); SprBank0 += 0x4;
    VDP_setSpriteFull( 8, xPos+64, yPos+16, SPRITE_SIZE(1, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 9 ); SprBank0 += 0x1;
    
    VDP_setSpriteFull( 9, xPos   , yPos+24, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 10); SprBank0 += 0x4;
    VDP_setSpriteFull(10, xPos+32, yPos+24, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 11); SprBank0 += 0x4;
    VDP_setSpriteFull(11, xPos+64, yPos+24, SPRITE_SIZE(1, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 12); SprBank0 += 0x1;

    VDP_setSpriteFull(12, xPos   , yPos+32, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 13); SprBank0 += 0x4;
    VDP_setSpriteFull(13, xPos+32, yPos+32, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 14); SprBank0 += 0x4;
    VDP_setSpriteFull(14, xPos+64, yPos+32, SPRITE_SIZE(1, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 15); SprBank0 += 0x1;

    VDP_setSpriteFull(15, xPos   , yPos+40, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 16); SprBank0 += 0x4;
    VDP_setSpriteFull(16, xPos+32, yPos+40, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 17); SprBank0 += 0x4;
    VDP_setSpriteFull(17, xPos+64, yPos+40, SPRITE_SIZE(1, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 18); SprBank0 += 0x1;
    
    VDP_setSpriteFull(18, xPos   , yPos+48, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 19); SprBank0 += 0x4;
    VDP_setSpriteFull(19, xPos+32, yPos+48, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 20); SprBank0 += 0x4;
    VDP_setSpriteFull(20, xPos+64, yPos+48, SPRITE_SIZE(1, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 21); SprBank0 += 0x1;
    
    VDP_setSpriteFull(21, xPos   , yPos+56, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 22); SprBank0 += 0x4;
    VDP_setSpriteFull(22, xPos+32, yPos+56, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 23); SprBank0 += 0x4;
    VDP_setSpriteFull(23, xPos+64, yPos+56, SPRITE_SIZE(1, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 0 ); SprBank0 += 0x1;

    SetSelection();

    PAL_setColor(59, 0xEEE); // Teal: 0xEE0 - Yellow greenish: 0xEA

    VDP_loadTileData(BG_INGAME_MENU.tiles, SPR_BANK0, 126, DMA_QUEUE);
    SYS_doVBlankProcess();
}

static void SetSelection()
{
    u8 in = (ySel * 3);     // Sprite index current selection
    u8 tn = (ySel * 9);     // Sprite tile current selection
    u8 ip = (ySelPrev * 3); // Sprite index previous selection
    u8 tp = (ySelPrev * 9); // Sprite tile previous selection

    // Previous selection
    VDP_setSpriteTile(ip+3, SPR_BANK0 + 9  + tp);
    VDP_setSpriteTile(ip+4, SPR_BANK0 + 13 + tp);
    VDP_setSpriteTile(ip+5, SPR_BANK0 + 17 + tp);

    // Current selection
    VDP_setSpriteTile(in+3, SPR_BANK0 + 72 + tn);
    VDP_setSpriteTile(in+4, SPR_BANK0 + 76 + tn);
    VDP_setSpriteTile(in+5, SPR_BANK0 + 80 + tn);

    VDP_updateSprites(24, DMA_QUEUE);
}

static void ResetScroll()
{
    s16 LineTable[LINETABLE_SIZE];
    memsetU16((u16*)LineTable, 0, LINETABLE_SIZE);
    VDP_setHorizontalScrollLine(BG_A, 0, LineTable, LINETABLE_SIZE, DMA_QUEUE);
    VDP_setHorizontalScrollLine(BG_B, 0, LineTable, LINETABLE_SIZE, DMA_QUEUE);
    SYS_doVBlankProcess();
}


const VN_GameState InGameMenuState = 
{
    Enter_Menu, ReEnter_Menu, Exit_Menu, Run_Menu, Input_Menu, NULL, VBlank_Menu
};

