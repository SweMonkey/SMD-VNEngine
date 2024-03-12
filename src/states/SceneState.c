
#include "GameState.h"
#include "ScriptEngine.h"
#include "SceneFX.h"
#include "../res/system.h"

#define BUF_MAX_STRLEN 40
//#define NO_INT

// Entry point to begin running from
extern const VN_Page EntryPage;

// In case of redraw
static const Image *LastBG = NULL;
static const Image *LastFG = NULL;
static bool bRedrawBG = FALSE;
static bool bRedrawFG = FALSE;
static bool bInstantText = FALSE;

static const VN_Track *LastTrack = NULL;

// Temporary buffer variables
static char BUF_Name[BUF_MAX_STRLEN];
static char BUF_TextLine[4][BUF_MAX_STRLEN];
static char BUF_NextPtr[4][8];

// External variables
extern char InputBuffer[32];

// Scene state variables
static bool bSwitchPage = TRUE;
static VN_Page *ActivePage = NULL;

// Page variables
static u8 MoreArrowCounter = 32;

// Choice variables
static u8 sIdx = 0; // Choice selection index
static u8 cCnt = 0; // Choice count


/// @brief Set the new palette depending on page FX (Fade In/Out or Set)
void set_Palettes()
{
    // Foreground
    if ((ActivePage->EffectFG & LFX_FADEIN) && (!bRedrawFG))
    {
        PAL_fadeIn(16, 47, ActivePage->FG->palette->data, 20, TRUE);
    }
    else if (ActivePage->EffectFG & LFX_SILHOUETTE)
    {
        PAL_fadeOut(16, 47, 10, TRUE);
    }
    else 
    {
        PAL_setColors(16, ActivePage->FG->palette->data, 32, DMA_QUEUE);
    }

    PAL_waitFadeCompletion();

    // Background
    if ((ActivePage->EffectBG & LFX_FADEIN) && (!bRedrawBG))
    {
        PAL_fadeIn(0, 15, ActivePage->BG->palette->data, 20, TRUE);
    }
    else if (ActivePage->EffectBG & LFX_SILHOUETTE)
    {
        PAL_fadeOut(0, 15, 10, TRUE);
    }
    else 
    {
        PAL_setColors(0, ActivePage->BG->palette->data, 16, DMA_QUEUE);
    }

    PAL_waitFadeCompletion();

    return;
}

/// @brief Set and draw the background image or just set it if its the same as the last image
void set_BG()
{
    if (!bRedrawBG)
    {
        if (ActivePage->BG == LastBG) goto SkipDrawBG;

        if (ActivePage->BG == NULL)
        {
            ActivePage->BG = LastBG;
            goto SkipDrawBG;
        }
    }

    DrawImageBG(ActivePage->BG);

    SkipDrawBG:

    if (ActivePage->BG != NULL) LastBG = ActivePage->BG;
        
    return;
}

/// @brief Set and draw the foreground image or just set it if its the same as the last image
void set_FG()
{
    if (!bRedrawFG)
    {
        if (ActivePage->FG == LastFG) goto SkipDrawFG;

        if (ActivePage->FG == NULL)
        {
            ActivePage->FG = LastFG;
            goto SkipDrawFG;
        }
    }

    DrawImageFG(ActivePage->FG);

    SkipDrawFG:

    if (ActivePage->FG != NULL) LastFG = ActivePage->FG;

    return;
}

