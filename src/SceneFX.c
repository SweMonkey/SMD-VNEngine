#include "SceneFX.h"

#define LINETABLE_SIZE 224

// LineGlitch variables
static u8 DelayTimer[2] = {0, 0};

// SlideIn/Out variables
static s16 SlideXPos[2] = {0, 0};

// ScreenShake variables
static u8 ShakeCount[2] = {80, 80};

// SineWave variables
static u16 SineScroll[2] = {0,  256};

// Line offsets used for scrolling individual screen scanlines
static s16 LineTable[2][LINETABLE_SIZE] = {{0}, {0}};

// Misc effect status
static LayerEffect ActiveEffect[2] = {0, 0};
static bool bEffectRunning[2] = {FALSE, FALSE};


/// @brief Effect which (tries to) simulates glitchy lines
/// @param Layer Layer to run the effect on (PL_BG, PL_FG)
void FX_LineGlitch(PageLayer Layer)
{
    u8 i = 0;
    
    if (DelayTimer[Layer] >= 16)//6)
    {
        DelayTimer[Layer] = 0;
    }
    else if (DelayTimer[Layer] >= 5)//6)
    {
        memsetU16((u16*)LineTable[Layer], 0, LINETABLE_SIZE);
    }
    else if (DelayTimer[Layer] == 1)
    {
        while (i < LINETABLE_SIZE)
        {
            u16 r = random();
            u8 skip = 1 + (r % 2);

            if (skip)
            for (u8 j = skip; j > 0; j--) LineTable[Layer][((i+j)%LINETABLE_SIZE)] = (-16*skip) + ((r % 32)*skip);
            else LineTable[Layer][i] = 0;
                
            i += 1 + (r % 32);
        }
    }
    else if (DelayTimer[Layer] == 3)
    {
        u16 r = 0;
        
        if (Layer == PL_FG)
        {
            r = random() % 64;
            
            if (r == 0) r = 32;
            else if (r == 1) r = -32;
            else r = 0;
        }

        memsetU16((u16*)LineTable[Layer], r, LINETABLE_SIZE);
    }

    DelayTimer[Layer]++;
}

/// @brief Slide Layer into the center of screen
/// @param Layer Layer to run the effect on (PL_BG, PL_FG)
void FX_SlideIn(PageLayer Layer)
{
    if (SlideXPos[Layer])
    {
        memsetU16((u16*)LineTable[Layer], SlideXPos[Layer], LINETABLE_SIZE);

        SlideXPos[Layer] -= 8;
    }
    else ActiveEffect[Layer] &= ~LFX_SLIDEIN;
}

/// @brief Slide Layer out of the center of screen to the right
/// @param Layer Layer to run the effect on (PL_BG, PL_FG)
void FX_SlideOut(PageLayer Layer)
{
    if (SlideXPos[Layer] < 256)
    {
        memsetU16((u16*)LineTable[Layer], SlideXPos[Layer], LINETABLE_SIZE);

        SlideXPos[Layer] += 8;
    }
    else ActiveEffect[Layer] &= ~LFX_SLIDEOUT;
}

/// @brief Simple effect to shake the screen left and right
/// @param Layer Layer to run the effect on (PL_BG, PL_FG)
void FX_ShakeLR(PageLayer Layer)
{
    if (ShakeCount[Layer]--)
    {
        if (ShakeCount[Layer] % 4 == 0)
        {
            memsetU16((u16*)LineTable[Layer], (ShakeCount[Layer]%8?-ShakeCount[Layer]>>3:ShakeCount[Layer]>>3), LINETABLE_SIZE);
        }
    }
    else 
    {
        memsetU16((u16*)LineTable[Layer], 0, LINETABLE_SIZE);
        ActiveEffect[Layer] &= ~LFX_SHAKELR;
        ShakeCount[Layer] = 80;
    }
}

