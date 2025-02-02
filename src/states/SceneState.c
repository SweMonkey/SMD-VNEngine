#include "GameState.h"
#include "ScriptEngine.h"
#include "SceneFX.h"
#include "SceneUtil.h"
#include "../res/System_res.h"

#define BUF_MAX_STRLEN 40

// Entry point to begin running from
extern const VN_Page EntryPage;

// In case of redraw
static const Image *LastBG = NULL;
static const Image *LastFG = NULL;
static bool bRedrawBG = FALSE;
static bool bRedrawFG = FALSE;
static bool bFxEnable = TRUE;
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
static u16 AutoSwitchCounter = 0;

// Choice variables
static u8 sIdx = 0; // Choice selection index
static u8 cCnt = 0; // Choice count

// Forward decl.
VN_GameState SceneState;


/// @brief Set the new palette depending on page FX (Fade In/Out or Set)
void set_Palettes()
{
    // Foreground
    if ((ActivePage->EffectFG & LFX_FADEIN) && (!bRedrawFG))
    {
        if (ActivePage->bHighColourBG) PAL_fadeIn(32, 47, ActivePage->FG->palette->data, 20, TRUE);
        else PAL_fadeIn(16, 47, ActivePage->FG->palette->data, 20, TRUE);
    }
    else if (ActivePage->EffectFG & LFX_SILHOUETTE)
    {
        if (ActivePage->bHighColourBG) PAL_fadeOut(32, 47, 10, TRUE);
        else PAL_fadeOut(16, 47, 10, TRUE);
    }
    else 
    {
        if (ActivePage->bHighColourBG) PAL_setColors(32, ActivePage->FG->palette->data, 16, DMA_QUEUE);
        else PAL_setColors(16, ActivePage->FG->palette->data, 32, DMA_QUEUE);
    }

    PAL_waitFadeCompletion();

    // Background
    if ((ActivePage->EffectBG & LFX_FADEIN) && (!bRedrawBG))
    {
        if (ActivePage->bHighColourBG) PAL_fadeIn(0, 31, ActivePage->BG->palette->data, 20, TRUE);
        else PAL_fadeIn(0, 15, ActivePage->BG->palette->data, 20, TRUE);
    }
    else if (ActivePage->EffectBG & LFX_SILHOUETTE)
    {
        if (ActivePage->bHighColourBG) PAL_fadeOut(0, 31, 10, TRUE);
        else PAL_fadeOut(0, 15, 10, TRUE);
    }
    else 
    {
        if (ActivePage->bHighColourBG) PAL_setColors(0, ActivePage->BG->palette->data, 32, DMA_QUEUE);
        else PAL_setColors(0, ActivePage->BG->palette->data, 16, DMA_QUEUE);
    }

    PAL_waitFadeCompletion();

    SYS_doVBlankProcess();  // Test

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

    if (DrawImageBG(ActivePage->BG) == FALSE) 
    {
        #ifdef DEBUG_STATE_MSG
        KLog("DrawImageBG returned false!");
        #endif
    }

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

    if (DrawImageFG(ActivePage->FG, ActivePage->bHighColourBG) == FALSE) 
    {
        #ifdef DEBUG_STATE_MSG
        KLog("DrawImageFG returned false!");
        #endif
    }

    SkipDrawFG:

    if (ActivePage->FG != NULL) LastFG = ActivePage->FG;

    return;
}

/// @brief Prepare data for script engine, run script and retrieve data from script engine
void ExecuteScript()
{
    // Transfer data to VM...
    if (ActivePage->Character != NULL) Script_SetVar("p.name", ActivePage->Character->Name);
    else Script_SetVar("p.name", "");

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
    Set_SHBoxEnable(ActivePage, bVisible);

    if (bVisible) VDP_setSpriteLink(20, 21);
    else VDP_setSpriteLink(20, 0);
}