/// @brief Prepare data for script engine, run script and retrieve data from script engine
void ExecuteScript()
{
    // Transfer data to VM...
    Script_SetVar("p.name", ActivePage->Character->Name);
    Script_SetVar("p.line[1]", ActivePage->TextLine[0]);
    Script_SetVar("p.line[2]", ActivePage->TextLine[1]);
    Script_SetVar("p.line[3]", ActivePage->TextLine[2]);
    Script_SetVar("p.line[4]", ActivePage->TextLine[3]);

    Script_SetVar("i.buf", InputBuffer);

    // You saw nothing... >:(
    itoa(BUF_NextPtr[0], (s32)ActivePage->NextPage[0]);
    itoa(BUF_NextPtr[1], (s32)ActivePage->NextPage[1]);
    itoa(BUF_NextPtr[2], (s32)ActivePage->NextPage[2]);
    itoa(BUF_NextPtr[3], (s32)ActivePage->NextPage[3]);

    Script_SetVar("p.next[0]", BUF_NextPtr[0]);
    Script_SetVar("p.next[1]", BUF_NextPtr[1]);
    Script_SetVar("p.next[2]", BUF_NextPtr[2]);
    Script_SetVar("p.next[3]", BUF_NextPtr[3]);

    if (ActivePage->Script != NULL) Script_Execute(*ActivePage->Script);

    // Transfer results back from VM...
    Script_GetVar("p.next[0]", BUF_NextPtr[0], 8);
    Script_GetVar("p.next[1]", BUF_NextPtr[1], 8);
    Script_GetVar("p.next[2]", BUF_NextPtr[2], 8);
    Script_GetVar("p.next[3]", BUF_NextPtr[3], 8);

    // Again, nothing bad going on here!
    ActivePage->NextPage[0] = (const struct s_page *)atoi(BUF_NextPtr[0]);
    ActivePage->NextPage[1] = (const struct s_page *)atoi(BUF_NextPtr[1]);
    ActivePage->NextPage[2] = (const struct s_page *)atoi(BUF_NextPtr[2]);
    ActivePage->NextPage[3] = (const struct s_page *)atoi(BUF_NextPtr[3]);

    Script_GetVar("p.name", BUF_Name, BUF_MAX_STRLEN);
    Script_GetVar("p.line[1]", BUF_TextLine[0], BUF_MAX_STRLEN);
    Script_GetVar("p.line[2]", BUF_TextLine[1], BUF_MAX_STRLEN);
    Script_GetVar("p.line[3]", BUF_TextLine[2], BUF_MAX_STRLEN);
    Script_GetVar("p.line[4]", BUF_TextLine[3], BUF_MAX_STRLEN);
}

/// @brief Set visibility of shadow textbox (Recommended to clear text first if FALSE)
/// @param bVisible 
void setTextBoxVisibility(bool bVisible)
{
    VDP_setHilightShadow(bVisible);
}

/// @brief Call BG/FG/Palette drawing functions and set the new effects for the current page
void DrawNext()
{
    if ((ActivePage->XGM_Track != NULL) && (ActivePage->XGM_Track->TrackPtr != NULL))
    {
        if (LastTrack != ActivePage->XGM_Track)
        {
            //if (XGM_isPlaying) XGM_stopPlay();

            XGM_setLoopNumber(ActivePage->XGM_Track->Repeat);
            XGM_startPlay(ActivePage->XGM_Track->TrackPtr);

            LastTrack = ActivePage->XGM_Track;
        }
    }

    set_FG();
    set_BG();

#ifndef NO_INT
//    SYS_disableInts();
#endif

    set_Palettes();

    // Don't shake screen again if bRedraw is true
    FX_SetEffects(PL_BG, (ActivePage->EffectBG & (bRedrawBG?(~LFX_SHAKELR):0xFFFFFFF) ));
    FX_SetEffects(PL_FG, (ActivePage->EffectFG & (bRedrawFG?(~LFX_SHAKELR):0xFFFFFFF) ));

    if (bRedrawBG || bRedrawFG)
    {
        bRedrawBG = FALSE;
        bRedrawFG = FALSE;
    }

    setTextBoxVisibility(ActivePage->bTextbox);

#ifndef NO_INT
//    SYS_enableInts();
#endif

    return;
}

/// @brief Simple page which just prints page text
void DrawPage()
{
    u16 tmp_textdelay;

    if (bInstantText) tmp_textdelay = 0;
    else tmp_textdelay = VNS_TextDelay;

    DrawNext();

    PrintTextLine(BUF_Name, 1, 0, 0);

    PrintTextLine(BUF_TextLine[0], 1, 1, tmp_textdelay);
    PrintTextLine(BUF_TextLine[1], 1, 2, tmp_textdelay);
    PrintTextLine(BUF_TextLine[2], 1, 3, tmp_textdelay);
    PrintTextLine(BUF_TextLine[3], 1, 4, tmp_textdelay);

    return;
}

