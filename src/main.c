
#include "GameState.h"

int main(bool hardReset)
{
    if (!hardReset)
    {
        SYS_hardReset();
    }

    VDP_setEnable(FALSE);
    SYS_disableInts();

    SYS_setInterruptMaskLevel(2);

    VDP_setPlaneSize(64, 32, FALSE);

    VDP_setBGAAddress(0xC000);
    VDP_setBGBAddress(0xE000);
    VDP_setWindowAddress(0xF000);
    VDP_setSpriteListAddress(0xF800);
    VDP_setHScrollTableAddress(0xFC00);

    SYS_enableInts();
    VDP_setEnable(TRUE);

    ChangeState(GS_MainMenu, 0, NULL);

    while (TRUE)
    {
        StateTick();
        SYS_doVBlankProcess();
    }

    return 0;
}
