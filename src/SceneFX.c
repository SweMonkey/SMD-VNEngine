
#include "SceneFX.h"

// LineGlitch variables
static u8 DelayTimer[2] = {0, 0};

// ScreenShake variables
static u8 ShakeCount[2] = {80, 80};

// SineWave variables
static u16 SineScroll[2] = {0,  256};

// Line offsets used for scrolling individual screen scanlines
static s16 LineTable[2][256] = {{0}, {0}};

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
        memsetU16((u16*)LineTable[Layer], 0, 224);
    }
    else if (DelayTimer[Layer] == 1)
    {
        while (i < 224)
        {
            u16 r = random();
            u8 skip = 1 + (r % 2);

            if (skip)
            for (u8 j = skip; j > 0; j--) LineTable[Layer][((i+j)%224)] = (-16*skip) + ((r % 32)*skip);
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

        memsetU16((u16*)LineTable[Layer], r, 224);
    }

    DelayTimer[Layer]++;
    VDP_setHorizontalScrollLine((Layer==PL_FG?BG_A:BG_B), 0, LineTable[Layer], 224, DMA);
}

/// @brief Simple effect to shake the screen left and right
/// @param Layer Layer to run the effect on (PL_BG, PL_FG)
void FX_ShakeLR(PageLayer Layer)
{
    if (ShakeCount[Layer]--)
    {
        if (ShakeCount[Layer] % 4 == 0)
        {
            memsetU16((u16*)LineTable[Layer], (ShakeCount[Layer]%8?-8:8), 224);
            VDP_setHorizontalScrollLine((Layer==PL_FG?BG_A:BG_B), 0, LineTable[Layer], 224, DMA);
        }
    }
    else 
    {
        memsetU16((u16*)LineTable[Layer], 0, 224);
        VDP_setHorizontalScrollLine((Layer==PL_FG?BG_A:BG_B), 0, LineTable[Layer], 224, DMA);

        ActiveEffect[Layer] &= ~LFX_SHAKELR;
        ShakeCount[Layer] = 80;
    }
}

/// @brief Wave layer in a sinus shape
/// @param Layer Layer to run the effect on (PL_BG, PL_FG)
void FX_SineWave(PageLayer Layer)
{
    for (u8 i = 0; i < 224; i++)
    {
        LineTable[Layer][i] = sinFix16(i+SineScroll[Layer]);
    }
    
    SineScroll[Layer] += 4;
    VDP_setHorizontalScrollLine((Layer==PL_FG?BG_A:BG_B), 0, LineTable[Layer], 224, DMA);
}

void CalcEffect()
{
    
}

/// @brief This function ticks the active effects and should be run from a VBlank handler.
void RunEffectVSYNC()
{
    for (u8 l = 0; l < 2; l++)
    {
        bEffectRunning[l] = TRUE;

        if (ActiveEffect[l] & LFX_LINEGLITCH) FX_LineGlitch(l);
        else if (ActiveEffect[l] & LFX_SHAKELR) FX_ShakeLR(l);
        else if (ActiveEffect[l] & LFX_SINEWAVE) FX_SineWave(l);
    }
}

/// @brief Set the active effects
/// @param Layer Layer to set the effect for (PL_BG, PL_FG)
/// @param Effect Effects to set active
void SetEffects(PageLayer Layer, LayerEffect Effect)
{
    ActiveEffect[Layer] |= Effect;
}

/// @brief Reset all active effects and restore the screen to normal
void ResetEffect()
{
    for (u8 l = 0; l < 2; l++)
    {
        if (bEffectRunning[l])
        {
            memsetU16((u16*)LineTable[l], 0, 224);
            VDP_setHorizontalScrollLine((l==PL_FG?BG_A:BG_B), 0, LineTable[l], 224, DMA);

            bEffectRunning[l] = FALSE;
        }

        //SineScroll[l] = random() % 256;
        ActiveEffect[l] = 0;
    }
}

void SemiResetEffect(PageLayer Layer, LayerEffect NewEffects)
{
    if (((NewEffects          & (LFX_SHAKELR | LFX_LINEGLITCH | LFX_SINEWAVE)) & 
         (ActiveEffect[Layer] & (LFX_SHAKELR | LFX_LINEGLITCH | LFX_SINEWAVE))) == 0)
    {
        if (bEffectRunning[Layer])
        {
            memsetU16((u16*)LineTable[Layer], 0, 224);
            VDP_setHorizontalScrollLine((Layer==PL_FG?BG_A:BG_B), 0, LineTable[Layer], 224, DMA);

            bEffectRunning[Layer] = FALSE;
        }

        ActiveEffect[Layer] = 0;
    }
    else ActiveEffect[Layer] = NewEffects;
}