/// @brief Print choice page text and show portrait sprites
void DrawChoice()
{
    u8 cIdx = 0;
    cCnt = 0;
    
    DrawNext();

    VDP_setSpriteLink(20, 21);  // Enable extra portrait sprites

    DrawPortrait(ActivePage->Character->Portrait);

    sIdx = 0;
    PrintTextLine(">", 8, 1, 0);

    if (ActivePage->NextPage[cIdx] != NULL){ PrintTextLine(ActivePage->TextLine[cIdx], 9, cIdx+1, 0); cCnt++;} cIdx++;
    if (ActivePage->NextPage[cIdx] != NULL){ PrintTextLine(ActivePage->TextLine[cIdx], 9, cIdx+1, 0); cCnt++;} cIdx++;
    if (ActivePage->NextPage[cIdx] != NULL){ PrintTextLine(ActivePage->TextLine[cIdx], 9, cIdx+1, 0); cCnt++;} cIdx++;
    if (ActivePage->NextPage[cIdx] != NULL){ PrintTextLine(ActivePage->TextLine[cIdx], 9, cIdx+1, 0); cCnt++;} cIdx++;

    return;
}

void GetInput()
{
    const char *argv[2] =
    {
        ActivePage->TextLine[0],
        ActivePage->TextLine[1]
    };

    ChangeState(GS_TEXTINPUT, 2, argv);

    return;
}

/// @brief Clear last page/choice screen and get the next page/choice
void PrepareNext()
{
    const VN_Page *NextPage = NULL;

    ClearTextArea();
    bInstantText = FALSE;

    if (ActivePage->PageType == PAGETYPE_PAGE)
    {
        VDP_setSpritePosition(0, 156, 256);
        MoreArrowCounter = 32;

        NextPage = ActivePage->NextPage[0];
    }
    else if (ActivePage->PageType == PAGETYPE_CHOICE)
    {        
        //DMA_doVRamFill(0x7D9*32, 0xC0, 0, 1);
        VDP_setSpriteLink(20, 0);   // Disable extra portrait sprites
        VDP_updateSprites(23, CPU);

        PAL_setColors(48, palette_black, 12, DMA_QUEUE);

        NextPage = ActivePage->NextPage[sIdx];
    }
    else if (ActivePage->PageType == PAGETYPE_INPUT)
    {
        NextPage = ActivePage->NextPage[0];
    }

    VDP_updateSprites(23, DMA);

    if ((ActivePage->EffectBG & LFX_FADEOUT) && (ActivePage->EffectFG & LFX_FADEOUT))
    {
        setTextBoxVisibility(FALSE);
    }

    //waitMs(500);
    
#ifndef NO_INT
//    SYS_disableInts();
#endif

    //if (ActivePage->EffectBG & LFX_FADEOUT) PAL_fadeOut(0, 15, 20, FALSE);
    //if (ActivePage->EffectFG & LFX_FADEOUT) PAL_fadeOut(16, 47, 20, FALSE);

    PAL_fadeOut(((ActivePage->EffectBG & LFX_FADEOUT) ? 0  : 16), ((ActivePage->EffectFG & LFX_FADEOUT) ? 47 : 15), 20, TRUE);    

    if (ActivePage->SwitchDelay)
    {
        waitMs(ActivePage->SwitchDelay);
    }
    
    if (NextPage == NULL) 
    {
        KLog("NextPage is NULL!");

#ifndef NO_INT
//        SYS_enableInts();
#endif
        ChangeState(GS_CRASH, 0, NULL);
        return;
    }

    PAL_waitFadeCompletion();

    FX_SemiResetEffect(PL_BG, NextPage->EffectBG);
    FX_SemiResetEffect(PL_FG, NextPage->EffectFG);

    memcpy(ActivePage, NextPage, sizeof(VN_Page));

#ifndef NO_INT
//    SYS_enableInts();
#endif

    return;
}