/// @brief Call BG/FG/Palette drawing functions and set the new effects for the current page
void DrawNext()
{
    if ((ActivePage->XGM_Track != NULL) && (ActivePage->XGM_Track->TrackPtr != NULL))
    {
        if (LastTrack != ActivePage->XGM_Track)
        {
            XGM_setLoopNumber(ActivePage->XGM_Track->Repeat);
            XGM_startPlay(ActivePage->XGM_Track->TrackPtr);

            LastTrack = ActivePage->XGM_Track;
        }
    }

    set_FG();
    set_BG();

    if (bFxEnable) set_Palettes();

    // Don't shake screen again if bRedraw is true
    FX_SetEffects(PL_BG, (ActivePage->EffectBG & ((bRedrawBG || !bFxEnable)?(~LFX_SHAKELR):0xFFFFFFF) ));
    FX_SetEffects(PL_FG, (ActivePage->EffectFG & ((bRedrawFG || !bFxEnable)?(~LFX_SHAKELR):0xFFFFFFF) ));

    if (bRedrawBG || bRedrawFG)
    {
        bRedrawBG = FALSE;
        bRedrawFG = FALSE;
    }

    setTextBoxVisibility(ActivePage->bTextbox);

    return;
}

/// @brief Simple page which just prints page text
void DrawPage()
{
    u16 tmp_textdelay;

    if (bInstantText) tmp_textdelay = 0;
    else tmp_textdelay = VNS_TextDelay;

    DrawNext();

    PAL_setColor(59, ActivePage->Character->Color);

    PrintTextSpeaker(BUF_Name);

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

    // Change sprite 30/31 to point to portrait tiles
    VDP_setSpriteAttribut(30, TILE_ATTR_FULL(PAL3, 1, 0, 0, 0x7D9));
    VDP_setSpriteAttribut(31, TILE_ATTR_FULL(PAL3, 1, 0, 0, 0x7DD));

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

    // Setup the NextPage to point to next page and remove the "more text available" arrow sprite
    if (ActivePage->PageType == PAGETYPE_PAGE)
    {
        VDP_setSpritePosition(0, 156, 256);
        MoreArrowCounter = 32;

        NextPage = ActivePage->NextPage[0];
    }
    // Setup the NextPage to point to the chosen choice page and reset the character image box from the textbox
    else if (ActivePage->PageType == PAGETYPE_CHOICE)
    {
        VDP_setSpriteAttribut(30, TILE_ATTR_FULL(PAL3, 1, 0, 0, 0x7FC));
        VDP_setSpriteAttribut(31, TILE_ATTR_FULL(PAL3, 1, 0, 0, 0x7FC));

        PAL_setColors(48, palette_black, 12, DMA_QUEUE);

        NextPage = ActivePage->NextPage[sIdx];
    }
    // Setup the NextPage to point to the first next page, first next page may point to anywhere depending on scripts
    else if (ActivePage->PageType == PAGETYPE_INPUT)
    {
        NextPage = ActivePage->NextPage[0];
    }

    // Remove the textbox and text if we're doing a fade out
    if ((ActivePage->EffectBG & LFX_FADEOUT) && (ActivePage->EffectFG & LFX_FADEOUT))
    {
        setTextBoxVisibility(FALSE);
    }

    // Fade out palette colours if BG/FG has the LFX_FADEOUT flag. 
    // This also takes care of fading out the correct colour entries (for example if the BG is highcolour mode)
    if (ActivePage->bHighColourBG) PAL_fadeOut(((ActivePage->EffectBG & LFX_FADEOUT) ? 0 : 32), ((ActivePage->EffectFG & LFX_FADEOUT) ? 47 : 31), 20, TRUE);
    else PAL_fadeOut(((ActivePage->EffectBG & LFX_FADEOUT) ? 0  : 16), ((ActivePage->EffectFG & LFX_FADEOUT) ? 47 : 15), 20, TRUE);

    // Time delay between switching to a new page (in ms)
    if ((ActivePage->SwitchDelay) && (ActivePage->bAutoSwitch == FALSE))
    {
        waitMs(ActivePage->SwitchDelay);
    }
    
    // Make sure we're not going into oblivion...
    if (NextPage == NULL) 
    {
        #ifdef DEBUG_STATE_MSG
        KLog("NextPage is NULL!");
        #endif

        const char *err[] = {"Next page does not exist!"};
        ChangeState(GS_CRASH, 1, err);
        return;
    }

    PAL_waitFadeCompletion();

    FX_SemiResetEffect(PL_BG, NextPage->EffectBG);
    FX_SemiResetEffect(PL_FG, NextPage->EffectFG);

    memcpy(ActivePage, NextPage, sizeof(VN_Page));

    AutoSwitchCounter = ActivePage->SwitchDelay;

    return;
}

