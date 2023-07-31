
#include "GameState.h"
#include "../res/system.h"

// Temp for main menu BG
#include "../res/testres.h"

// Start a new game
void Menu_StartGame()
{
    PAL_fadeOut(0, 63, 20, FALSE);
    
    DMA_doVRamFill(0, 0xFFFF, 0, 1);
    DMA_waitCompletion();

    waitMs(1000);

    ChangeState(GS_Scene, 0, NULL);
}

void Menu_Options()
{
    ChangeState(GS_Options, 0, NULL);
}

void Menu_Debug()
{
    ChangeState(GS_DEBUG, 0, NULL);
}

static struct sMenu
{
    u8 num_entries;             // Number of entries in menu
    u8 selected_entry;          // Saved selected entry (automatic, leave at 0)
    u8 prev_menu;               // Previous menu to return to when going back (automatic, leave at 0)
    VoidCallback *entry_function;   // Function callback which is called when entering entry
    u8 next_menu[8];            // Menu number selected entry leads to (255 = Do nothing)
    const char *text[8];        // Entry text
} menu[] =
{{
    3,
    0, 0,
    NULL,
    {1, 2, 3},
    {" Start",
     "Options",
     " Debug"}
},
{
    1,
    0, 0,
    Menu_StartGame,
    {255},
    {""}
},
{
    1,
    0, 0,
    Menu_Options,
    {255},
    {""}
},
{
    1,
    0, 0,
    Menu_Debug,
    {255},
    {"Debuggy menu"}
}};

static u8 selected_idx = 0;
static u8 menu_idx = 0;
static const u8 menu_x = 16, menu_y = 22;

void DrawMenu(u8 idx)
{
    VDP_clearPlane(BG_A, TRUE);

    menu[menu_idx].selected_entry = selected_idx;   // Mark previous menu selection entry

    menu_idx = idx;
    selected_idx = menu[menu_idx].selected_entry;   // Get menu selection entry from new menu

    for (u8 i = 0; i < menu[menu_idx].num_entries; i++)
    {
        VDP_drawText(menu[menu_idx].text[i], menu_x, menu_y+i);
    }

    VDP_drawText(">", menu_x-1, menu_y+selected_idx);

    VoidCallback *func = menu[menu_idx].entry_function;

    if (func != NULL) func();
}

void Enter_MainMenu(u8 argc, char *argv[])
{
    PAL_setPalette(PAL0, palette_black, CPU);
    PAL_setPalette(PAL1, palette_black, CPU);
    PAL_setPalette(PAL2, palette_black, CPU);
    PAL_setPalette(PAL3, palette_black, CPU);

    VDP_drawImageEx(BG_B, &testBG1, TILE_ATTR_FULL(PAL0, 0, 0, 0, 1), 1, 1, TRUE, DMA_QUEUE);

    VDP_setTextPlane(BG_A);
    VDP_setTextPriority(1);
    VDP_setTextPalette(PAL3);
    VDP_loadFontData(FONT_NORMAL.tileset->tiles, 96, DMA);

    PAL_setColor(59, 0xEEE); // Special character
    PAL_setColor(60, 0x000); // Text Outline
    PAL_setColor(61, 0xEEE); // Text FG

    KLog("Entering main menu");
    DrawMenu(0);

    return;
}

void ReEnter_MainMenu()
{
    KLog("RE Entering main menu");
    Enter_MainMenu(0, NULL);

    return;
}

void Exit_MainMenu()
{
    VDP_clearPlane(BG_B, TRUE);
    VDP_clearPlane(BG_A, TRUE);

    return;
}

void Run_MainMenu()
{
    return;
}

void Input_MainMenu(u16 joy, u16 changed, u16 state)
{
    if (changed & state & BUTTON_A)
    {
        u8 next = menu[menu_idx].next_menu[selected_idx];
        if (next != 255) 
        {
            menu[next].prev_menu = menu_idx;    // Return menu, when going back
            DrawMenu(next);
        }
    }

    if (changed & state & BUTTON_B)
    {
        DrawMenu(menu[menu_idx].prev_menu);
    }

    if (changed & state & BUTTON_UP)
    {
        VDP_drawText(" ", menu_x-1, menu_y+selected_idx);
        selected_idx = (selected_idx == 0 ? menu[menu_idx].num_entries-1 : selected_idx-1);
        VDP_drawText(">", menu_x-1, menu_y+selected_idx);
    }

    if (changed & state & BUTTON_DOWN)
    {
        VDP_drawText(" ", menu_x-1, menu_y+selected_idx);
        selected_idx = (selected_idx == menu[menu_idx].num_entries-1 ? 0 : selected_idx+1);
        VDP_drawText(">", menu_x-1, menu_y+selected_idx);
    }

    return;
}

void VBlank_MainMenu()
{
    return;
}

const VN_GameState MainMenuState = 
{
    Enter_MainMenu, ReEnter_MainMenu, Exit_MainMenu, Run_MainMenu, Input_MainMenu, VBlank_MainMenu
};