/// @brief Initialize and setup everything needed for the scene state
void SetupState()
{
    u16 SprBank0 = SPR_BANK0;   // $D000 - $DFFF
    u16 SprBank1 = SPR_BANK1;   // $F000 - $F7FF
    const u8 PL = PAL3; // Sprite textbox palette
    const u8 PR = 1;    // Sprite textbox priority
    const u8 yS = 8;    // Sprite textbox yShift

    MEM_pack();

    PAL_setPalette(PAL0, palette_black, CPU);
    PAL_setPalette(PAL1, palette_black, CPU);
    PAL_setPalette(PAL2, palette_black, CPU);
    PAL_setPalette(PAL3, palette_black, CPU);
    PAL_setColor(60, 0x000); // Text Outline
    PAL_setColor(61, 0xFFF); // Text FG

    //DMA_setBufferSize(0x2000);

    //VDP_setReg(0, 0x24);

    VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_COLUMN);

    Z80_loadDriver(Z80_DRIVER_XGM, TRUE);
    //XGM_setManualSync(TRUE);

    ClearTextArea();

    // Top 10 sprites
    VDP_setSpriteFull( 1,   8, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 2); SprBank0 += 0x10;
    VDP_setSpriteFull( 2,  40, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 3); SprBank0 += 0x10;
    VDP_setSpriteFull( 3,  72, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 4); SprBank0 += 0x10;
    VDP_setSpriteFull( 4, 104, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 5); SprBank0 += 0x10;
    VDP_setSpriteFull( 5, 136, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 6); SprBank0 += 0x10;
    VDP_setSpriteFull( 6, 168, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 7); SprBank0 += 0x10;
    VDP_setSpriteFull( 7, 200, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 8); SprBank0 += 0x10;
    VDP_setSpriteFull( 8, 232, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0), 9);

    VDP_setSpriteFull( 9, 264, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 10); SprBank1 += 0x10;
    VDP_setSpriteFull(10, 296, 168+yS, SPRITE_SIZE(2, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 11); SprBank1 += 0x8;

    // Bottom 10 sprites
    VDP_setSpriteFull(11,   8, 200+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 12); SprBank1 += 0x4;
    VDP_setSpriteFull(12,  40, 200+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 13); SprBank1 += 0x4;
    VDP_setSpriteFull(13,  72, 200+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 14); SprBank1 += 0x4;    
    VDP_setSpriteFull(14, 104, 200+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 15); SprBank1 += 0x4;
    VDP_setSpriteFull(15, 136, 200+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 16); SprBank1 += 0x4;
    VDP_setSpriteFull(16, 168, 200+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 17); SprBank1 += 0x4;
    VDP_setSpriteFull(17, 200, 200+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 18); SprBank1 += 0x4;
    VDP_setSpriteFull(18, 232, 200+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 19); SprBank1 += 0x4;
    VDP_setSpriteFull(19, 264, 200+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 20); SprBank1 += 0x4;
    VDP_setSpriteFull(20, 296, 200+yS, SPRITE_SIZE(2, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1), 0);

    // Extra 1x6 sprites for portrait
    VDP_setSpriteFull(21,  16, 160+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, 0x7D9), 22);//D9 DA DB DC
    VDP_setSpriteFull(22,  48, 160+yS, SPRITE_SIZE(2, 1), TILE_ATTR_FULL(PL, PR, 0, 0, 0x7DD),  0);//DD DE

    // More arrow
    VDP_setSpriteFull(0, 156, 256, SPRITE_SIZE(1, 1), TILE_ATTR_FULL(3, 1, 0, 0, 0x7DF), 1);
    VDP_loadTileData(FONT_SCENESH.tiles+0x200, 0x7DF , 1, CPU);

    if (0)//VNS_TextBoxStyle)   // Solid textbox
    {
    }
    else    // Transparent
    {
        VDP_setHilightShadow(1);
        VDP_setSpriteLink(0, 1);
    }
    
    VDP_updateSprites(23, DMA_QUEUE);
    SYS_doVBlankProcess();
    
    return;
}

void Enter_Scene(u8 argc, const char *argv[])
{
    KLog("Entering scene");

    Script_DeleteVariables();

    LastBG = NULL;
    LastFG = NULL;
    bRedrawBG = FALSE;
    bRedrawFG = FALSE;
    bInstantText = FALSE;

    SetupState();

    if (ActivePage == NULL) ActivePage = (VN_Page*)MEM_alloc(sizeof(VN_Page));
    memcpy(ActivePage, &EntryPage, sizeof(VN_Page));

    // Cheaty kickstart
    ExecuteScript();

    switch (ActivePage->PageType)
    {
    case PAGETYPE_NULL:
    break;

    case PAGETYPE_PAGE:
        DrawPage();
    break;

    case PAGETYPE_CHOICE:
        DrawChoice();
    break;

    default:
    break;
    }

    bSwitchPage = FALSE;

    return;
}

