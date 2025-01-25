#include "Story.h"
#include "../res/Story_res.h"
#include "../res/System_res.h"

// Dumb script to change all the text lines in a page and getting player input string
const char * const LineModScript[] =
{
"\
defv view, 1\n\
setv str1, \"You have seen this screen \"\n\
ieql view, 1\n\
setv str2, \" time\"\n\
else\n\
setv str2, \" times\"\n\
iend\n\
cats str1, view\n\
cats str1, str2\n\
setv p.name, \"Someone\"\n\
setv p.line[1], \"Hello \"\n\
cats p.line[1], i.buf\n\
cats p.line[1], \"!\"\n\
setv p.line[2], \"Line2\"\n\
setv p.line[3], \"Line3\"\n\
setv p.line[4], str1\n\
addv view, 1\n\
\0"
};

const VN_Character UnknownFemale =
{
    &IMG_FS_000,
    "Unknown Female"
};

const VN_Character CyberAnomalie =
{
    &IMG_FS_000,
    "Cyber anomalie"
};

const VN_Track Track1 =
{
    NULL, //MUS_TRACK1,
    -1
};

const VN_Track Track2 =
{
    NULL, //MUS_TRACK2,
    -1
};

const VN_Page PageInput =
{
    PAGETYPE_INPUT, // Page Type
    NULL,           // Character

    NULL,           // BG
    NULL,           // FG

    (LFX_NONE),     // BG Effects
    (LFX_NONE),     // FG Effects
    {
        "Name:\0",
        "8\0",
        "\0",
        "\0"
    },
    0,              // Switch delay
    {               // Next Page
        &Page01,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};

const VN_Page Page0 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    &IMG_BG_000,        // BG
    &IMG_FG_002,        // FG

    (LFX_FADEIN),       // BG Effects
    (LFX_FADEIN),       // FG Effects
    {
        "...\0",
        "oh hi there\0",
        "whats your name?\0",
        "\0"
    },
    5,                  // Switch delay
    {                   // Next Page
        &PageInput,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};

const VN_Page Page01 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    &IMG_BG_000,        // BG
    &IMG_FG_002,        // FG

    (LFX_FADEOUT),      // BG Effects
    (LFX_FADEOUT),      // FG Effects
    {
        "...\0",
        "...\0",
        "\0",
        "\0"
    },
    500,                // Switch delay
    {                   // Next Page
        &EntryPage,
        &Page2,
        NULL,
        NULL
    },
    LineModScript,  // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};

const VN_Page Page1 =
{
    PAGETYPE_PAGE,      // Page Type
    &CyberAnomalie,     // Character

    &IMG_BG_001,        // BG
    &IMG_FG_001,        // FG

    (LFX_FADEIN | LFX_FADEOUT | LFX_LINEGLITCH),   // BG Effects
    (LFX_FADEIN | LFX_FADEOUT | LFX_LINEGLITCH),   // FG Effects
    {
        "Lorem ipsum dolor sit amet massa nun\0",
        "Aenean nec porta lectus. Sed non dui\0",
        "Ut consectetur scelerisque felis nis\0",
        "Sed ex elit, vestibulum et urna fusc\0"
    },
    300,                // Switch delay
    {                   // Next Page
        &EntryPage,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    &Track2,        // Track
    TRUE,           // bTextbox
    TRUE,           // bHighColourBG
    TRUE,          // bAutoSwitch
};

const VN_Page EntryPage =
{
    PAGETYPE_CHOICE,    // Page Type
    &UnknownFemale,     // Character

    &IMG_BG_003,        // BG
    &IMG_FG_003,        // FG

    (LFX_FADEIN | LFX_FADEOUT),                 // BG Effects
    (LFX_FADEIN | LFX_FADEOUT),  // FG Effects  | LFX_SINEWAVE
    {
        "Choice 1 - Script string test\0",
        "Choice 2 - error\0",
        "Choice 3 - FX Tests\0",
        "Choice 4 - This page\0"
    },
    500,                // Switch delay
    {                   // Next Page
        &Page0,
        &Page1,
        &Page2,
        &EntryPage
    },
    NULL,           // Script
    NULL,//&Track1  // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};



// Fx tests

const VN_Page Page2 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    (LFX_FADEIN),       // BG Effects
    (LFX_NONE),         // FG Effects
    {
        "FadeIn BG\0",
        "\0",
        "\0",
        "\0"
    },
    250,                // Switch delay
    {                   // Next Page
        &Page3,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};
const VN_Page Page3 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    (LFX_NONE),         // BG Effects
    (LFX_FADEIN),       // FG Effects
    {
        "FadeIn FG\0",
        "\0",
        "\0",
        "\0"
    },
    250,                // Switch delay
    {                   // Next Page
        &Page4,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};

const VN_Page Page4 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    (LFX_FADEOUT),      // BG Effects
    (LFX_NONE),         // FG Effects
    {
        "FadeOut BG\0",
        "\0",
        "\0",
        "\0"
    },
    250,                // Switch delay
    {                   // Next Page
        &Page5,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};

const VN_Page Page5 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    (LFX_NONE),         // BG Effects
    (LFX_FADEOUT),      // FG Effects
    {
        "FadeOut FG\0",
        "\0",
        "\0",
        "\0"
    },
    250,                // Switch delay
    {                   // Next Page
        &Page6,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};

const VN_Page Page6 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    (LFX_SHAKELR),      // BG Effects
    (LFX_NONE),         // FG Effects
    {
        "ShakeLR BG\0",
        "\0",
        "\0",
        "\0"
    },
    250,                // Switch delay
    {                   // Next Page
        &Page7,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};

const VN_Page Page7 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    (LFX_NONE),         // BG Effects
    (LFX_SHAKELR),      // FG Effects
    {
        "ShakeLR FG\0",
        "\0",
        "\0",
        "\0"
    },
    250,                // Switch delay
    {                   // Next Page
        &Page8,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};

const VN_Page Page8 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    (LFX_LINEGLITCH),   // BG Effects
    (LFX_NONE),         // FG Effects
    {
        "LineGlitch BG\0",
        "\0",
        "\0",
        "\0"
    },
    250,                // Switch delay
    {                   // Next Page
        &Page9,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};

const VN_Page Page9 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    (LFX_NONE),         // BG Effects
    (LFX_LINEGLITCH),   // FG Effects
    {
        "LineGlitch FG\0",
        "\0",
        "\0",
        "\0"
    },
    250,                // Switch delay
    {                   // Next Page
        &Page10,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};

const VN_Page Page10 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    (LFX_NONE),         // BG Effects
    (LFX_SILHOUETTE),   // FG Effects
    {
        "Silhouette FG\0",
        "\0",
        "\0",
        "\0"
    },
    250,                // Switch delay
    {                   // Next Page
        &Page11,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};

const VN_Page Page11 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    (LFX_SINEWAVE),     // BG Effects
    (LFX_NONE),         // FG Effects
    {
        "SineWave BG\0",
        "\0",
        "\0",
        "\0"
    },
    250,                // Switch delay
    {                   // Next Page
        &Page12,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};

const VN_Page Page12 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    (LFX_NONE),         // BG Effects
    (LFX_SINEWAVE),     // FG Effects
    {
        "SineWave FG\0",
        "\0",
        "\0",
        "\0"
    },
    500,                // Switch delay
    {                   // Next Page
        &Page13,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};

const VN_Page Page13 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    (LFX_NONE),         // BG Effects
    (LFX_SLIDEOUT),     // FG Effects
    {
        "SlideOut FG\0",
        "\0",
        "\0",
        "\0"
    },
    500,                // Switch delay
    {                   // Next Page
        &Page14,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};

const VN_Page Page14 =
{
    PAGETYPE_PAGE,      // Page Type
    &UnknownFemale,     // Character

    NULL,               // BG
    NULL,               // FG

    (LFX_FADEOUT),                  // BG Effects
    (LFX_FADEOUT | LFX_SLIDEIN),    // FG Effects
    {
        "SlideIn FG\0",
        "\0",
        "\0",
        "\0"
    },
    500,                // Switch delay
    {                   // Next Page
        &EntryPage,
        NULL,
        NULL,
        NULL
    },
    NULL,           // Script
    NULL,           // Track
    TRUE,           // bTextbox
    FALSE,          // bHighColourBG
    FALSE,          // bAutoSwitch
};
