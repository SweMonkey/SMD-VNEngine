#ifndef SCENEUTIL_H
#define SCENEUTIL_H

#include <genesis.h>

#define SPR_BANK0 0x680
#define SPR_BANK1 0x780

#define PAGETYPE_NULL 0
#define PAGETYPE_PAGE 1
#define PAGETYPE_CHOICE 2
#define PAGETYPE_INPUT 3

typedef enum e_pagelayer{PL_BG = BG_B, PL_FG = BG_A} PageLayer;

typedef enum e_effect{LFX_NONE          = 0, 
                      LFX_FADEIN        = 0x1,
                      LFX_FADEOUT       = 0x2,
                      LFX_SLIDEIN       = 0x4,
                      LFX_SLIDEOUT      = 0x8,
                      LFX_SHAKELR       = 0x10,
                      LFX_SILHOUETTE    = 0x20,
                      LFX_LINEGLITCH    = 0x40,
                      LFX_SINEWAVE      = 0x80} LayerEffect;

typedef struct s_music
{
    const u8 *TrackPtr;
    const s8 Repeat; 
} VN_Track;

typedef struct s_character
{
    const Image *Portrait;
    const char *Name;
    const u16 Color;
} VN_Character;

typedef struct s_page
{
    u8 PageType;

    const VN_Character *Character;

    const Image *BG;    // NULL = Use last page's BG
    const Image *FG;    // NULL = Use last page's FG

    const LayerEffect EffectBG;
    const LayerEffect EffectFG;

    const char *TextLine[4];

    const u16 SwitchDelay;   // 0 = skip delay, >0 = clear plan_a/plan_b and wait x frames

    const struct s_page *NextPage[4];
    const char * const *Script;
    const struct s_music *XGM_Track;

    const bool bTextbox;        // True: Draw a textbox behind text
    const bool bHighColourBG;   // True: Use dual palette for BG, otherwise use dual palette for FG
    const bool bAutoSwitch;     // True: Switch to next page automatically when SwitchDelay reaches 0
} VN_Page;

// Textbox variables
extern u16 VNS_TextDelay;
extern u8 VNS_TextBoxStyle;
extern u16 VNS_TextBoxColor;

bool DrawImageFG(const Image *image, bool bLowColour);
bool DrawImageBG(const Image *image);
bool Set_SHBoxEnable(VN_Page *page, bool bEnabled);
void PrintTextSpeaker(const char *str);
void PrintTextLine(const char *str, u8 x, u8 y, u16 delay);
void DrawPortrait(const Image *image);
void ClearTextArea();
void WaitFrames(u16 delay);
void VN_DoVBlank();

#endif // SCENEUTIL_H