void ReEnter_Scene()
{
    KLog("RE Entering scene");

    SetupState();

    if (XGM_isPlaying()) XGM_resumePlay();

    bRedrawBG = TRUE;
    bRedrawFG = TRUE;

    switch (ActivePage->PageType)
    {
    case PAGETYPE_NULL:
    break;

    case PAGETYPE_PAGE:
        bInstantText = TRUE;
        DrawPage();
    break;

    case PAGETYPE_CHOICE:
        bInstantText = TRUE;
        DrawChoice();
    break;

    case PAGETYPE_INPUT:
        bSwitchPage = TRUE;
    break;

    default:
    break;
    }

    return;
}

void Exit_Scene(GameState new_state)
{
    KLog("Exiting scene");

    VDP_setSpritePosition(0, 156, 384); // Remove textbox sprite
    VDP_setSpriteLink(0, 0);            // Remove S/H related sprites
    VDP_updateSprites(1, TRUE);

    VDP_clearPlane(BG_B, TRUE);
    VDP_clearPlane(BG_A, TRUE);

    FX_ResetEffect();

    if ((new_state != GS_TEXTINPUT) && (new_state != GS_Options))
    {
        XGM_pausePlay();
        KLog("Pausing music");
    }

    SYS_doVBlankProcess();

    return;
}

void Run_Scene()
{
    if (bSwitchPage)
    {
        PrepareNext();
        ExecuteScript();

        switch (ActivePage->PageType)
        {
        case PAGETYPE_NULL:
        break;

        case PAGETYPE_PAGE:
            DrawPage();
        break;

        case PAGETYPE_CHOICE:
            DrawChoice();
        break;

        case PAGETYPE_INPUT:
            GetInput();
        break;
        
        default:
            break;
        }

        bSwitchPage = FALSE;
    }
    else if (ActivePage->PageType == PAGETYPE_PAGE)
    {
        if (MoreArrowCounter >= 64)
        {
            VDP_setSpritePosition(0, 156, 216);
            MoreArrowCounter = 0;
        }

        if (MoreArrowCounter == 32)
        {
            VDP_setSpritePosition(0, 156, 220);
        }

        MoreArrowCounter++;
    }
    else if (ActivePage->PageType == PAGETYPE_CHOICE)
    {
    }

    if (bSwitchPage == FALSE) FX_RunEffect();
    
    //XGM_nextXFrame(1);

    return;
}

void Input_Scene(u16 joy, u16 changed, u16 state)
{
    if (bSwitchPage) return;

    if (changed & state & BUTTON_START)
    {
        ChangeState(GS_Options, 0, NULL);
    }

    if (ActivePage->PageType == PAGETYPE_CHOICE)
    {
        if (changed & state & BUTTON_UP)
        {
            if (sIdx > 0) sIdx--;
        }

        if (changed & state & BUTTON_DOWN)
        {
            if (sIdx < (cCnt-1)) sIdx++;
        }

        if (changed & state & (BUTTON_UP | BUTTON_DOWN))
        {
            PrintTextLine(" ", 8, 1, 0);
            PrintTextLine(" ", 8, 2, 0);
            PrintTextLine(" ", 8, 3, 0);
            PrintTextLine(" ", 8, 4, 0);
            PrintTextLine(">", 8, sIdx+1, 0);
        }
    }

    if (changed & state & BUTTON_A)
    {
        bSwitchPage = TRUE;
    }

    return;
}

void VBlank_Scene()
{
    VDP_updateSprites(23, DMA);

    FX_UpdateScroll();

    //XGM_nextFrame();
    
    //PAL_setColors(0, ActivePage->BG->palette->data, 16, CPU);
    //PAL_setColors(16, ActivePage->FG->palette->data, 32, CPU);

    //if (bSwitchPage) RunEffectVSYNC();  // Keep animating FX during text render/Page switch

    return;
}

const VN_GameState SceneState = 
{
    Enter_Scene, ReEnter_Scene, Exit_Scene, Run_Scene, Input_Scene, NULL, VBlank_Scene
};
