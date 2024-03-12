
#include "SceneUtil.h"
#include "SceneFX.h"
#include "../res/system.h"

const u16 CharMap[5][38] =
{
    {   // Row 1
        0x680, 0x684, 0x688, 0x68C,
        0x690, 0x694, 0x698, 0x69C,
        0x6A0, 0x6A4, 0x6A8, 0x6AC,
        0x6B0, 0x6B4, 0x6B8, 0x6BC,
        0x6C0, 0x6C4, 0x6C8, 0x6CC,
        0x6D0, 0x6D4, 0x6D8, 0x6DC,
        0x6E0, 0x6E4, 0x6E8, 0x6EC,
        0x6F0, 0x6F4, 0x6F8, 0x6FC,
        0x780, 0x784, 0x788, 0x78C, 0x790, 0x794
    },
    {   // Row 2
        0x681, 0x685, 0x689, 0x68D,
        0x691, 0x695, 0x699, 0x69D,
        0x6A1, 0x6A5, 0x6A9, 0x6AD,
        0x6B1, 0x6B5, 0x6B9, 0x6BD,
        0x6C1, 0x6C5, 0x6C9, 0x6CD,
        0x6D1, 0x6D5, 0x6D9, 0x6DD,
        0x6E1, 0x6E5, 0x6E9, 0x6ED,
        0x6F1, 0x6F5, 0x6F9, 0x6FD,
        0x781, 0x785, 0x789, 0x78D, 0x791, 0x795
    },
    {   // Row 3
        0x682, 0x686, 0x68A, 0x68E,
        0x692, 0x696, 0x69A, 0x69E,
        0x6A2, 0x6A6, 0x6AA, 0x6AE,
        0x6B2, 0x6B6, 0x6BA, 0x6BE,
        0x6C2, 0x6C6, 0x6CA, 0x6CE,
        0x6D2, 0x6D6, 0x6DA, 0x6DE,
        0x6E2, 0x6E6, 0x6EA, 0x6EE,
        0x6F2, 0x6F6, 0x6FA, 0x6FE,
        0x782, 0x786, 0x78A, 0x78E, 0x792, 0x796
    },
    {   // Row 4
        0x683, 0x687, 0x68B, 0x68F,
        0x693, 0x697, 0x69B, 0x69F,
        0x6A3, 0x6A7, 0x6AB, 0x6AF,
        0x6B3, 0x6B7, 0x6BB, 0x6BF,
        0x6C3, 0x6C7, 0x6CB, 0x6CF,
        0x6D3, 0x6D7, 0x6DB, 0x6DF,
        0x6E3, 0x6E7, 0x6EB, 0x6EF,
        0x6F3, 0x6F7, 0x6FB, 0x6FF,
        0x783, 0x787, 0x78B, 0x78F, 0x793, 0x797
    },
    {   // Row 5
        0x798, 0x799, 0x79A, 0x79B,
        0x79C, 0x79D, 0x79E, 0x79F,
        0x7A0, 0x7A1, 0x7A2, 0x7A3,
        0x7A4, 0x7A5, 0x7A6, 0x7A7,
        0x7A8, 0x7A9, 0x7AA, 0x7AB,
        0x7AC, 0x7AD, 0x7AE, 0x7AF,
        0x7B0, 0x7B1, 0x7B2, 0x7B3,
        0x7B4, 0x7B5, 0x7B6, 0x7B7,
        0x7B8, 0x7B9, 0x7BA, 0x7BB, 0x7BC, 0x7BD
    },
};

const u16 PortraitMap[6][6] =
{
    {   // Row 1
        0x7D9, 0x7DA, 0x7DB, 0x7DC, 0x7DD, 0x7DE
    },
    {   // Row 2
        0x684, 0x688, 0x68C, 0x690, 0x694, 0x698
    },
    {   // Row 3
        0x685, 0x689, 0x68D, 0x691, 0x695, 0x699
    },
    {   // Row 4
        0x686, 0x68A, 0x68E, 0x692, 0x696, 0x69A
    },
    {   // Row 5
        0x687, 0x68B, 0x68F, 0x693, 0x697, 0x69B
    },
    {   // Row 6
        0x799, 0x79A, 0x79B, 0x79C, 0x79D, 0x79E
    },
};

u16 VNS_TextDelay = 20;         // 100-VNS_TextDelay = Text printing speed
u8 VNS_TextBoxStyle = 0;        // TODO: reimplement?
u16 VNS_TextBoxColor = 0x222;   // TODO: reimplement?

static u16 TileIdxFG_End = 1;
static u16 TileIdxBG_Start = 0x5FF;

TileSet *UnpackTS(const Image *image)
{
    TileSet *TS = NULL;

    if (image->tileset->compression == COMPRESSION_NONE) TS = image->tileset;
    else TS = unpackTileSet(image->tileset, NULL);

    if (TS == NULL)
    {
        KLog("Failed to unpack tileset");
        KLog_U1("Largest free block: ", MEM_getLargestFreeBlock());
        KLog_U1("Tileset compression: ", image->tileset->compression);
        return NULL;
    }

    return TS;
}

TileMap *UnpackTM(const Image *image)
{
    TileMap *TM = NULL;

    if (image->tilemap->compression == COMPRESSION_NONE) TM = image->tilemap;
    else TM = unpackTileMap(image->tilemap, NULL);

    if (TM == NULL)
    {
        KLog("Failed to unpack tilemap");
        KLog_U1("Largest free block: ", MEM_getLargestFreeBlock());
        KLog_U1("Tilemap compression: ", image->tilemap->compression);
        return NULL;
    }

    return TM;
}