/// @brief Setup sprites needed for the scene state
static void SetupSprites()
{
    u16 SprBank0 = SPR_BANK0;   // $D000 - $DFFF
    u16 SprBank1 = SPR_BANK1;   // $F000 - $F7FF
    const u8 PL = PAL3;         // Sprite textbox palette
    const u8 PR = 1;            // Sprite textbox priority
    const u8 yS = 8;            // Sprite textbox yShift

    // More arrow
    VDP_setSpriteFull(0, 156, 256, SPRITE_SIZE(1, 1), TILE_ATTR_FULL(3, 1, 0, 0, 0x7DF), 1);
    
    // Top 10 sprites
    VDP_setSpriteFull( 1,   8, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  2); SprBank0 += 0x10;
    VDP_setSpriteFull( 2,  40, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  3); SprBank0 += 0x10;
    VDP_setSpriteFull( 3,  72, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  4); SprBank0 += 0x10;
    VDP_setSpriteFull( 4, 104, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  5); SprBank0 += 0x10;
    VDP_setSpriteFull( 5, 136, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  6); SprBank0 += 0x10;
    VDP_setSpriteFull( 6, 168, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  7); SprBank0 += 0x10;
    VDP_setSpriteFull( 7, 200, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  8); SprBank0 += 0x10;
    VDP_setSpriteFull( 8, 232, 168+yS, SPRITE_SIZE(4, 4), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank0),  9);

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
    VDP_setSpriteFull(20, 296, 200+yS, SPRITE_SIZE(2, 1), TILE_ATTR_FULL(PL, PR, 0, 0, SprBank1),  0);

    // Textbox top border line
    VDP_setSpriteFull(21,   0, 160+yS, SPRITE_SIZE(2, 1), TILE_ATTR_FULL(PL, PR, 0, 0, 0x7FC), 22);
    // ... gap for portrait (sprites 30/31) ...
    VDP_setSpriteFull(22,  64, 160+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, 0x7FC), 23);
    VDP_setSpriteFull(23,  96, 160+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, 0x7FC), 24);
    VDP_setSpriteFull(24, 128, 160+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, 0x7FC), 25);
    VDP_setSpriteFull(25, 160, 160+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, 0x7FC), 26);
    VDP_setSpriteFull(26, 192, 160+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, 0x7FC), 27);
    VDP_setSpriteFull(27, 224, 160+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, 0x7FC), 28);
    VDP_setSpriteFull(28, 256, 160+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, 0x7FC), 29);
    VDP_setSpriteFull(29, 288, 160+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, 0x7FC), 30); // Should point to 0 if you don't have the textbox top border on

    // Textbox top border line (Gap may be used for portrait sprites)
    VDP_setSpriteFull(30,  16, 160+yS, SPRITE_SIZE(4, 1), TILE_ATTR_FULL(PL, PR, 0, 0, 0x7FC), 31);
    VDP_setSpriteFull(31,  48, 160+yS, SPRITE_SIZE(2, 1), TILE_ATTR_FULL(PL, PR, 0, 0, 0x7FC), 0);

    VDP_updateSprites(20, DMA);   // 23
}