/// @brief Wave layer in a sinus shape
/// @param Layer Layer to run the effect on (PL_BG, PL_FG)
void FX_SineWave(PageLayer Layer)
{
    for (u8 i = 0; i < LINETABLE_SIZE; i++)
    {
        LineTable[Layer][i] = sinFix16(i+SineScroll[Layer]);
    }

    SineScroll[Layer] += 4;
}

/// @brief Ticks the active effects, should be run from main loop
void FX_RunEffect()
{
    for (u8 l = 0; l < 2; l++)
    {
        bEffectRunning[l] = TRUE;

        if (ActiveEffect[l] & LFX_LINEGLITCH) FX_LineGlitch(l);

        if (ActiveEffect[l] & LFX_SLIDEIN) FX_SlideIn(l);

        if (ActiveEffect[l] & LFX_SLIDEOUT) FX_SlideOut(l);

        if (ActiveEffect[l] & LFX_SHAKELR) FX_ShakeLR(l);

        if (ActiveEffect[l] & LFX_SINEWAVE) FX_SineWave(l);
    }
}

/// @brief Update effects. This should be run from a VBlank handler
void FX_UpdateScroll()
{
    for (u8 l = 0; l < 2; l++)
    {
        if ((ActiveEffect[l] & LFX_LINEGLITCH) ||
            (ActiveEffect[l] & LFX_SLIDEIN)    ||
            (ActiveEffect[l] & LFX_SLIDEOUT)   ||
            (ActiveEffect[l] & LFX_SHAKELR)    ||
            (ActiveEffect[l] & LFX_SINEWAVE))
        {
            VDP_setHorizontalScrollLine((l==PL_FG?BG_A:BG_B), 0, LineTable[l], LINETABLE_SIZE, DMA);
        }
    }
}

/// @brief Set the active effects
/// @param Layer Layer to set the effect for (PL_BG, PL_FG)
/// @param Effect Effects to set active
void FX_SetEffects(PageLayer Layer, LayerEffect Effect)
{
    ActiveEffect[Layer] |= Effect;
}

/// @brief Reset all active effects and restore the screen to normal
void FX_ResetEffect()
{
    for (u8 l = 0; l < 2; l++)
    {
        if (bEffectRunning[l])
        {
            memsetU16((u16*)LineTable[l], 0, LINETABLE_SIZE);
            VDP_setHorizontalScrollLine((l==PL_FG?BG_A:BG_B), 0, LineTable[l], LINETABLE_SIZE, DMA_QUEUE_COPY);

            bEffectRunning[l] = FALSE;
        }

        //SineScroll[l] = random() % 256;
        ActiveEffect[l] = 0;
    }
}

/// @brief Reset and cancel effects from old page, but keep the effects in NewEffects going
/// @param Layer Layer to reset the effect for (PL_BG, PL_FG)
/// @param Effect New effects to keep active or set as active
void FX_SemiResetEffect(PageLayer Layer, LayerEffect NewEffects)
{
    if ((SlideXPos[Layer]) && (NewEffects & LFX_SLIDEIN)) return;

    if (((NewEffects          & (LFX_SLIDEIN | LFX_SLIDEOUT | LFX_SHAKELR | LFX_LINEGLITCH | LFX_SINEWAVE)) & 
         (ActiveEffect[Layer] & (LFX_SLIDEIN | LFX_SLIDEOUT | LFX_SHAKELR | LFX_LINEGLITCH | LFX_SINEWAVE))) == 0)
    {
        if (bEffectRunning[Layer])
        {
            memsetU16((u16*)LineTable[Layer], 0, LINETABLE_SIZE);
            VDP_setHorizontalScrollLine((Layer==PL_FG?BG_A:BG_B), 0, LineTable[Layer], LINETABLE_SIZE, DMA_QUEUE_COPY);

            bEffectRunning[Layer] = FALSE;
        }

        ActiveEffect[Layer] = 0;
    }
    else ActiveEffect[Layer] = NewEffects;
}