bool DrawImageFG(const Image *image)
{
    if ((1 + image->tileset->numTile >= TileIdxBG_Start)) 
    {
        KLog("Warning: FG image is stomping over BG image!");
    }

    // Tileset

    TileSet *TS = UnpackTS(image);    
    if (TS == NULL) return FALSE;

    VDP_fillTileMapRect(BG_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE,FALSE, 0), 0, 0, 40, 21);
    VDP_fillTileMapRect(BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE,FALSE, 0), 0, 21, 40, 7);

    VDP_loadTileSet(TS, 1, DMA_QUEUE);

    SYS_doVBlankProcess();

    TileIdxFG_End = TS->numTile+1;

    if (image->tileset->compression != COMPRESSION_NONE) MEM_free(TS);


    // Tilemap

    TileMap *TM = UnpackTM(image);    
    if (TM == NULL) return FALSE;

    u16 offsetLo = (22 * TM->w);  // Lower half of image (shadowed)
    u8 offx = (40 - TM->w) >> 1;  // Centering image
    u8 height = TM->h - 22;       // Height of lower half of image (shadowed)
    if (height > 8) height = 8;   // Clip the height of the lower half of image

    // Set top and bottom half of tilemap (Top 20 high prio, bottom 10 low prio)
    VDP_setTileMapDataRectEx(BG_A, TM->tilemap, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, 1), offx,  0, TM->w, 22, TM->w, DMA_QUEUE);
    if (height > 0) VDP_setTileMapDataRectEx(BG_A, TM->tilemap + offsetLo, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, 1), offx, 22, TM->w, height, TM->w, DMA_QUEUE);

    SYS_doVBlankProcess();

    if (image->tilemap->compression != COMPRESSION_NONE) MEM_free(TM);

    return TRUE;
}

bool DrawImageBG(const Image *image)
{
    // Tileset

    TileSet *TS = UnpackTS(image);    
    if (TS == NULL) return FALSE;

    TileIdxBG_Start = 0x600-image->tileset->numTile;

    VDP_fillTileMapRect(BG_B, TILE_ATTR_FULL(PAL0, TRUE, FALSE,FALSE, 0), 0, 0, 40, 21);
    VDP_fillTileMapRect(BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE,FALSE, 0), 0, 21, 40, 7);

    VDP_loadTileSet(TS, TileIdxBG_Start, DMA_QUEUE);

    SYS_doVBlankProcess();

    if (TileIdxBG_Start <= TileIdxFG_End) KLog("Warning: BG image is stomping over FG image!");

    if (image->tileset->compression != COMPRESSION_NONE) MEM_free(TS);


    // Tilemap

    TileMap *TM = UnpackTM(image);    
    if (TM == NULL) return FALSE;

    u16 offsetLo = (22 * TM->w);  // Lower half of image (shadowed)
    u8 offx = (40 - TM->w) >> 1;  // Centering image
    u8 height = TM->h - 22;       // Height of lower half of image (shadowed)
    if (height > 8) height = 8;   // Clip the height of the lower half of image

    // Set top and bottom half of tilemap (Top 20 high prio, bottom 10 low prio)
    VDP_setTileMapDataRectEx(BG_B, TM->tilemap, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, TileIdxBG_Start), offx,  0, TM->w, 22, TM->w, DMA_QUEUE);
    if (height > 0) VDP_setTileMapDataRectEx(BG_B, TM->tilemap + offsetLo, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TileIdxBG_Start), offx, 22, TM->w, height, TM->w, DMA_QUEUE);

    SYS_doVBlankProcess();

    if (image->tilemap->compression != COMPRESSION_NONE) MEM_free(TM);

    return TRUE;
}

void PrintTextLine(const char *str, u8 x, u8 y, u16 delay)
{
    u8 px = x;

    for (u16 current_char = 0; current_char < 38; current_char++)
    {
        if (str[current_char] == 0) break;

        VDP_loadTileData(FONT_SCENESH.tiles+((str[current_char]-32)<<3), CharMap[y][px] , 1, DMA_QUEUE_COPY); // Many small updates...

        px++;

        WaitFrames(delay);
    }

    return;
}

// Draw portrait on textbox - Do not use images containing flipped tiles!
void DrawPortrait(const Image *image)
{
    u8 i = 0;

    TileSet *TS = UnpackTS(image);    
    if (TS == NULL) return;

    TileMap *TM = UnpackTM(image);    
    if (TM == NULL) return;

    PAL_setColors(48, image->palette->data, 12, DMA_QUEUE);

    for (u8 y = 0; y < 6; y++)
    {
    for (u8 x = 0; x < 6; x++)
    {
        VDP_loadTileData(TS->tiles+((TM->tilemap[i] & 0x7FF)*8), PortraitMap[y][x] , 1, DMA_QUEUE);
        i++;   
    }
    }

    SYS_doVBlankProcess();
    MEM_free(TS);
    MEM_free(TM);

    return;
}

void ClearTextArea()
{
    SYS_disableInts();
    DMA_doVRamFill(SPR_BANK0*32, 0x1000, 0, 1);
    DMA_waitCompletion();
    DMA_doVRamFill(SPR_BANK1*32, 0x800, 0, 1);
    DMA_waitCompletion();
    SYS_enableInts();

    return;
}

inline void WaitFrames(u16 delay)
{
    for (u8 w = 0; w < delay>>3; w++) 
    {
        //VN_DoVBlank();
        FX_RunEffect();
        SYS_doVBlankProcess();
    }
}

inline void VN_DoVBlank()
{
    FX_RunEffect();
    //XGM_nextXFrame(1);
    SYS_doVBlankProcess();
}
