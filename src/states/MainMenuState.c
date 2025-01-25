#include "GameState.h"
#include "../res/System_res.h"

// External main menu background
extern const Image IMG_Title;


/// @brief Begin a new game
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

static struct s_menu
{
    u8 num_entries;             // Number of entries in menu
    u8 selected_entry;          // Saved selected entry (automatic, leave at 0)
    u8 prev_menu;               // Previous menu to return to when going back (automatic, leave at 0)
    VoidCallback *entry_function;   // Function callback which is called when entering entry
    u8 next_menu[8];            // Menu number selected entry leads to (255 = Do nothing)
    const char *text[8];        // Entry text
} MainMenu[] =
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

static u8 SelectedIdx = 0;
static u8 MenuIdx = 0;
static const u8 MenuPosX = 16, MenuPosY = 22;


void DrawMenu(u8 idx)
{
    VDP_clearPlane(BG_A, TRUE);

    MainMenu[MenuIdx].selected_entry = SelectedIdx;   // Mark previous menu selection entry

    MenuIdx = idx;
    SelectedIdx = MainMenu[MenuIdx].selected_entry;   // Get menu selection entry from new menu

    for (u8 i = 0; i < MainMenu[MenuIdx].num_entries; i++)
    {
        VDP_drawText(MainMenu[MenuIdx].text[i], MenuPosX, MenuPosY+i);
    }

    VDP_drawText(">", MenuPosX-1, MenuPosY+SelectedIdx);

    VoidCallback *func = MainMenu[MenuIdx].entry_function;

    if (func != NULL) func();
}

void Enter_MainMenu(u8 argc, const char *argv[])
{
    PAL_setPalette(PAL0, palette_black, CPU);
    PAL_setPalette(PAL1, palette_black, CPU);
    PAL_setPalette(PAL2, palette_black, CPU);
    PAL_setPalette(PAL3, palette_black, CPU);

    VDP_drawImageEx(BG_B, &IMG_Title, TILE_ATTR_FULL(PAL0, 0, 0, 0, 1), 1, 1, TRUE, DMA_QUEUE);

    VDP_setTextPlane(BG_A);
    VDP_setTextPriority(1);
    VDP_setTextPalette(PAL3);
    VDP_loadFontData(FONT_NORMAL.tiles, 96, DMA);

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

void Exit_MainMenu(GameState new_state)
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
        u8 next = MainMenu[MenuIdx].next_menu[SelectedIdx];
        if (next != 255) 
        {
            MainMenu[next].prev_menu = MenuIdx;    // Return menu, when going back
            DrawMenu(next);
        }
    }

    if (changed & state & BUTTON_B)
    {
        DrawMenu(MainMenu[MenuIdx].prev_menu);
    }

    if (changed & state & BUTTON_UP)
    {
        VDP_drawText(" ", MenuPosX-1, MenuPosY+SelectedIdx);
        SelectedIdx = (SelectedIdx == 0 ? MainMenu[MenuIdx].num_entries-1 : SelectedIdx-1);
        VDP_drawText(">", MenuPosX-1, MenuPosY+SelectedIdx);
    }

    if (changed & state & BUTTON_DOWN)
    {
        VDP_drawText(" ", MenuPosX-1, MenuPosY+SelectedIdx);
        SelectedIdx = (SelectedIdx == MainMenu[MenuIdx].num_entries-1 ? 0 : SelectedIdx+1);
        VDP_drawText(">", MenuPosX-1, MenuPosY+SelectedIdx);
    }

    return;
}

void VBlank_MainMenu()
{
    return;
}

const VN_GameState MainMenuState = 
{
    Enter_MainMenu, ReEnter_MainMenu, Exit_MainMenu, Run_MainMenu, Input_MainMenu, NULL, VBlank_MainMenu
};