/// @brief Initialize and setup everything needed for the scene state
static void SetupState()
{
    VDP_setEnable(FALSE);
    MEM_pack();

    PAL_setColors(0, palette_black, 64, DMA);
    PAL_setColor(60, 0x000); // Text Outline
    PAL_setColor(61, 0xFFF); // Text FG

    ClearTextArea();
    SetupSprites();

    // "More text" down arrow
    VDP_loadTileData(FONT_NORMAL.tiles+0x200, 0x7DF, 1, CPU);

    // Tiles for top border line of textbox
    VDP_loadTileData(TB_BORDER.tiles, 0x7FC, 1, CPU);
    VDP_loadTileData(TB_BORDER.tiles, 0x7FD, 1, CPU);
    VDP_loadTileData(TB_BORDER.tiles, 0x7FE, 1, CPU);
    VDP_loadTileData(TB_BORDER.tiles, 0x7FF, 1, CPU);

    if (0)//VNS_TextBoxStyle)   // Solid textbox
    {
    }
    else    // Transparent
    {
        VDP_setHilightShadow(TRUE);
        VDP_setSpriteLink(0, 1);
    }

    FX_UpdateScroll();

    SYS_doVBlankProcess();
    
    VDP_setEnable(TRUE);
    
    return;
}

void Enter_Scene(u8 argc, const char *argv[])
{
    #ifdef DEBUG_STATE_MSG
    KLog("Entering scene");
    #endif

    Script_DeleteVariables();

    LastBG = NULL;
    LastFG = NULL;
    bRedrawBG = FALSE;
    bRedrawFG = FALSE;
    bInstantText = FALSE;

    XGM_setLoopNumber(-1);
    XGM_startPlay(MUS_DUMMY);

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
    #ifdef DEBUG_STATE_MSG
    KLog("RE Entering scene");
    #endif

    SetupState();

    XGM_resumePlay();

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

void ReEnter_Scene_FromMenu()
{
    #ifdef DEBUG_STATE_MSG
    KLog("RE Entering scene from in game menu");
    #endif

    SceneState.ReEnter = ReEnter_Scene;

    ClearTextArea();
    SetupSprites();
    Set_SHBoxEnable(ActivePage, TRUE);
    XGM_resumePlay();

    bFxEnable = FALSE;

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

    bFxEnable = TRUE;

    return;
}

void Exit_Scene(GameState new_state)
{
    #ifdef DEBUG_STATE_MSG
    KLog("Exiting scene");
    #endif

    VDP_setSpritePosition(0, 156, 384); // Remove textbox sprite
    VDP_setSpriteLink(0, 0);            // Remove S/H related sprites
    VDP_updateSprites(1, TRUE);

    Set_SHBoxEnable(ActivePage, FALSE);

    if (new_state != GS_INGAME_MENU) 
    {
        #ifdef DEBUG_STATE_MSG
        KLog("Resetting FX");
        #endif

        FX_ResetEffect();
    }

    if (new_state == GS_INGAME_MENU)
    {
        #ifdef DEBUG_STATE_MSG
        KLog("Pausing music");
        #endif

        XGM_pausePlay();
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
        if (ActivePage->bAutoSwitch)
        {
            if (AutoSwitchCounter)
            {
                AutoSwitchCounter--;
            }
            else
            {
                bSwitchPage = TRUE;
            }
        }
        else
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

    }
    else if (ActivePage->PageType == PAGETYPE_CHOICE)
    {
    }

    if (bSwitchPage == FALSE) FX_RunEffect();

    return;
}

void Input_Scene(u16 joy, u16 changed, u16 state)
{
    if (bSwitchPage) return;

    if (changed & state & BUTTON_START)
    {
        SceneState.ReEnter = ReEnter_Scene_FromMenu;
        ChangeState(GS_INGAME_MENU, 0, NULL);
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

    if ((changed & state & BUTTON_A) && (ActivePage->bAutoSwitch == FALSE))
    {
        bSwitchPage = TRUE;
    }

    return;
}

void VBlank_Scene()
{
    VDP_updateSprites(32, DMA);

    FX_UpdateScroll();

    return;
}

VN_GameState SceneState = 
{
    Enter_Scene, ReEnter_Scene, Exit_Scene, Run_Scene, Input_Scene, NULL, VBlank_Scene
};
