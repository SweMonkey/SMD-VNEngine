#include "GameState.h"

extern VN_GameState DummyState;
extern VN_GameState MainMenuState;
extern VN_GameState OptionsState;
extern VN_GameState SceneState;
extern VN_GameState CrashState;
extern VN_GameState DebugStateState;
extern VN_GameState TextInputState;
extern VN_GameState InGameMenuState;

static VN_GameState *CurrentState = &DummyState;
static VN_GameState *PrevState = &DummyState;

static GameState CurrentStateEnum = GS_Dummy;
static GameState PrevStateEnum = GS_Dummy;


void ChangeState(GameState new_state, u8 argc, const char *argv[])
{
    PrevState = CurrentState;
    PrevStateEnum = CurrentStateEnum;

    SYS_disableInts();

    JOY_setEventHandler(NULL);
    SYS_setHIntCallback(NULL);
    SYS_setVIntCallback(NULL);
    CurrentState->Exit(new_state);

    switch (new_state)
    {
        case GS_Dummy:
            CurrentState = &DummyState;
        break;

        case GS_MainMenu:
            CurrentState = &MainMenuState;
        break;

        case GS_Options:
            CurrentState = &OptionsState;
        break;

        case GS_Scene:
            CurrentState = &SceneState;
        break;

        case GS_DEBUG:
            CurrentState = &DebugStateState;
        break;

        case GS_CRASH:
            CurrentState = &CrashState;
        break;

        case GS_TEXTINPUT:
            CurrentState = &TextInputState;
        break;

        case GS_INGAME_MENU:
            CurrentState = &InGameMenuState;
        break;
    
        default:
            CurrentState = &DummyState;
        break;
    }

    CurrentStateEnum = new_state;

    CurrentState->Enter(argc, argv);
    JOY_setEventHandler(CurrentState->Input);
    SYS_setHIntCallback(CurrentState->HBlank);
    SYS_setVIntCallback(CurrentState->VBlank);
    SYS_enableInts();
}

// Return to previous state
void RevertState()
{
    #ifdef DEBUG_STATE_MSG
    KLog("Reverting game state...");
    #endif

    VN_GameState *ShadowState = CurrentState;

    CurrentState->Exit(PrevStateEnum);

    SYS_disableInts();
    
    CurrentState = PrevState;
    CurrentStateEnum = PrevStateEnum;

    CurrentState->ReEnter();
    JOY_setEventHandler(CurrentState->Input);
    SYS_setVIntCallback(CurrentState->VBlank);

    PrevState = ShadowState;

    SYS_enableInts();
}

void ReEnterState(GameState new_state)
{
    #ifdef DEBUG_STATE_MSG
    KLog("Reverting to specific game state...");
    #endif

    VN_GameState *ShadowState = CurrentState;

    CurrentState->Exit(new_state);

    SYS_disableInts();
    
    switch (new_state)
    {
        case GS_Dummy:
            CurrentState = &DummyState;
        break;

        case GS_MainMenu:
            CurrentState = &MainMenuState;
        break;

        case GS_Options:
            CurrentState = &OptionsState;
        break;

        case GS_Scene:
            CurrentState = &SceneState;
        break;

        case GS_DEBUG:
            CurrentState = &DebugStateState;
        break;

        case GS_CRASH:
            CurrentState = &CrashState;
        break;

        case GS_TEXTINPUT:
            CurrentState = &TextInputState;
        break;

        case GS_INGAME_MENU:
            CurrentState = &InGameMenuState;
        break;
    
        default:
            CurrentState = &DummyState;
        break;
    }
    
    CurrentStateEnum = new_state;

    CurrentState->ReEnter();
    JOY_setEventHandler(CurrentState->Input);
    SYS_setVIntCallback(CurrentState->VBlank);

    PrevState = ShadowState;

    SYS_enableInts();
}

bool isCurrentState(GameState this)
{
    return CurrentStateEnum == this;
}

GameState GetPreviousState()
{
    return PrevStateEnum;
}

void StateTick()
{
    CurrentState->Run();
}
