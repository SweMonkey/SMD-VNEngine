
#include "SceneFX.h"

static u8 DelayTimer[2] = {0};
static s16 LineTable[2][224] = {0};

static u8 ShakeCount[2] = {80};

static LayerEffect ActiveEffect[2] = {0};
static bool bEffectRunning[2] = {FALSE};


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

void FX_ShadeUD(PageLayer Layer)
{
    ActiveEffect[Layer] &= ~LFX_SHAKEUD;
}

void FX_ShadeLR(PageLayer Layer)
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

void RunEffectVSYNC()
{
    for (u8 l = 0; l < 2; l++)
    {
        bEffectRunning[l] = TRUE;

        if (ActiveEffect[l] & LFX_LINEGLITCH) FX_LineGlitch(l);

        if (ActiveEffect[l] & LFX_SHAKELR) FX_ShadeLR(l);
    }
}

void SetEffects(PageLayer Layer, LayerEffect Effect)
{
    ActiveEffect[Layer] |= Effect;
}

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

        ActiveEffect[l] = 0;
    }
}
